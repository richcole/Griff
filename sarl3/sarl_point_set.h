#ifndef SARL_POINT_SET_H
#define SARL_POINT_SET_H

#include "sarl_types.h"
#include "sarl_hash_map.h"
#include "sarl_point.h"
#include "sarl_int_set.h"

#define SARL_SIGN_TO_UNSIGN(x) ((sarl_unsigned_int)(x))
#define SARL_UNSIGN_TO_SIGN(x) ((sarl_signed_int)(x))

inline sarl_point_set_t* sarl_point_set_default_new() {
  return sarl_hash_map_default_new();
};

inline sarl_unsigned_int sarl_point_set_is_member(
  sarl_point_set_t *ps, sarl_point_t* p) 
{
  if ( ps == 0 ) {
    sarl_null_pointer_error;
    return 0;
  }

  sarl_int_set_t* xs = 
    (sarl_int_set_t *)sarl_hash_map_image(ps, SARL_SIGN_TO_UNSIGN(p->y));
  if ( xs != 0 ) {
    return sarl_int_set_is_member(xs, SARL_SIGN_TO_UNSIGN(p->x));
  }

  return 0;
}

inline sarl_unsigned_int sarl_point_set_insert(
  sarl_point_set_t *ps, sarl_point_t* p) 
{
  if ( ps == 0 ) {
    sarl_null_pointer_error;
    return 0;
  }

  sarl_int_set_t* xs = 
    (sarl_int_set_t *)sarl_hash_map_image(ps, SARL_SIGN_TO_UNSIGN(p->y));
  if ( xs == 0 ) {
    xs = sarl_int_set_default_new();
    sarl_hash_map_insert(ps, p->y, xs);
  }
  sarl_int_set_insert(xs, SARL_SIGN_TO_UNSIGN(p->x));

  return 0;
}

inline void sarl_point_set_clear(sarl_point_set_t *ps)
{
  sarl_unsigned_int x;
  sarl_int_set_t* is;

  SARL_HASH_MAP_FOR_EACH(sarl_int_set_t, x, is, ps);
    sarl_int_set_clear(is);
  SARL_HASH_MAP_END;
};

sarl_unsigned_int sarl_point_set_count(sarl_point_set_t *ps);

#define SARL_POINT_SET_FOR_EACH(p, ps) \
  { sarl_unsigned_int __ps_x, __ps_y; sarl_int_set_t *__ps_xs; \
  SARL_HASH_MAP_FOR_EACH(sarl_int_set_t, __ps_y, __ps_xs, ps); \
    (p)->y = SARL_UNSIGN_TO_SIGN(__ps_y); \
    SARL_INT_SET_FOR_EACH(__ps_x, __ps_xs); \
    (p)->y = SARL_UNSIGN_TO_SIGN(__ps_x); 

#define SARL_POINT_SET_END \
  SARL_INT_SET_END; \
  SARL_HASH_MAP_END; }

#define SARL_POINT_SET_EACH_LEVEL(y, xs, ps) \
  { sarl_unsigned_int __ps_y; \
  SARL_HASH_MAP_FOR_EACH(sarl_int_set_t, __ps_y, xs, ps); \
    (y) = SARL_UNSIGN_TO_SIGN(__ps_y); \

#define SARL_POINT_SET_LEVEL_END \
  SARL_HASH_MAP_END; }


#endif
