#include "nomp-test.h"

#define free_before_mapping                                                    \
  TOKEN_PASTE(nomp_api_050_free_before_mapping, TEST_SUFFIX)
static int free_before_mapping(unsigned s, unsigned e) {
  nomp_test_assert(e <= TEST_MAX_SIZE);

  TEST_TYPE a[TEST_MAX_SIZE] = {0};

  int err = nomp_update(a, s, e, sizeof(TEST_TYPE), NOMP_FREE);
  nomp_test_assert(nomp_get_err_no(err) == NOMP_USER_MAP_OP_IS_INVALID);

  char *desc = nomp_get_err_str(err);
  int   eq   = logcmp(
      desc, "\\[Error\\] "
                ".*libnomp\\/src\\/nomp.c:[0-9]* NOMP_FREE or NOMP_FROM can only "
                "be called on a pointer which is already on the device.");
  nomp_free(&desc);
  nomp_test_assert(eq);

  return 0;
}
#undef free_before_mapping

#define d2h_before_h2d TOKEN_PASTE(nomp_api_050_d2h_before_h2d, TEST_SUFFIX)
static int d2h_before_h2d(unsigned s, unsigned e) {
  nomp_test_assert(e <= TEST_MAX_SIZE);

  TEST_TYPE a[TEST_MAX_SIZE] = {0};
  int       err = nomp_update(a, s, e, sizeof(TEST_TYPE), NOMP_FROM);
  nomp_test_assert(nomp_get_err_no(err) == NOMP_USER_MAP_OP_IS_INVALID);

  char *desc = nomp_get_err_str(err);
  int   eq   = logcmp(
      desc, "\\[Error\\] "
                ".*libnomp\\/src\\/nomp.c:[0-9]* NOMP_FREE or NOMP_FROM can only "
                "be called on a pointer which is already on the device.");
  nomp_free(&desc);
  nomp_test_assert(eq);

  return 0;
}
#undef d2h_before_h2d

#define multiple_h2d_calls                                                     \
  TOKEN_PASTE(nomp_api_050_multiple_h2d_calls, TEST_SUFFIX)
static int multiple_h2d_calls(unsigned s, unsigned e) {
  nomp_test_assert(e <= TEST_MAX_SIZE);

  TEST_TYPE a[TEST_MAX_SIZE] = {0};

  nomp_test_check(nomp_update(a, s, e, sizeof(TEST_TYPE), NOMP_TO));
  nomp_test_check(nomp_update(a, s, e, sizeof(TEST_TYPE), NOMP_TO));
  nomp_test_check(nomp_update(a, s, e, sizeof(TEST_TYPE), NOMP_FREE));

  return 0;
}
#undef multiple_h2d_calls

#define multiple_d2h_calls                                                     \
  TOKEN_PASTE(nomp_api_050_multiple_d2h_calls, TEST_SUFFIX)
static int multiple_d2h_calls(unsigned s, unsigned e) {
  nomp_test_assert(e <= TEST_MAX_SIZE);

  TEST_TYPE a[TEST_MAX_SIZE] = {0};

  nomp_test_check(nomp_update(a, s, e, sizeof(TEST_TYPE), NOMP_TO));

  nomp_test_check(nomp_update(a, s, e, sizeof(TEST_TYPE), NOMP_FROM));
  nomp_test_check(nomp_update(a, s, e, sizeof(TEST_TYPE), NOMP_FROM));
  nomp_test_check(nomp_update(a, s, e, sizeof(TEST_TYPE), NOMP_FREE));

  return 0;
}
#undef multiple_d2h_calls

#define d2h_after_h2d TOKEN_PASTE(nomp_api_050_d2h_after_h2d, TEST_SUFFIX)
static int d2h_after_h2d(unsigned s, unsigned e) {
  nomp_test_assert(e <= TEST_MAX_SIZE);

  TEST_TYPE a[TEST_MAX_SIZE] = {0};

  for (unsigned i = s; i < e; i++)
    a[i] = i;
  nomp_test_check(nomp_update(a, s, e, sizeof(TEST_TYPE), NOMP_TO));

  for (unsigned i = s; i < e; i++)
    a[i] = 0;
  nomp_test_check(nomp_update(a, s, e, sizeof(TEST_TYPE), NOMP_FROM));

#if defined(TEST_TOL)
  for (unsigned i = 0; i < s; i++)
    nomp_test_assert(fabs(a[i]) < TEST_TOL);
  for (unsigned i = s; i < e; i++)
    nomp_test_assert(fabs(a[i] - i) < TEST_TOL);
  for (unsigned i = e; i < TEST_MAX_SIZE; i++)
    nomp_test_assert(fabs(a[i]) < TEST_TOL);
#else
  for (unsigned i = 0; i < s; i++)
    nomp_test_assert(a[i] == 0);
  for (unsigned i = s; i < e; i++)
    nomp_test_assert(a[i] == (TEST_TYPE)i);
  for (unsigned i = e; i < TEST_MAX_SIZE; i++)
    nomp_test_assert(a[i] == 0);
#endif

  nomp_test_check(nomp_update(a, s, e, sizeof(TEST_TYPE), NOMP_FREE));

  return 0;
}
#undef d2h_after_h2d

#define free_after_h2d TOKEN_PASTE(nomp_api_050_free_after_h2d, TEST_SUFFIX)
static int free_after_h2d(unsigned s, unsigned e) {
  nomp_test_assert(e <= TEST_MAX_SIZE);

  TEST_TYPE a[TEST_MAX_SIZE] = {0};
  nomp_test_check(nomp_update(a, s, e, sizeof(TEST_TYPE), NOMP_TO));

  nomp_test_check(nomp_update(a, s, e, sizeof(TEST_TYPE), NOMP_FREE));

  return 0;
}
#undef free_after_h2d

#define free_after_d2h TOKEN_PASTE(nomp_api_050_free_after_d2h, TEST_SUFFIX)
static int free_after_d2h(unsigned s, unsigned e) {
  nomp_test_assert(e <= TEST_MAX_SIZE);

  TEST_TYPE a[TEST_MAX_SIZE] = {0};
  nomp_test_check(nomp_update(a, s, e, sizeof(TEST_TYPE), NOMP_TO));

  nomp_test_check(nomp_update(a, s, e, sizeof(TEST_TYPE), NOMP_FROM));
  nomp_test_check(nomp_update(a, s, e, sizeof(TEST_TYPE), NOMP_FREE));

  return 0;
}
#undef free_after_d2h

#define in_range_d2h TOKEN_PASTE(nomp_api_050_in_range_d2h, TEST_SUFFIX)
static int in_range_d2h(unsigned lb, unsigned ub, unsigned s, unsigned e) {
  nomp_test_assert(ub - lb <= TEST_MAX_SIZE);

  TEST_TYPE a[TEST_MAX_SIZE] = {0};

  for (unsigned i = lb; i < ub; i++)
    a[i] = i;
  nomp_test_check(nomp_update(a, lb, ub, sizeof(TEST_TYPE), NOMP_TO));

  for (unsigned i = lb; i < ub; i++)
    a[i] = 0;
  nomp_test_check(nomp_update(a, s, e, sizeof(TEST_TYPE), NOMP_FROM));

#if defined(TEST_TOL)
  for (unsigned i = 0; i < s; i++)
    nomp_test_assert(fabs(a[i]) < TEST_TOL);
  for (unsigned i = s; i < e; i++)
    nomp_test_assert(fabs(a[i] - i) < TEST_TOL);
  for (unsigned i = e; i < TEST_MAX_SIZE; i++)
    nomp_test_assert(fabs(a[i]) < TEST_TOL);
#else
  for (unsigned i = 0; i < s; i++)
    nomp_test_assert(a[i] == 0);
  for (unsigned i = s; i < e; i++)
    nomp_test_assert(a[i] == (TEST_TYPE)i);
  for (unsigned i = e; i < TEST_MAX_SIZE; i++)
    nomp_test_assert(a[i] == 0);
#endif

  nomp_test_check(nomp_update(a, lb, ub, sizeof(TEST_TYPE), NOMP_FREE));

  return 0;
}
#undef in_range_d2h

#define in_range_h2d TOKEN_PASTE(nomp_api_050_in_range_h2d, TEST_SUFFIX)
static int in_range_h2d(unsigned lb, unsigned ub, unsigned s, unsigned e) {
  nomp_test_assert(ub - lb <= TEST_MAX_SIZE);

  TEST_TYPE a[TEST_MAX_SIZE] = {0};
  nomp_test_check(nomp_update(a, lb, ub, sizeof(TEST_TYPE), NOMP_TO));

  for (unsigned i = lb; i < ub; i++)
    a[i] = i;
  nomp_test_check(nomp_update(a, lb, s, sizeof(TEST_TYPE), NOMP_TO));
  nomp_test_check(nomp_update(a, e, ub, sizeof(TEST_TYPE), NOMP_TO));

  for (unsigned i = lb; i < ub; i++)
    a[i] = 0;
  nomp_test_check(nomp_update(a, lb, ub, sizeof(TEST_TYPE), NOMP_FROM));

#if defined(TEST_TOL)
  for (unsigned i = lb; i < s; i++)
    nomp_test_assert(fabs(a[i] - i) < TEST_TOL);
  for (unsigned i = s; i < e; i++)
    nomp_test_assert(fabs(a[i]) < TEST_TOL);
  for (unsigned i = e; i < ub; i++)
    nomp_test_assert(fabs(a[i] - i) < TEST_TOL);
#else
  for (unsigned i = lb; i < s; i++)
    nomp_test_assert(a[i] == (TEST_TYPE)i);
  for (unsigned i = s; i < e; i++)
    nomp_test_assert(a[i] == 0);
  for (unsigned i = e; i < ub; i++)
    nomp_test_assert(a[i] == (TEST_TYPE)i);
#endif

  nomp_test_check(nomp_update(a, lb, ub, sizeof(TEST_TYPE), NOMP_FREE));

  return 0;
}
#undef in_range_h2d

#define dynamic_data_type                                                      \
  TOKEN_PASTE(nomp_api_050_dynamic_data_type, TEST_SUFFIX)
static int dynamic_data_type(unsigned n) {
  nomp_test_assert(n <= TEST_MAX_SIZE);

  char a[TEST_MAX_SIZE];
  nomp_test_check(nomp_update(a, 0, n, sizeof(char), NOMP_ALLOC));

  TEST_TYPE     *b        = (TEST_TYPE *)a;
  const unsigned new_size = n / sizeof(TEST_TYPE);
  for (unsigned i = 0; i < new_size; i++)
    b[i] = i;
  nomp_update(b, 0, new_size, sizeof(TEST_TYPE), NOMP_TO);

  for (unsigned i = 0; i < new_size; i++)
    b[i] = 0;
  nomp_update(b, 0, new_size, sizeof(TEST_TYPE), NOMP_FROM);

#if defined(TEST_TOL)
  for (unsigned i = 0; i < new_size; i++)
    nomp_test_assert(fabs(b[i] - i) < TEST_TOL);
#else
  for (unsigned i = 0; i < new_size; i++)
    nomp_test_assert(b[i] == (TEST_TYPE)i);
#endif

  nomp_test_check(nomp_update(a, 0, n, sizeof(char), NOMP_FREE));

  return 0;
}
#undef dynamic_data_type
