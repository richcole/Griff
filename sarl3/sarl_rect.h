#ifndef SARL_RECT_H
#define SARL_RECT_H

#include "sarl_types.h"
#include "sarl_exceptions.h"
#include "sarl_point.h"

#include <gc/gc.h>
#include <string.h>

struct _sarl_rect_t {
  sarl_signed_int left;
  sarl_signed_int top;
  sarl_signed_int width;
  sarl_signed_int height;
};

inline sarl_rect_t* sarl_rect_new() {
  sarl_rect_t *rect = (sarl_rect_t *)GC_MALLOC_ATOMIC(sizeof(sarl_rect_t));
  if (rect) {
    memset(rect, 0, sizeof(sarl_rect_t));
  }
  else {
    sarl_out_of_memory_error;
  }
  return rect;
}

inline void sarl_rect_clear(sarl_rect_t *rect) {
  if (rect) {
    memset(rect, 0, sizeof(sarl_rect_t));
  }
  else {
    sarl_null_pointer_error;
  }
}

inline int sarl_rect_includes(sarl_rect_t *r, sarl_point_t *p)
{
  return (
    p->x >= r->left && p->y >= r->top && 
    p->x <= (r->left + r->width) && p->y <= (r->top + r->height)
  );
};

inline void sarl_rect_insert_point(sarl_rect_t *r, sarl_point_t *p)
{
  sarl_signed_int right = sarl_max_of(r->left + r->width, p->x);
  sarl_signed_int bottom = sarl_max_of(r->top + r->height, p->y);

  r->left = sarl_min_of(r->left, p->x);
  r->top  = sarl_min_of(r->top, p->y);

  r->width = right - r->left;
  r->height = bottom - r->top;
};

inline void sarl_rect_center_top_on(
  sarl_rect_t* rect, sarl_signed_int x, sarl_signed_int y)
{
  rect->left = x - (rect->width/2);
  rect->top  = y;
};
    
inline void sarl_rect_center_bottom_on(
  sarl_rect_t* rect, sarl_signed_int x, sarl_signed_int y)
{
  rect->left = x - (rect->width/2);
  rect->top  = y - rect->height;
};
    
inline void sarl_rect_center_top_left_on(
  sarl_rect_t* rect, sarl_signed_int x, sarl_signed_int y)
{
  rect->left = x;
  rect->top  = y;
};
    
#endif
