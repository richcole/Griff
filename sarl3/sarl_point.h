#ifndef SARL_POINT_H
#define SARL_POINT_H

#include "sarl_types.h"

#include <gc/gc.h>
#include <string.h>

struct _sarl_point_t {
  sarl_signed_int x;
  sarl_signed_int y;
};

inline sarl_point_t* sarl_point_new(sarl_signed_int x, sarl_signed_int y) {
  sarl_point_t* ret = (sarl_point_t *)GC_MALLOC_ATOMIC(sizeof(sarl_point_t));
  ret->x = x;
  ret->y = y;
  return ret;
};

inline void sarl_point_assign_eq(sarl_point_t *dst, sarl_point_t *src)
{
  memcpy(dst, src, sizeof(*src));
}

inline void sarl_point_minus(sarl_point_t *dst, sarl_point_t *a, sarl_point_t *b) {
  dst->x = a->x - b->x;
  dst->y = a->y - b->y;
};

sarl_unsigned_int sarl_point_crossing(
  sarl_point_t *p1, sarl_point_t *p2, sarl_point_t *a1, sarl_point_t *a2);

#endif
