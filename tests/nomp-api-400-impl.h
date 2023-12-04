#include "nomp-test.h"

#define nomp_api_400_static_1d_array_aux                                       \
  TOKEN_PASTE(nomp_api_400_static_1d_array_aux, TEST_SUFFIX)
static int nomp_api_400_static_1d_array_aux(const char *fmt, TEST_TYPE *b,
                                            TEST_TYPE *a, int n) {
  char *knl = generate_knl(fmt, 3, TOSTRING(TEST_TYPE), TOSTRING(TEST_TYPE),
                           TOSTRING(TEST_TYPE));

  int id = -1;
  const char *clauses[4] = {"transform", "nomp_api_400", "static", 0};
  nomp_test_check(nomp_jit(&id, knl, clauses, 3, "b", sizeof(TEST_TYPE),
                           NOMP_PTR, "a", sizeof(TEST_TYPE), NOMP_PTR, "n",
                           sizeof(int), NOMP_INT));
  nomp_free(&knl);

  nomp_test_check(nomp_run(id, b, a, &n));

  return 0;
}

#define nomp_api_400_static_1d_array                                           \
  TOKEN_PASTE(nomp_api_400_static_1d_array, TEST_SUFFIX)
static int nomp_api_400_static_1d_array(int n) {
  nomp_test_assert(n <= TEST_MAX_SIZE / 32);

  const char *knl_fmt =
      "void foo(%s *b, const %s *a, int n) {                           \n"
      "  for (int i = 0; i < n; i++) {                                 \n"
      "    %s s[32];                                                   \n"
      "    for (int j = 0; j < 32; j++)                                \n"
      "      s[j] = a[i * 32 + j];                                     \n"
      "    for (int j = 0; j < 32; j++)                                \n"
      "      s[j] += s[j];                                             \n"
      "    for (int j = 0; j < 32; j++)                                \n"
      "      b[i * 32 + j] = s[j];                                     \n"
      "  }                                                             \n"
      "}                                                               \n";

  TEST_TYPE a[TEST_MAX_SIZE], b[TEST_MAX_SIZE];
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < 32; j++)
      a[i * 32 + j] = i;
  }

  nomp_test_check(nomp_update(a, 0, TEST_MAX_SIZE, sizeof(TEST_TYPE), NOMP_TO));
  nomp_test_check(nomp_update(b, 0, TEST_MAX_SIZE, sizeof(TEST_TYPE), NOMP_TO));

  nomp_api_400_static_1d_array_aux(knl_fmt, b, a, n);

  nomp_test_check(
      nomp_update(b, 0, TEST_MAX_SIZE, sizeof(TEST_TYPE), NOMP_FROM));
  nomp_test_check(
      nomp_update(b, 0, TEST_MAX_SIZE, sizeof(TEST_TYPE), NOMP_FREE));
  nomp_test_check(
      nomp_update(a, 0, TEST_MAX_SIZE, sizeof(TEST_TYPE), NOMP_FREE));

#if defined(TEST_TOL)
  for (int i = 0; i < 32 * n; i++)
    nomp_test_assert(fabs(b[i] - (TEST_TYPE)(2 * a[i])) < TEST_TOL);
#else
  for (int i = 0; i < 32 * n; i++)
    nomp_test_assert(b[i] == (TEST_TYPE)(2 * a[i]));
#endif

  return 0;
}
#undef nomp_api_400_static_1d_array
#undef nomp_api_400_static_1d_array_aux