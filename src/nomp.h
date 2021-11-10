#if !defined(_LIB_NOMP_H_)
#define _LIB_NOMP_H_

#include <stddef.h>

/* Map Direction */
#define NOMP_H2D 1
#define NOMP_D2H 2
#define NOMP_ALLOC 4

/* Errors */
#define NOMP_INVALID_BACKEND -1
#define NOMP_INVALID_PLATFORM -2
#define NOMP_INVALID_DEVICE -4
#define NOMP_INVALID_TYPE -8
#define NOMP_INVALID_MAP_PTR -16
#define NOMP_INVALID_HANDLE -32
#define NOMP_MALLOC_ERROR -64
#define NOMP_INVALID_MAP_OP -128
#define NOMP_INVALID_ERROR -1024

/* Types */
#define NOMP_SHORT 0
#define NOMP_USHORT 1
#define NOMP_INT 10
#define NOMP_UINT 11
#define NOMP_LONG 20
#define NOMP_ULONG 21
#define NOMP_FLOAT 30
#define NOMP_DOUBLE 31
#define NOMP_PTR 50

/* Functions */
#ifdef __cplusplus
extern "C" {
#endif

int nomp_init(int *handle, const char *backend, const int platform,
              const int device);

int nomp_map(void *ptr, const size_t start_idx, const size_t end_idx,
             const size_t unit_size, const int op, const int handle);

int nomp_run(int *id, const char *source, const char *name, const int handle,
             const int ndim, const size_t *global, const size_t *local,
             const int nargs, ...);

int nomp_err_str(int err_id, char *buf, int buf_size);

int nomp_finalize(int *handle);

#ifdef __cplusplus
}
#endif

#endif // _LIB_NOMP_H_