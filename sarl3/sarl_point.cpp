#include "sarl_point.h"

sarl_unsigned_int sarl_point_crossing(
  sarl_point_t *p1, sarl_point_t *p2, sarl_point_t *a1, sarl_point_t *a2)
{
  double m1, m2, t, s, intercept;

  double p1x = p1->x;
  double p1y = p1->y;
  double p2x = p2->x;
  double p2y = p2->y;
  double a1x = a1->x;
  double a1y = a1->y;
  double a2x = a2->x;
  double a2y = a2->y;

  if (p1x > p2x) {
    t = p1x;
    p1x = p2x;
    p2x = t;
    t = p1y;
    p1y = p2y;
    p2y = t;
  };

  if (a1x > a2x) {
    t = a1x;
    a1x = a2x;
    a2x = t;
    t = a1y;
    a1y = a2y;
    a2y = t;
  };

  if (p2x == p1x) {
    if (p1x < a1x || p1x > a2x) return 0;
    if (a2x == a1x) return 0;
    t = p1x - a1x;
    m2 = (a2y - a1y) / (a2x - a1x);
    intercept = a1y + (m2 * t);
    if ( p2y > p1y && (intercept > p2y || intercept < p1y)) return 0;
    if ( p2y < p1y && (intercept < p2y || intercept > p1y)) return 0;
    return 1; // [p1x, intercept];
  };
    
  if (a1x == a2x) {
    if ( a1x < p1x || a1x > p2x) return 0;
    if ( p2x == p1x) return 0;
    t = a1x - p1x;
    m1 = (p2y - p1y) / (p2x - p1x);
    intercept = p1y + (m1 * t);
    if ( a2y > a1y && (intercept > a2y || intercept < a1y)) return 0;
    if ( a2y < a1y && (intercept < a2y || intercept > a1y)) return 0;
    return 1; // [a1x, intercept]
  };
    
  m1 = (p2y - p1y) / (p2x - p1x);
  m2 = (a2y - a1y) / (a2x - a1x);
  if (m1 == m2) return 0;
  t = ((p1y - a1y) - m2*(p1x - a1x)) / (m2 - m1);
  s = (p1x - a1x) + t;

  if ( t < 0 || s < 0 || p1x + t > p2x || a1x + s > a2x ) {
    return 0;
  } else {
    return 1; // [p1x + t, p1y + (m1 * t)]
  };
};
