#ifndef SARL_EXCEPTIONS_H
#define SARL_EXCEPTIONS_H

#include <stdio.h>
#include "sarl_types.h"

extern sarl_unsigned_int sarl_errno;

#define SARL_ERROR_NO_ERROR            0
#define SARL_ERROR_OUT_OF_MEMORY       1
#define SARL_ERROR_NULL_POINTER        2
#define SARL_ERROR_INVALID_INDEX       3
#define SARL_ERROR_INTERNAL            4
#define SARL_ERROR_NOT_IMPLEMENTED     5

inline void sarl_clear_error() {
  sarl_errno = SARL_ERROR_NO_ERROR;
};

#define sarl_report_error(err) \
  fprintf(stderr, "%s:%d: error: " err "\n", __FILE__, __LINE__);

#define sarl_out_of_memory_error \
  sarl_report_error("out of memory"); \
  if ( sarl_errno == 0 ) sarl_errno = SARL_ERROR_OUT_OF_MEMORY;

#define sarl_null_pointer_error \
  sarl_report_error("null pointer error"); \
  if ( sarl_errno == 0 ) sarl_errno = SARL_ERROR_NULL_POINTER;

#define sarl_invalid_index_error \
  sarl_report_error("invalid index error"); \
  if ( sarl_errno == 0 ) sarl_errno = SARL_ERROR_INVALID_INDEX;

#define sarl_internal_error \
  sarl_report_error("internal error"); \
  if ( sarl_errno == 0 ) sarl_errno = SARL_ERROR_INTERNAL;

#define sarl_not_implemented_error \
  sarl_report_error("not implemented error"); \
  if ( sarl_errno == 0 ) sarl_errno = SARL_ERROR_NOT_IMPLEMENTED;

#define sarl_return_zero_error_if_zero(p) \
  if (p == 0) { sarl_null_pointer_error; sarl_report_error("  " #p " was null."); return 0; };

#define sarl_return_zero_error_if_zero_2(p, q) \
  sarl_return_zero_error_if_zero(p); \
  sarl_return_zero_error_if_zero(q);

#define sarl_return_zero_error_if_zero_3(p, q, r) \
  sarl_return_zero_error_if_zero(p); \
  sarl_return_zero_error_if_zero(q); \
  sarl_return_zero_error_if_zero(r);


#endif
