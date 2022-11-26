#include "nomp-test.h"

#define nomp_api_223_add_aux TOKEN_PASTE(nomp_api_223_add_aux, TEST_SUFFIX)
int nomp_api_223_add_aux(TEST_TYPE *a, TEST_TYPE *b, int N) {
  const char *knl_fmt =
      "void foo(%s *a, %s *b, int N) {                        \n"
      "  for (int i = 0; i < N; i++)                          \n"
      "    a[i] += b[i];                                      \n"
      "}                                                      \n";

  size_t len = strlen(knl_fmt) + 2 * strlen(TOSTRING(TEST_TYPE)) + 1;
  char *knl = tcalloc(char, len);
  snprintf(knl, len, knl_fmt, TOSTRING(TEST_TYPE), TOSTRING(TEST_TYPE));

  static int id = -1;
  const char *clauses[4] = {"transform", "nomp-api-200", "transform", 0};
  int err = nomp_jit(&id, knl, clauses);
  nomp_chk(err);

  err = nomp_run(id, 3, "a", NOMP_PTR, sizeof(TEST_TYPE), a, "b", NOMP_PTR,
                 sizeof(TEST_TYPE), b, "N", NOMP_INTEGER, sizeof(int), &N);
  nomp_chk(err);

  tfree(knl);
  return 0;
}

#define nomp_api_223_add TOKEN_PASTE(nomp_api_223_add, TEST_SUFFIX)
int nomp_api_223_add(int n) {
  nomp_assert(n <= 20);
  TEST_TYPE a[20], b[20];
  for (unsigned i = 0; i < n; i++)
    a[i] = n - i, b[i] = i;

  int err = nomp_update(a, 0, n, sizeof(TEST_TYPE), NOMP_TO);
  nomp_chk(err);
  err = nomp_update(b, 0, n, sizeof(TEST_TYPE), NOMP_TO);
  nomp_chk(err);

  nomp_api_223_add_aux(a, b, n);

  err = nomp_update(a, 0, n, sizeof(TEST_TYPE), NOMP_FROM);
  nomp_chk(err);

#if defined(TEST_TOL)
  for (unsigned i = 0; i < n; i++)
    nomp_assert(fabs(a[i] - n) < TEST_TOL);
#else
  for (unsigned i = 0; i < n; i++)
    nomp_assert(a[i] == n);
#endif

  err = nomp_update(a, 0, n, sizeof(TEST_TYPE), NOMP_FREE);
  nomp_chk(err);
  err = nomp_update(b, 0, n, sizeof(TEST_TYPE), NOMP_FREE);
  nomp_chk(err);

  return 0;
}
#undef nomp_api_223_add
#undef nomp_api_223_add_aux

#define nomp_api_223_sub_aux TOKEN_PASTE(nomp_api_223_sub_aux, TEST_SUFFIX)
int nomp_api_223_sub_aux(TEST_TYPE *a, TEST_TYPE *b, int N) {
  const char *knl_fmt =
      "void foo(%s *a, %s *b, int N) {                        \n"
      "  for (int i = 0; i < N; i++)                          \n"
      "    a[i] -= b[i] + 1;                                  \n"
      "}                                                      \n";

  size_t len = strlen(knl_fmt) + 2 * strlen(TOSTRING(TEST_TYPE)) + 1;
  char *knl = tcalloc(char, len);
  snprintf(knl, len, knl_fmt, TOSTRING(TEST_TYPE), TOSTRING(TEST_TYPE));

  static int id = -1;
  const char *clauses[4] = {"transform", "nomp-api-200", "transform", 0};
  int err = nomp_jit(&id, knl, clauses);
  nomp_chk(err);

  err = nomp_run(id, 3, "a", NOMP_PTR, sizeof(TEST_TYPE), a, "b", NOMP_PTR,
                 sizeof(TEST_TYPE), b, "N", NOMP_INTEGER, sizeof(int), &N);
  nomp_chk(err);

  tfree(knl);
  return 0;
}

#define nomp_api_223_sub TOKEN_PASTE(nomp_api_223_sub, TEST_SUFFIX)
int nomp_api_223_sub(int n) {
  nomp_assert(n <= 20);
  TEST_TYPE a[20], b[20];
  for (unsigned i = 0; i < n; i++)
    a[i] = n + i, b[i] = i;

  int err = nomp_update(a, 0, n, sizeof(TEST_TYPE), NOMP_TO);
  nomp_chk(err);
  err = nomp_update(b, 0, n, sizeof(TEST_TYPE), NOMP_TO);
  nomp_chk(err);

  nomp_api_223_sub_aux(a, b, n);

  err = nomp_update(a, 0, n, sizeof(TEST_TYPE), NOMP_FROM);
  nomp_chk(err);

#if defined(TEST_TOL)
  for (unsigned i = 0; i < n; i++)
    nomp_assert(fabs(a[i] - n + 1) < TEST_TOL);
#else
  for (unsigned i = 0; i < n; i++)
    nomp_assert(a[i] == n - 1);
#endif

  err = nomp_update(a, 0, n, sizeof(TEST_TYPE), NOMP_FREE);
  nomp_chk(err);
  err = nomp_update(b, 0, n, sizeof(TEST_TYPE), NOMP_FREE);
  nomp_chk(err);

  return 0;
}
#undef nomp_api_223_sub
#undef nomp_api_223_sub_aux

#define nomp_api_223_mul_aux TOKEN_PASTE(nomp_api_223_mul_aux, TEST_SUFFIX)
int nomp_api_223_mul_aux(TEST_TYPE *a, TEST_TYPE *b, int N) {
  const char *knl_fmt =
      "void foo(%s *a, %s *b, int N) {                        \n"
      "  for (int i = 0; i < N; i++)                          \n"
      "    a[i] *= b[i] + 1;                                  \n"
      "}                                                      \n";

  size_t len = strlen(knl_fmt) + 2 * strlen(TOSTRING(TEST_TYPE)) + 1;
  char *knl = tcalloc(char, len);
  snprintf(knl, len, knl_fmt, TOSTRING(TEST_TYPE), TOSTRING(TEST_TYPE));

  static int id = -1;
  const char *clauses[4] = {"transform", "nomp-api-200", "transform", 0};
  int err = nomp_jit(&id, knl, clauses);
  nomp_chk(err);

  err = nomp_run(id, 3, "a", NOMP_PTR, sizeof(TEST_TYPE), a, "b", NOMP_PTR,
                 sizeof(TEST_TYPE), b, "N", NOMP_INTEGER, sizeof(int), &N);
  nomp_chk(err);

  tfree(knl);
  return 0;
}

#define nomp_api_223_mul TOKEN_PASTE(nomp_api_223_mul, TEST_SUFFIX)
int nomp_api_223_mul(int n) {
  nomp_assert(n <= 20);
  TEST_TYPE a[20], b[20];
  for (unsigned i = 0; i < n; i++)
    a[i] = n - i, b[i] = i;

  int err = nomp_update(a, 0, n, sizeof(TEST_TYPE), NOMP_TO);
  nomp_chk(err);
  err = nomp_update(b, 0, n, sizeof(TEST_TYPE), NOMP_TO);
  nomp_chk(err);

  nomp_api_223_mul_aux(a, b, n);

  err = nomp_update(a, 0, n, sizeof(TEST_TYPE), NOMP_FROM);
  nomp_chk(err);

#if defined(TEST_TOL)
  for (unsigned i = 0; i < n; i++)
    nomp_assert(fabs(a[i] - (n - i) * (i + 1)) < TEST_TOL);
#else
  for (unsigned i = 0; i < n; i++)
    nomp_assert(a[i] == (n - i) * (i + 1));
#endif

  err = nomp_update(a, 0, n, sizeof(TEST_TYPE), NOMP_FREE);
  nomp_chk(err);
  err = nomp_update(b, 0, n, sizeof(TEST_TYPE), NOMP_FREE);
  nomp_chk(err);

  return 0;
}
#undef nomp_api_223_mul
#undef nomp_api_223_mul_aux