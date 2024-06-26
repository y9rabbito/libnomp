#include "nomp-test.h"

#define nomp_api_225_aux TOKEN_PASTE(nomp_api_225_aux, TEST_SUFFIX)
static int nomp_api_225_aux(const char *fmt, TEST_TYPE *a, int n) {
  nomp_test_check(nomp_update(a, 0, n, sizeof(TEST_TYPE), NOMP_TO));

  int         id         = -1;
  const char *clauses[4] = {"transform", "nomp_api_225", "tile_outer", 0};
  char       *knl        = generate_knl(fmt, 1, TOSTRING(TEST_TYPE));
  nomp_test_check(nomp_jit(&id, knl, clauses, 2, "a", sizeof(TEST_TYPE),
                           NOMP_PTR, "N", sizeof(int), NOMP_INT));
  nomp_free(&knl);

  nomp_test_check(nomp_run(id, a, &n));

  nomp_test_check(nomp_sync());

  nomp_test_check(nomp_update(a, 0, n, sizeof(TEST_TYPE), NOMP_FROM));
  nomp_test_check(nomp_update(a, 0, n, sizeof(TEST_TYPE), NOMP_FREE));

  return 0;
}

#define nomp_api_225_logical_ops                                               \
  TOKEN_PASTE(nomp_api_225_logical_ops, TEST_SUFFIX)
static int nomp_api_225_logical_ops(unsigned n) {
  nomp_test_assert(n <= TEST_MAX_SIZE);

  TEST_TYPE a[TEST_MAX_SIZE];
  for (unsigned i = 0; i < n; i++)
    a[i] = 0;

  const char *fmt =
      "void foo(%s *a,  int N) {                                       \n"
      "  for (int i = 0; i < N; i++) {                                 \n"
      "    int t = 0;                                                  \n"
      "    for (int j = 0; j < 10; j++) {                              \n"
      "      if ((!(j < 3) && (j < 5)) || j == 1)                      \n"
      "        continue;                                               \n"
      "      t += 1;                                                   \n"
      "    }                                                           \n"
      "    a[i] = t;                                                   \n"
      "  }                                                             \n"
      "}                                                               \n";
  nomp_api_225_aux(fmt, a, n);

#if defined(TEST_TOL)
  for (unsigned i = 0; i < n; i++)
    nomp_test_assert(fabs(a[i] - 7) < TEST_TOL);
#else
  for (unsigned i = 0; i < n; i++)
    nomp_test_assert(a[i] == 7);
#endif

  return 0;
}
#undef nomp_api_225_logical_ops

#define nomp_api_225_ternary_ops                                               \
  TOKEN_PASTE(nomp_api_225_ternary_ops, TEST_SUFFIX)
static int nomp_api_225_ternary_ops(unsigned n) {
  nomp_test_assert(n <= TEST_MAX_SIZE);

  TEST_TYPE a[TEST_MAX_SIZE];
  for (unsigned i = 0; i < n; i++)
    a[i] = 0;

  const char *fmt =
      "void foo(%s *a,  int N) {                                       \n"
      "  for (int i = 0; i < N; i++) {                                 \n"
      "    int t = 0;                                                  \n"
      "    for (int j = 0; j < 10; j++) {                              \n"
      "      t = ((!(j < 3) && (j < 5)) || j == 1) ? t : t + 1;        \n"
      "      t += ((!(j < 3) && (j < 5)) || j == 1) ? 0 : 1;           \n"
      "    }                                                           \n"
      "    a[i] = t;                                                   \n"
      "  }                                                             \n"
      "}                                                               \n";
  nomp_api_225_aux(fmt, a, n);

#if defined(TEST_TOL)
  for (unsigned i = 0; i < n; i++)
    nomp_test_assert(fabs(a[i] - 14) < TEST_TOL);
#else
  for (unsigned i = 0; i < n; i++)
    nomp_test_assert(a[i] == 14);
#endif

  return 0;
}
#undef nomp_api_225_ternary_ops
#undef nomp_api_225_aux
