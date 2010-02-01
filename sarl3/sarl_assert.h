#ifndef SARL_ASSERT_H
#define SARL_ASSERT_H

#include <stdio.h>
#include "sarl_exceptions.h"

inline void sarl_test_assert_func(
  int expr, char *expr_string, char *filename, int line_num
) 
{
  if (! expr) {
    fprintf(stderr, "%s:%d: error: Assertion Failed: '%s'\n",
      filename, line_num, expr_string);
  }
}

#define sarl_test_assert(expr) \
  sarl_test_assert_func(sarl_errno == SARL_ERROR_NO_ERROR, "sarl_errno == SARL_ERROR_NO_ERROR", __FILE__, __LINE__); \
  sarl_test_assert_func(expr, #expr, __FILE__, __LINE__)

#define sarl_test_end \
  sarl_test_assert_func(sarl_errno == SARL_ERROR_NO_ERROR, "sarl_errno == SARL_ERROR_NO_ERROR", __FILE__, __LINE__); \
  return (int)sarl_errno;

#define sarl_test_begin(test_name) \
    fprintf(stderr, "Starting test '%s' ...\n", test_name);

#endif
