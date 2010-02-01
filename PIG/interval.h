#ifndef INTERVAL_H
#define INTERVAL_H

#include "tuple.h"
#include <iostream>

struct Interval {
  Tuple start;
  Tuple end;

  void clear() {
    memset(this, 0, sizeof(Interval));
  };
};

inline void assign_eq(Interval& dst, Interval const& src) 
{
  assign_eq(dst.start, src.start);
  assign_eq(dst.end, src.end);
};


inline void write_tuple(std::ostream& out, Tuple const& t)
{
  out << t[0] << "," << t[1] << "," << t[2] << "," << t[3];
};

inline void write_interval(std::ostream& out, Interval const& i)
{
  out << "[";
  write_tuple(out, i.start);
  out << " - ";
  write_tuple(out, i.end);
  out << "]";
};

#endif
