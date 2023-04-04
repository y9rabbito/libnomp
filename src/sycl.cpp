#include "nomp-impl.h"
#include "nomp-jit.h"
#include <CL/sycl.hpp>

static const char *ERR_STR_SYCL_FAILURE = "SYCL backend failed with error: %s.";

#define chk_sycl(try_body)                                                     \
  try {                                                                        \
    try_body                                                                   \
  } catch (const std::exception &ex) {                                         \
    return nomp_set_log(NOMP_SYCL_FAILURE, NOMP_ERROR, ERR_STR_SYCL_FAILURE,   \
                        ex.what());                                            \
  }

struct sycl_backend {
  sycl::device device_id;
  sycl::queue queue;
  sycl::context ctx;
  char *compiler;
  char *compiler_flags;
};

struct sycl_prog {
  int sycl_id;
};

static int sycl_update(struct backend *bnd, struct mem *m, const int op) {
  struct sycl_backend *sycl = (struct sycl_backend *)bnd->bptr;

  if (op & NOMP_ALLOC) {
    chk_sycl(m->bptr = sycl::malloc_device((m->idx1 - m->idx0) * m->usize,
                                           sycl->device_id, sycl->ctx););
  }

  if (op & NOMP_TO) {
    chk_sycl({
      sycl->queue.memcpy(m->bptr, (char *)(m->hptr) + m->usize * m->idx0,
                         (m->idx1 - m->idx0) * m->usize);
      sycl->queue.wait();
    });
  } else if (op == NOMP_FROM) {
    chk_sycl({
      sycl->queue.memcpy((char *)(m->hptr) + m->usize * m->idx0, m->bptr,
                         (m->idx1 - m->idx0) * m->usize);
      sycl->queue.wait();
    });
  } else if (op == NOMP_FREE) {
    chk_sycl({
      sycl::free(m->bptr, sycl->ctx);
      m->bptr = NULL;
    });
  }

  return 0;
}

static int sycl_knl_free(struct prog *prg) {
  struct sycl_prog *sycl_prg = (struct sycl_prog *)prg->bptr;
  int err = jit_free(&sycl_prg->sycl_id);
  nomp_free(prg->bptr), prg->bptr = NULL;

  return err;
}

static int sycl_knl_build(struct backend *bnd, struct prog *prg,
                          const char *source, const char *name) {
  struct sycl_backend *sycl = (struct sycl_backend *)bnd->bptr;
  struct sycl_prog *sycl_prg = nomp_calloc(struct sycl_prog, 1);
  sycl_prg->sycl_id = -1;
  prg->bptr = (void *)sycl_prg;

  char cwd[BUFSIZ];
  if (getcwd(cwd, BUFSIZ) == NULL) {
    return nomp_set_log(NOMP_SYCL_FAILURE, NOMP_ERROR, ERR_STR_SYCL_FAILURE,
                        "Failed to get current working directory.");
  }

  char *wkdir = nomp_str_cat(3, BUFSIZ, cwd, "/", ".nomp_jit_cache");
  int err = jit_compile(&sycl_prg->sycl_id, source, sycl->compiler,
                        sycl->compiler_flags, name, wkdir, "nomp_sycl_src.cpp",
                        "lib.so");
  nomp_free(wkdir);
  return err;
}

static int sycl_knl_run(struct backend *bnd, struct prog *prg, va_list args) {
  struct sycl_backend *sycl = (struct sycl_backend *)bnd->bptr;
  struct sycl_prog *sycl_prg = (struct sycl_prog *)prg->bptr;

  struct mem *m;
  size_t size;
  void *arg_list[MAX_KNL_ARGS];
  for (int i = 0; i < prg->nargs; i++) {
    const char *var = va_arg(args, const char *);
    int type = va_arg(args, int);
    size = va_arg(args, size_t);
    void *p = va_arg(args, void *);
    switch (type) {
    case NOMP_INT:
    case NOMP_FLOAT:
      break;
    case NOMP_PTR:
      m = mem_if_mapped(p);
      if (m == NULL) {
        return nomp_set_log(NOMP_USER_MAP_PTR_IS_INVALID, NOMP_ERROR,
                            ERR_STR_USER_MAP_PTR_IS_INVALID, p);
      }
      p = m->bptr;
      break;
    default:;
      return nomp_set_log(
          NOMP_USER_KNL_ARG_TYPE_IS_INVALID, NOMP_ERROR,
          "Kernel argument type %d passed to libnomp is not valid.", type);
      break;
    }
    arg_list[i] = p;
  }

  arg_list[prg->nargs] = (void *)&sycl->queue;

  size_t global[3];
  for (unsigned i = 0; i < 3; i++)
    global[i] = prg->global[i] * prg->local[i];

  sycl::nd_range<3> nd_range =
      sycl::nd_range(sycl::range(global[0], global[1], global[2]),
                     sycl::range(prg->local[0], prg->local[1], prg->local[2]));
  arg_list[prg->nargs + 1] = (void *)&nd_range;
  return jit_run(sycl_prg->sycl_id, arg_list);
}

static int sycl_sync(struct backend *bnd) {
  struct sycl_backend *sycl = (struct sycl_backend *)bnd->bptr;
  sycl->queue.wait();
  return 0;
}

static int sycl_finalize(struct backend *bnd) {
  struct sycl_backend *sycl = (struct sycl_backend *)bnd->bptr;
  nomp_free(sycl->compiler), sycl->compiler = NULL;
  nomp_free(sycl->compiler_flags), sycl->compiler_flags = NULL;
  nomp_free(bnd->bptr), bnd->bptr = NULL;

  return 0;
}

static char *copy_env(const char *name, size_t size) {
  const char *tmp = getenv(name);
  if (tmp)
    return strndup(tmp, size);
  return NULL;
}

static int check_env(struct sycl_backend *sycl) {
  char *tmp;
  if (tmp = copy_env("NOMP_SYCL_CC", MAX_BUFSIZ)) {
    sycl->compiler = strndup(tmp, MAX_BUFSIZ + 1), nomp_free(tmp);
  } else {
    return nomp_set_log(NOMP_SYCL_FAILURE, NOMP_ERROR,
                        "SYCL compiler NOMP_SYCL_CC must be set.");
  }
  if (tmp = copy_env("NOMP_SYCL_CFLAGS", MAX_BUFSIZ)) {
    sycl->compiler_flags = strndup(tmp, MAX_BUFSIZ + 1), nomp_free(tmp);
  } else {
    return nomp_set_log(NOMP_SYCL_FAILURE, NOMP_ERROR,
                        "SYCL compiler flags NOMP_SYCL_CFLAGS must be set.");
  }
  return 0;
}

int sycl_init(struct backend *bnd, const int platform_id, const int device_id) {
  struct sycl_backend *sycl = nomp_calloc(struct sycl_backend, 1);
  bnd->bptr = (void *)sycl;

  auto sycl_platforms = sycl::platform().get_platforms();
  if (platform_id < 0 | platform_id >= sycl_platforms.size()) {
    return nomp_set_log(NOMP_USER_INPUT_IS_INVALID, NOMP_ERROR,
                        "Platform id %d provided to libnomp is not valid.",
                        platform_id);
  }

  auto sycl_pdevices = sycl_platforms[platform_id].get_devices();
  if (device_id < 0 || device_id >= sycl_pdevices.size()) {
    return nomp_set_log(NOMP_USER_INPUT_IS_INVALID, NOMP_ERROR,
                        ERR_STR_USER_DEVICE_IS_INVALID, device_id);
  }

  sycl->device_id = sycl_pdevices[device_id];
  sycl->ctx = sycl::context(sycl->device_id);
  sycl->queue = sycl::queue(sycl->ctx, sycl->device_id);
  check_env(sycl);

  bnd->update = sycl_update;
  bnd->knl_build = sycl_knl_build;
  bnd->knl_run = sycl_knl_run;
  bnd->knl_free = sycl_knl_free;
  bnd->sync = sycl_sync;
  bnd->finalize = sycl_finalize;

  return 0;
}
