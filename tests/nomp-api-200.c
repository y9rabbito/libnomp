#define TEST_MAX_SIZE 100
#define TEST_IMPL_H   "nomp-api-200-impl.h"
#include "nomp-generate-tests.h"
#undef TEST_IMPL_H
#undef TEST_MAX_SIZE

static int test_vector_addition(void) {
  int err = 0;
  TEST_BUILTIN_TYPES(200_add, 10)
  TEST_BUILTIN_TYPES(200_add, 50)
  return err;
}

static int test_vector_subtraction(void) {
  int err = 0;
  TEST_BUILTIN_TYPES(200_sub, 10)
  TEST_BUILTIN_TYPES(200_sub, 50)
  return err;
}

static int test_vector_multiplication_sum(void) {
  int err = 0;
  TEST_BUILTIN_TYPES(200_mul_sum, 10)
  TEST_BUILTIN_TYPES(200_mul_sum, 50)
  return err;
}

static int test_vector_multiplication(void) {
  int err = 0;
  TEST_BUILTIN_TYPES(200_mul, 10)
  TEST_BUILTIN_TYPES(200_mul, 50)
  return err;
}

static int test_vector_square_sum(void) {
  int err = 0;
  TEST_BUILTIN_TYPES(200_square, 10)
  TEST_BUILTIN_TYPES(200_square, 50)
  return err;
}

static int test_vector_linear(void) {
  int err = 0;
  TEST_BUILTIN_TYPES(200_linear, 10)
  TEST_BUILTIN_TYPES(200_linear, 50)
  return err;
}

int main(int argc, const char *argv[]) {
  nomp_test_check(nomp_init(argc, argv));

  int err = 0;
  err |= SUBTEST(test_vector_addition);
  err |= SUBTEST(test_vector_subtraction);
  err |= SUBTEST(test_vector_multiplication_sum);
  err |= SUBTEST(test_vector_multiplication);
  err |= SUBTEST(test_vector_square_sum);
  err |= SUBTEST(test_vector_linear);

  nomp_test_check(nomp_finalize());

  return err;
}
