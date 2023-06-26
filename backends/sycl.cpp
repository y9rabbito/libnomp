#include "nomp-impl.h"
#include "nomp-jit.h"
#include <CL/sycl.hpp>

static const char *ERR_STR_SYCL_FAILURE = "SYCL backend failed with error: %s.";

#define chk_sycl(try_body)                                                     \
  try {                                                                        \
    try_body                                                                   \
  } catch (const std::exception &ex) {                                         \
    return nomp_log(NOMP_SYCL_FAILURE, NOMP_ERROR, ERR_STR_SYCL_FAILURE,       \
                    ex.what());                                                \
  }

struct sycl_backend {
  sycl::device device_id;
  sycl::queue queue;
  sycl::context ctx;
  char *compiler, *compiler_flags;
};

struct sycl_prog {
  int sycl_id;
};

static int sycl_update(struct nomp_backend_t *bnd, struct nomp_mem_t *m,
                       const nomp_map_direction_t op, size_t start, size_t end,
                       size_t usize) {
  struct sycl_backend *sycl = (struct sycl_backend *)bnd->bptr;

  if (op & NOMP_ALLOC) {
    chk_sycl(m->bptr = sycl::malloc_device(NOMP_MEM_BYTES(start, end, usize),
                                           sycl->device_id, sycl->ctx););
  }

  if (op & NOMP_TO) {
    chk_sycl({
      sycl->queue.memcpy((char *)(m->bptr) +
                             NOMP_MEM_OFFSET(start - m->idx0, usize),
                         (char *)(m->hptr) + NOMP_MEM_OFFSET(start, usize),
                         NOMP_MEM_BYTES(start, end, usize));
      sycl->queue.wait();
    });
  } else if (op == NOMP_FROM) {
    chk_sycl({
      sycl->queue.memcpy((char *)(m->hptr) + NOMP_MEM_OFFSET(start, usize),
                         (char *)(m->bptr) +
                             NOMP_MEM_OFFSET(start - m->idx0, usize),
                         NOMP_MEM_BYTES(start, end, usize));
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

static int sycl_knl_free(struct nomp_prog_t *prg) {
  struct sycl_prog *sycl_prg = (struct sycl_prog *)prg->bptr;

  if (sycl_prg)
    return nomp_jit_free(&sycl_prg->sycl_id);
  nomp_free(&prg->bptr);
  return 0;
}

static int sycl_knl_build(struct nomp_backend_t *bnd, struct nomp_prog_t *prg,
                          const char *source, const char *name) {
  struct sycl_backend *sycl = (struct sycl_backend *)bnd->bptr;
  struct sycl_prog *sycl_prg = nomp_calloc(struct sycl_prog, 1);
  sycl_prg->sycl_id = -1;
  prg->bptr = (void *)sycl_prg;

  char cwd[BUFSIZ];
  if (getcwd(cwd, BUFSIZ) == NULL) {
    return nomp_log(NOMP_SYCL_FAILURE, NOMP_ERROR, ERR_STR_SYCL_FAILURE,
                    "Failed to get current working directory.");
  }

  char *wkdir = nomp_str_cat(3, BUFSIZ, cwd, "/", ".nomp_jit_cache");
  int err = nomp_jit_compile(&sycl_prg->sycl_id, source, sycl->compiler,
                             sycl->compiler_flags, name, wkdir,
                             "nomp_sycl_src.cpp", "lib.so");
  nomp_free(&wkdir);
  return err;
}

static int sycl_knl_run(struct nomp_backend_t *bnd, struct nomp_prog_t *prg) {
  struct sycl_backend *sycl = (struct sycl_backend *)bnd->bptr;
  struct sycl_prog *sycl_prg = (struct sycl_prog *)prg->bptr;

  void *arg_list[NOMP_MAX_KNL_ARGS];
  for (unsigned i = 0; i < prg->nargs; i++)
    arg_list[i] = prg->args[i].ptr;
  arg_list[prg->nargs] = (void *)&sycl->queue;

  sycl::nd_range<3> nd_range =
      sycl::nd_range(sycl::range(prg->gws[0], prg->gws[1], prg->gws[2]),
                     sycl::range(prg->local[0], prg->local[1], prg->local[2]));
  arg_list[prg->nargs + 1] = (void *)&nd_range;
  return nomp_jit_run(sycl_prg->sycl_id, arg_list);
}

static int sycl_sync(struct nomp_backend_t *bnd) {
  struct sycl_backend *sycl = (struct sycl_backend *)bnd->bptr;
  sycl->queue.wait();
  return 0;
}

static int sycl_finalize(struct nomp_backend_t *bnd) {
  struct sycl_backend *sycl = (struct sycl_backend *)bnd->bptr;

  if (sycl)
    nomp_free(&sycl->compiler), nomp_free(&sycl->compiler_flags);
  nomp_free(&bnd->bptr);

  return 0;
}

static int check_env(struct sycl_backend *sycl) {
  char *tmp = NULL;
  if (tmp = getenv("NOMP_SYCL_CC")) {
    // FIXME: This should be something like pathlen().
    sycl->compiler = strndup(tmp, NOMP_MAX_BUFSIZ);
  } else {
    return nomp_log(NOMP_SYCL_FAILURE, NOMP_ERROR,
                    "SYCL compiler NOMP_SYCL_CC must be set.");
  }
  if (tmp = getenv("NOMP_SYCL_CFLAGS")) {
    sycl->compiler_flags = strndup(tmp, NOMP_MAX_CFLAGS_SIZE);
  } else {
    return nomp_log(NOMP_SYCL_FAILURE, NOMP_ERROR,
                    "SYCL compiler flags NOMP_SYCL_CFLAGS must be set.");
  }
  return 0;
}

int sycl_init(struct nomp_backend_t *bnd, const int platform_id,
              const int device_id) {
  struct sycl_backend *sycl = nomp_calloc(struct sycl_backend, 1);
  bnd->bptr = (void *)sycl;

  auto sycl_platforms = sycl::platform().get_platforms();
  if (platform_id < 0 | platform_id >= sycl_platforms.size()) {
    return nomp_log(NOMP_USER_INPUT_IS_INVALID, NOMP_ERROR,
                    "Platform id %d provided to libnomp is not valid.",
                    platform_id);
  }

  auto sycl_pdevices = sycl_platforms[platform_id].get_devices();
  if (device_id < 0 || device_id >= sycl_pdevices.size()) {
    return nomp_log(NOMP_USER_INPUT_IS_INVALID, NOMP_ERROR,
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