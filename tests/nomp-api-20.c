#include "nomp.h"
#include <stdlib.h>

int main(int argc, char *argv[]) {
  char *backend = argc > 1 ? argv[1] : "opencl";
  int device_id = argc > 2 ? atoi(argv[2]) : 0;
  int platform_id = argc > 3 ? atoi(argv[3]) : 0;

  int err = nomp_init(backend, device_id, platform_id);
  nomp_chk(err);

  const char *knl = "double *a;\n"
                    "for (int i = 0; i < 10; i++)\n"
                    "  a[i] = i;\n";
  int id = -1, ndim = -1;
  size_t global[3], local[3];
  err = nomp_jit(&id, &ndim, global, local, knl, NULL,
                 "invalid-file.py:invalid_func");
  nomp_assert(err == NOMP_USER_CALLBACK_NOT_FOUND);

  err = nomp_finalize();
  nomp_chk(err);

  return 0;
}