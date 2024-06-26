#include "nomp-test.h"

#define nomp_api_220_aux TOKEN_PASTE(nomp_api_220_aux, TEST_SUFFIX)
static int nomp_api_220_aux(const char *fmt, TEST_TYPE *a, int n) {
  nomp_test_check(nomp_update(a, 0, n, sizeof(TEST_TYPE), NOMP_TO));

  int         id         = -1;
  const char *clauses[4] = {"transform", "nomp_api_100", "tile", 0};
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

#define nomp_api_220_bitwise_and                                               \
  TOKEN_PASTE(nomp_api_220_bitwise_and, TEST_SUFFIX)
static int nomp_api_220_bitwise_and(unsigned n) {
  nomp_test_assert(n <= TEST_MAX_SIZE);

  TEST_TYPE a[TEST_MAX_SIZE];
  for (unsigned i = 0; i < n; i++)
    a[i] = i;

  const char *fmt =
      "void foo(%s *a, int N) {                                        \n"
      "  for (int i = 0; i < N; i++)                                   \n"
      "    a[i] = a[i] & 3;                                            \n"
      "}                                                               \n";
  nomp_api_220_aux(fmt, a, n);

  for (unsigned i = 0; i < n; i++)
    nomp_test_assert(a[i] == ((TEST_TYPE)i & 3));

  return 0;
}
#undef nomp_api_220_bitwise_and

#define nomp_api_220_bitwise_or                                                \
  TOKEN_PASTE(nomp_api_220_bitwise_or, TEST_SUFFIX)
static int nomp_api_220_bitwise_or(unsigned n) {
  nomp_test_assert(n <= TEST_MAX_SIZE);

  TEST_TYPE a[TEST_MAX_SIZE];
  for (unsigned i = 0; i < n; i++)
    a[i] = i;

  const char *fmt =
      "void foo(%s *a, int N) {                                        \n"
      "  for (int i = 0; i < N; i++)                                   \n"
      "    a[i] = i | 3;                                               \n"
      "}                                                               \n";
  nomp_api_220_aux(fmt, a, n);

  for (unsigned i = 0; i < n; i++)
    nomp_test_assert(a[i] == ((TEST_TYPE)i | 3));

  return 0;
}
#undef nomp_api_220_bitwise_or

#define nomp_api_220_bitwise_xor                                               \
  TOKEN_PASTE(nomp_api_220_bitwise_xor, TEST_SUFFIX)
static int nomp_api_220_bitwise_xor(unsigned n) {
  nomp_test_assert(n <= TEST_MAX_SIZE);

  TEST_TYPE a[TEST_MAX_SIZE];
  for (unsigned i = 0; i < n; i++)
    a[i] = i;

  const char *fmt =
      "void foo(%s *a, int N) {                                        \n"
      "  for (int i = 0; i < N; i++)                                   \n"
      "    a[i] = i ^ 3;                                               \n"
      "}                                                               \n";
  nomp_api_220_aux(fmt, a, n);

  for (unsigned i = 0; i < n; i++)
    nomp_test_assert(a[i] == ((TEST_TYPE)i ^ 3));

  return 0;
}
#undef nomp_api_220_bitwise_xor

#define nomp_api_220_bitwise_left_shift                                        \
  TOKEN_PASTE(nomp_api_220_bitwise_left_shift, TEST_SUFFIX)
static int nomp_api_220_bitwise_left_shift(unsigned n) {
  nomp_test_assert(n <= TEST_MAX_SIZE);

  TEST_TYPE a[TEST_MAX_SIZE];
  for (unsigned i = 0; i < n; i++)
    a[i] = i;

  const char *fmt =
      "void foo(%s *a, int N) {                                        \n"
      "  for (int i = 0; i < N; i++)                                   \n"
      "    a[i] = a[i] << 3;                                           \n"
      "}                                                               \n";
  nomp_api_220_aux(fmt, a, n);

  for (unsigned i = 0; i < n; i++)
    nomp_test_assert(a[i] == ((TEST_TYPE)i << 3));

  return 0;
}
#undef nomp_api_220_bitwise_left_shift

#define nomp_api_220_bitwise_right_shift                                       \
  TOKEN_PASTE(nomp_api_220_bitwise_right_shift, TEST_SUFFIX)
static int nomp_api_220_bitwise_right_shift(unsigned n) {
  nomp_test_assert(n <= TEST_MAX_SIZE);

  TEST_TYPE a[TEST_MAX_SIZE];
  for (unsigned i = 0; i < n; i++)
    a[i] = i;

  const char *fmt =
      "void foo(%s *a, int N) {                                        \n"
      "  for (int i = 0; i < N; i++)                                   \n"
      "    a[i] = a[i] >> 3;                                           \n"
      "}                                                               \n";
  nomp_api_220_aux(fmt, a, n);

  for (unsigned i = 0; i < n; i++)
    nomp_test_assert(a[i] == ((TEST_TYPE)i >> 3));

  return 0;
}
#undef nomp_api_220_bitwise_right_shift

#define nomp_api_220_bitwise_complement                                        \
  TOKEN_PASTE(nomp_api_220_bitwise_complement, TEST_SUFFIX)
static int nomp_api_220_bitwise_complement(unsigned n) {
  nomp_test_assert(n <= TEST_MAX_SIZE);

  TEST_TYPE a[TEST_MAX_SIZE];
  for (unsigned i = 0; i < n; i++)
    a[i] = i;

  const char *fmt =
      "void foo(%s *a, int N) {                                        \n"
      "  for (int i = 0; i < N; i++)                                   \n"
      "    a[i] = ~ a[i];                                              \n"
      "}                                                               \n";
  nomp_api_220_aux(fmt, a, n);

  for (unsigned i = 0; i < n; i++)
    nomp_test_assert(a[i] == (~(TEST_TYPE)i));

  return 0;
}
#undef nomp_api_220_bitwise_complement
#undef nomp_api_220_aux
