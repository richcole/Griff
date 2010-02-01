#ifndef TUPLE_H
#define TUPLE_H

#include <stdint.h>
#include <string.h>
#include <iostream>


typedef uint8_t   Byte;
typedef uint32_t  Index;
typedef uint64_t  LongIndex;

typedef Byte   *Payload;
typedef Index   Tuple[4];

inline void clear(Tuple& t)
{
  memset(t, 0, sizeof(t));
};

inline int cmp(Index const* x, Index const* y)
{

  int gt = 
    ((x[0] > y[0]) << 6) | ((x[1] > y[1]) << 4) | 
    ((x[2] > y[2]) << 2) | (x[3] > y[3]);
  int lt = 
    ((x[0] < y[0]) << 6) | ((x[1] < y[1]) << 4) |
    ((x[2] < y[2]) << 2) | (x[3] < y[3]);
  return gt - lt;
};

void plus_equals_one_modulo(Tuple& x);
void minus_equals_one_modulo(Tuple& x);

inline void plus_equals_modulo(Tuple &x, Tuple const& y)
{
  x[0] += y[0];
  x[1] += y[1];
  x[2] += y[2];
  x[3] += y[3];
};

inline void minus_equals_modulo(Tuple& x, Tuple& y)
{ 
  x[0] -= y[0];
  x[1] -= y[1];
  x[2] -= y[2];
  x[3] -= y[3];
};

void plus_equals_one(Tuple& x);
void minus_equals_one(Tuple& x);

inline void plus_equals(Tuple &x, Tuple const& y)
{
  Index s;
  Index c = 0;

  s = x[3] + y[3];
  c = s < x[3];
  x[3] = s;
  
  s = x[2] + y[2] + c;
  c = s < x[2];
  x[2] = s;
  
  s = x[1] + y[1] + c;
  c = s < x[1];
  x[1] = s;
  
  s = x[0] + y[0] + c;
  c = s < x[0];
  x[0] = s;
};

inline void minus_equals(Tuple& x, Tuple& y)
{
  Index s;
  Index c = 0;

  s = x[3] - y[3];
  c = s > x[3];
  x[3] = s;
  
  s = x[2] - y[2] - c;
  c = s > x[2];
  x[2] = s;
  
  s = x[1] - y[1] - c;
  c = s > x[1];
  x[1] = s;
  
  s = x[0] - y[0] - c;
  c = s > x[0];
  x[0] = s;
};

inline void assign_eq(Tuple& t, Tuple const& s)
{
  memcpy(t, s, sizeof(t));
};

inline unsigned int is_null(Tuple& tuple)
{
  unsigned int i;
  for(i=0;i<4;++i) {
    if ( tuple[i] != 0 ) {
      return 0;
    }
  }
  return 1;
};

#endif
  
