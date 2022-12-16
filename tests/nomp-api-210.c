#define TEST_IMPL_H "nomp-api-210-impl.h"
#include "nomp-generate-tests.h"
#undef TEST_IMPL_H

static int test_vector_addition() {
  TEST_BUILTIN_TYPES(210_add, 10)
  TEST_BUILTIN_TYPES(210_add, 20)
}

static int test_vector_subtraction() {
  TEST_BUILTIN_TYPES(210_sub, 10)
  TEST_BUILTIN_TYPES(210_sub, 20)
}

static int test_vector_multiplication_summation() {
  TEST_BUILTIN_TYPES(210_mul_sum, 10)
  TEST_BUILTIN_TYPES(210_mul_sum, 20)
}

static int test_vector_multiplication() {
  TEST_BUILTIN_TYPES(210_mul, 10)
  TEST_BUILTIN_TYPES(210_mul, 20)
}

static int test_vector_square_sum() {
  TEST_BUILTIN_TYPES(210_square, 10)
  TEST_BUILTIN_TYPES(210_square, 20)
}

static int test_vector_linear() {
  TEST_BUILTIN_TYPES(210_linear, 10)
  TEST_BUILTIN_TYPES(210_linear, 20)
}

int main(int argc, const char *argv[]) {
  int err = nomp_init(argc, argv);
  nomp_test_chk(err);

  err |= SUBTEST(test_vector_addition);
  err |= SUBTEST(test_vector_subtraction);
  err |= SUBTEST(test_vector_multiplication_summation);
  err |= SUBTEST(test_vector_multiplication);
  err |= SUBTEST(test_vector_square_sum);
  err |= SUBTEST(test_vector_linear);

  err = nomp_finalize();
  nomp_test_chk(err);

  return err;
}
