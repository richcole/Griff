#include "sarl_metrics.h"
#include "sarl_point_set.h"
#include "sarl_diagram.h"
#include "sarl_lattice.h"
#include "sarl_bit_set.h"
#include "sarl_int_array.h"

#include <set>
#include <ext/hash_map>
#include <ext/hash_set>
#include <math.h>

/* Metrics:
 *   more positive number means better
 *   more negative number means worse
 */

using namespace __gnu_cxx;

namespace __gnu_cxx {
  template<> struct hash<std::string>  {
    size_t operator()( const std::string& x ) const
    {
      return hash< const char* >()( x.c_str() );
    }
  };
  template<class T> struct hash<T *>  {
    size_t operator()( T* x ) const
    {
      return (size_t)x;
    }
  };
  template<> struct hash<double>  {
    size_t operator()(double x) const
    {
      if ( sizeof(size_t) == sizeof(double) ) {
        return *(size_t *)&x;
      }
      else if ( sizeof(size_t)*2 == sizeof(double) ) {
        size_t *p = (size_t *)&x;
        return p[0] ^ p[1];
      }
    }
  };
}

sarl_signed_int num_edge_crossings_metric(sarl_diagram_t *diagram)
{
  sarl_signed_int   result = 0;
  sarl_unsigned_int x, y, a, b;
  sarl_bit_set_t *lower_covers_x, *lower_covers_a;
  sarl_lattice_t *lattice = diagram->lattice;
  sarl_point_t *px, *py, *pa, *pb;

  SARL_POWER_MAP_FOR_EACH(x, lower_covers_x, lattice->lower_covers);
    SARL_POWER_MAP_FOR_EACH(a, lower_covers_a, lattice->lower_covers);
    if ( a > x ) {
      SARL_BIT_SET_FOR_EACH(y, lower_covers_x);
        SARL_BIT_SET_FOR_EACH(b, lower_covers_a);
        if ( x != a && x != b && y != a && y != b ) {
          px = sarl_diagram_get_concept_pos(diagram, x);
          py = sarl_diagram_get_concept_pos(diagram, y);
          pa = sarl_diagram_get_concept_pos(diagram, a);
          pb = sarl_diagram_get_concept_pos(diagram, b);
          if ( sarl_point_crossing(px, py, pa, pb) ) {
            result += 1;
          }
        } 
        SARL_BIT_SET_END;
      SARL_BIT_SET_END;
    }
    SARL_POWER_MAP_END;
  SARL_POWER_MAP_END;

  return -result;  /* smaller value is better */
};

sarl_unsigned_int debug_count_for_num_sym_children_nz_metric = 0;

sarl_signed_int num_sym_children_nz_metric(sarl_diagram_t *diagram)
{
  ++debug_count_for_num_sym_children_nz_metric;

  sarl_unsigned_int result = 0;
  sarl_point_set_t *ps = sarl_point_set_default_new();

  sarl_bit_set_t* lower_covers;
  sarl_unsigned_int c, lc;
  sarl_point_t p, *pc;

  SARL_POWER_MAP_FOR_EACH(c, lower_covers, diagram->lattice->lower_covers);
    sarl_point_set_clear(ps);
    pc = sarl_diagram_get_concept_pos(diagram, c);

    SARL_BIT_SET_FOR_EACH(lc, lower_covers);
      sarl_point_assign_eq(&p, sarl_diagram_get_concept_pos(diagram, lc));
      p.x -= pc->x; p.y -= pc->y;
      if ( p.x != 0 ) sarl_point_set_insert(ps, &p);
    SARL_BIT_SET_END;

    SARL_BIT_SET_FOR_EACH(lc, lower_covers);
      sarl_point_assign_eq(&p, sarl_diagram_get_concept_pos(diagram, lc));
      p.x -= pc->x; p.y -= pc->y;
      p.x = -p.x;
      if ( p.x != 0 && sarl_point_set_is_member(ps, &p) ) 
        result += 1;
    SARL_BIT_SET_END;
  SARL_POWER_MAP_END;

  return result;
};

sarl_signed_int num_sym_children_metric(sarl_diagram_t *diagram)
{
  sarl_unsigned_int result = 0;
  sarl_point_set_t *ps = sarl_point_set_default_new();

  sarl_bit_set_t* lower_covers;
  sarl_unsigned_int c, lc;
  sarl_point_t p, *pc;

  SARL_POWER_MAP_FOR_EACH(c, lower_covers, diagram->lattice->lower_covers);
    sarl_point_set_clear(ps);
    pc = sarl_diagram_get_concept_pos(diagram, c);

    SARL_BIT_SET_FOR_EACH(lc, lower_covers);
      sarl_point_assign_eq(&p, sarl_diagram_get_concept_pos(diagram, lc));
      p.x -= pc->x; p.y -= pc->y;
    SARL_BIT_SET_END;

    SARL_BIT_SET_FOR_EACH(lc, lower_covers);
      sarl_point_assign_eq(&p, sarl_diagram_get_concept_pos(diagram, lc));
      p.x -= pc->x; p.y -= pc->y;
      p.x = -p.x;
      if ( sarl_point_set_is_member(ps, &p) ) 
        result += 1;
    SARL_BIT_SET_END;
  SARL_POWER_MAP_END;

  return result;
};

sarl_signed_int num_reflections_metric(sarl_diagram_t *diagram)
{
  sarl_lattice_t *lattice = diagram->lattice;
  sarl_point_set_t *points = sarl_point_set_default_new();

  std::set<sarl_signed_int>  planes;
  hash_map<sarl_signed_int, sarl_unsigned_int> plane_count;
  hash_map<sarl_signed_int, sarl_unsigned_int>::iterator it;

  sarl_unsigned_int c;
  sarl_point_t *p;
  sarl_signed_int y, ax, bx, plane;
  sarl_int_set_t *xs;

  double result = 0;
  
  // collect points and planes
  SARL_LATTICE_FOR_EACH(c, lattice);
    p = sarl_diagram_get_concept_pos(diagram, c);
    sarl_point_set_insert(points, p);
    planes.insert(p->x);
  SARL_LATTICE_END;

  // collect the symmetry planes
  SARL_POINT_SET_EACH_LEVEL(y, xs, points);
    SARL_INT_SET_FOR_EACH(ax, xs);
      SARL_INT_SET_FOR_EACH(bx, xs);
        if ( bx > ax && (((ax + bx) % 2) == 0) ) {
          plane = (ax + bx) / 2;
          
          if ( planes.find(plane) != planes.end() ) {
            it = plane_count.find(plane);
            if ( it != plane_count.end() ) 
              ++(it->second);
            else
              plane_count[plane] = 1;
          }
        }
      SARL_INT_SET_END;
    SARL_INT_SET_END;
  SARL_POINT_SET_LEVEL_END;

  // boost each plane by the number of points on it
  SARL_LATTICE_FOR_EACH(c, lattice);
    p = sarl_diagram_get_concept_pos(diagram, c);
    it = plane_count.find(p->x);
    if (it != plane_count.end()) ++(it->second);
  SARL_LATTICE_END;

  for(it=plane_count.begin(); it!=plane_count.end(); ++it) {
    result += log((double)it->second);
  }

  return (int)(result * 10);
};

sarl_signed_int num_vectors_metric(sarl_diagram_t *diagram)
{
  sarl_point_set_t *points = sarl_point_set_default_new();
  sarl_point_t d;

  sarl_point_t *pa, *pb;
  
  SARL_DIAGRAM_COVERING_EACH(pa, pb, diagram);
    d.x = pa->x - pb->x;
    d.y = pa->y - pb->y;
    sarl_point_set_insert(points, &d);
  SARL_DIAGRAM_COVERING_END;

  return -(sarl_signed_int)sarl_point_set_count(points);
};

sarl_signed_int num_unique_gradients_metric(sarl_diagram_t *diagram)
{
  hash_set<double> grad_set;
  
  sarl_point_t d, *pa, *pb;

  SARL_DIAGRAM_COVERING_EACH(pa, pb, diagram);
    d.x = pa->x - pb->x;
    d.y = pa->y - pb->y;
    grad_set.insert(d.y / (double)d.x);
  SARL_DIAGRAM_COVERING_END;

  return -(sarl_signed_int)grad_set.size();
};

sarl_signed_int num_unique_gradients_abs_metric(sarl_diagram_t *diagram)
{
  hash_set<double> grad_set;
  
  sarl_point_t d, *pa, *pb;
  SARL_DIAGRAM_COVERING_EACH(pa, pb, diagram);
    d.x = pa->x - pb->x;
    d.y = pa->y - pb->y;
    grad_set.insert(fabs(d.y / (double)d.x));
  SARL_DIAGRAM_COVERING_END;

  return -(sarl_signed_int)grad_set.size();
};

sarl_signed_int num_meet_vectors_metric(sarl_diagram_t *diagram)
{
  sarl_power_map_t *upper_covers = diagram->lattice->upper_covers;
  sarl_bit_set_t *uc;
  sarl_unsigned_int c;
  sarl_point_t d;
  sarl_point_t *pa, *pb;

  sarl_point_set_t *points = sarl_point_set_default_new();

  SARL_POWER_MAP_FOR_EACH(c, uc, upper_covers);
  if ( sarl_bit_set_count(uc) == 1 ) {
    pa = sarl_diagram_get_concept_pos(diagram, c);
    pb = sarl_diagram_get_concept_pos(diagram, sarl_bit_set_first(uc));
    d.x = pa->x - pb->x;
    d.y = pa->y - pb->y;
    sarl_point_set_insert(points, &d);
  }
  SARL_POWER_MAP_END;

  return -(sarl_signed_int)sarl_point_set_count(points);
};

sarl_signed_int num_chains_metric(sarl_diagram_t *diagram)
{
  sarl_power_map_t *upper_covers = diagram->lattice->upper_covers;
  sarl_power_map_t *lower_covers = diagram->lattice->lower_covers;

  sarl_bit_set_t *uc;
  sarl_unsigned_int x, child_x;
  sarl_point_t du, dv;
  
  sarl_signed_int num_chains = 0;

  SARL_POWER_MAP_FOR_EACH(x, uc, upper_covers);
  if ( sarl_bit_set_count(uc) == 1 ) {
    sarl_unsigned_int parent_x = sarl_bit_set_first(uc);
    sarl_bit_set_t *lc_x = sarl_power_map_get(lower_covers, x);
    SARL_BIT_SET_FOR_EACH(child_x, lc_x);
      sarl_bit_set_t *child_x_uc = sarl_power_map_get(upper_covers, child_x);
      if ( sarl_bit_set_count(child_x_uc) == 1 ) {
        sarl_point_t *parent_pt = sarl_diagram_get_concept_pos(diagram, parent_x);
        sarl_point_t *x_pt      = sarl_diagram_get_concept_pos(diagram, x);
        sarl_point_t *child_pt  = sarl_diagram_get_concept_pos(diagram, child_x);
        dv.x = x_pt->x - parent_pt->x;
        dv.y = x_pt->y - parent_pt->y;
        du.x = child_pt->x - x_pt->x;
        du.y = child_pt->y - x_pt->y;
        if (dv.x == du.x && dv.y == du.y) {
          num_chains += 1;
        }
      }
    SARL_BIT_SET_END;
  }
  SARL_POWER_MAP_END;

  return num_chains;
};

sarl_signed_int num_three_chains_metric(sarl_diagram_t *diagram)
{
  sarl_power_map_t *upper_covers = diagram->lattice->upper_covers;

  sarl_bit_set_t *uc;
  sarl_unsigned_int x;
  sarl_point_t dxp, dpg, dgg;
  
  sarl_signed_int num_chains = 0;

  SARL_POWER_MAP_FOR_EACH(x, uc, upper_covers);
  if ( sarl_bit_set_count(uc) == 1 ) {
    sarl_unsigned_int parent = sarl_bit_set_first(uc);
    sarl_bit_set_t *parent_uc = sarl_power_map_get(upper_covers, parent);
    if ( sarl_bit_set_count(parent_uc) == 1 ) {
      sarl_unsigned_int granny = sarl_bit_set_first(parent_uc);
      sarl_bit_set_t *granny_uc = sarl_power_map_get(upper_covers, granny);
      if ( sarl_bit_set_count(granny_uc) == 1 ) {
        sarl_unsigned_int great_granny = sarl_bit_set_first(granny_uc);
        sarl_point_t* pt_x = sarl_diagram_get_concept_pos(diagram, x);
        sarl_point_t* pt_parent = sarl_diagram_get_concept_pos(diagram, parent);
        sarl_point_t* pt_granny = sarl_diagram_get_concept_pos(diagram, granny);
        sarl_point_t* pt_great_granny = sarl_diagram_get_concept_pos(diagram, great_granny);
        dxp.x = pt_parent->x - pt_x->x;
        dxp.y = pt_parent->y - pt_x->y;
        dpg.x = pt_granny->x - pt_parent->x;
        dpg.y = pt_granny->y - pt_parent->y;
        dgg.x = pt_great_granny->x - pt_granny->x;
        dgg.y = pt_great_granny->y - pt_granny->y;
        if ( 
          dxp.x == dpg.x && dxp.x == dgg.x && 
          dxp.y == dpg.y && dxp.y == dgg.y 
        ) {
          num_chains += 1;
        }
      }
    }
  }
  SARL_POWER_MAP_END;

  return num_chains;
};

sarl_signed_int num_well_placed_children_metric(sarl_diagram_t *diagram)
{
  sarl_signed_int count = 0;

  sarl_unsigned_int a;
  sarl_lattice_t *lattice = diagram->lattice;

  SARL_LATTICE_FOR_EACH(a, lattice);
    sarl_bit_set_t *lc = sarl_power_map_get(lattice->lower_covers, a);
    sarl_unsigned_int mini_count, b;
    switch ( sarl_bit_set_count(lc) ) {
    case 1:
      mini_count = 0;
      b = sarl_bit_set_first(lc);
      sarl_point_t dx; sarl_point_minus(&dx,
        sarl_diagram_get_concept_pos(diagram, b), 
        sarl_diagram_get_concept_pos(diagram, a)
      );
      if ( dx.y == 1 && dx.x == 0 ) {
        mini_count += 1;
      }
      if ( mini_count == 1 ) {
        count += 1;
      }
      break;
    case 2:
      mini_count = 0;
      SARL_BIT_SET_FOR_EACH(b, lc);
      sarl_point_t dx; sarl_point_minus(&dx,
        sarl_diagram_get_concept_pos(diagram, b), 
        sarl_diagram_get_concept_pos(diagram, a)
      );
      if (dx.y == 1 && (dx.x == -1 || dx.x == 1)) {
        mini_count += 1;
      };
      SARL_BIT_SET_END;
      if (mini_count == 2) {
        count += 1;
      }
      break;
    case 3:
      mini_count = 0;
      SARL_BIT_SET_FOR_EACH(b, lc);
      sarl_point_t dx; sarl_point_minus(&dx,
        sarl_diagram_get_concept_pos(diagram, b), 
        sarl_diagram_get_concept_pos(diagram, a)
      );
      if (dx.y == 1 && (dx.x == -1 || dx.x == 1 || dx.x == 0)) {
        mini_count += 1;
      };
      SARL_BIT_SET_END;
      if (mini_count == 3) {
        count += 1;
      }
      break;
    };
  SARL_LATTICE_END;

  return count;
};

sarl_signed_int num_ok_placed_children_metric(sarl_diagram_t *diagram)
{
  sarl_signed_int count = 0;

  sarl_unsigned_int a;
  sarl_lattice_t *lattice = diagram->lattice;

  SARL_LATTICE_FOR_EACH(a, lattice);
  sarl_bit_set_t *lc = sarl_power_map_get(lattice->lower_covers, a);
  sarl_signed_int mini_count = 0;
  sarl_unsigned_int b;

  if ( sarl_bit_set_count(lc) >= 2 ) {
    mini_count = 0;
    SARL_BIT_SET_FOR_EACH(b, lc);
      sarl_point_t dx; sarl_point_minus(&dx,
        sarl_diagram_get_concept_pos(diagram, b), 
        sarl_diagram_get_concept_pos(diagram, a)
      );
      if (dx.y == 1 && (dx.x == -1 || dx.x == 1 )) {
        mini_count += 1;
      };
    SARL_BIT_SET_END;
    if (mini_count == 2) {
      count += 1;
    }
  };
  SARL_LATTICE_END;

  return count;
};

sarl_signed_int total_edge_length_metric(sarl_diagram_t *diagram)
{
  double total = 0.0;
  
  sarl_point_t *pa, *pb;

  SARL_DIAGRAM_COVERING_EACH(pa, pb, diagram);
    double dx = pa->x - pb->x;
    double dy = pa->y - pb->y;
    total += sqrt((dx * dx) + (dy * dy));
  SARL_DIAGRAM_COVERING_END;

  return -(sarl_signed_int)total;
};

void each_path(sarl_diagram_t *diagram, sarl_unsigned_int x, sarl_int_array_t *xs,
  void *arg, void (*block)(void *arg, sarl_int_array_t *path))
{
  sarl_bit_set_t *lc_x = sarl_power_map_get(diagram->lattice->lower_covers, x);
  sarl_unsigned_int y;

  sarl_int_array_push(xs, x);
  if ( sarl_bit_set_is_empty(lc_x) ) {
    (*block)(arg, xs);
  }
  else {
    SARL_BIT_SET_FOR_EACH(y, lc_x);
      each_path(diagram, y, xs, arg, block);
    SARL_BIT_SET_END;
  }
  sarl_int_array_pop(xs);
};

struct _measure_path_width_arg_t {
  sarl_unsigned_int num_paths;
  sarl_unsigned_int total_path_width;
  sarl_diagram_t *diagram;
};
typedef struct _measure_path_width_arg_t measure_path_width_arg_t;

void measure_path_width(void *arg, sarl_int_array_t *path)
{
  measure_path_width_arg_t *pw_arg = (measure_path_width_arg_t *)arg;
  sarl_signed_int min_x = 0, max_x = 0;
  bool is_first = true;
  sarl_unsigned_int x;
  SARL_INT_ARRAY_FOR_EACH(x, path);
    sarl_point_t *pt_x = sarl_diagram_get_concept_pos(pw_arg->diagram, x);
    if ( is_first ) {
      is_first = false;
      max_x = min_x = pt_x->x;
    }
    else {
      min_x = sarl_min_of(min_x, pt_x->x);
      max_x = sarl_max_of(max_x, pt_x->x);
    }
  SARL_INT_ARRAY_END;
  pw_arg->num_paths += 1;
  pw_arg->total_path_width += max_x - min_x;
};

sarl_signed_int ave_path_width_metric(sarl_diagram_t *diagram)
{
  measure_path_width_arg_t pw_arg = {
    0, 0, diagram
  };
  sarl_unsigned_int x = 1;
  sarl_int_array_t *xs = sarl_int_array_default_new();
  each_path(diagram, x, xs, &pw_arg, &measure_path_width);
  return -(sarl_signed_int)(pw_arg.total_path_width / pw_arg.num_paths);
};

sarl_signed_int vector_size_metric(sarl_int_array_t *a)
{
  sarl_unsigned_int v;
  sarl_signed_int result = 0;
  SARL_INT_ARRAY_FOR_EACH(v, a);
    result += v;
  SARL_INT_ARRAY_END;
  return -result;
};

sarl_signed_int children_ballance_metric(sarl_diagram_t *diagram)
{
  sarl_lattice_t *lattice = diagram->lattice;
  sarl_signed_int result = 0, ballance = 0;
  sarl_unsigned_int a, b;

  SARL_LATTICE_FOR_EACH(a, lattice);
    ballance = 0;
    sarl_bit_set_t *lc = sarl_power_map_get(lattice->lower_covers, a);
    SARL_BIT_SET_FOR_EACH(b, lc);
      sarl_point_t* pa = sarl_diagram_get_concept_pos(diagram, a);
      sarl_point_t* pb = sarl_diagram_get_concept_pos(diagram, b);
      if ( pb->x > pa->x ) {
        ballance += 1;
      }
      else if (pb->x < pa->x) {
        ballance -= 1;
      }
    SARL_BIT_SET_END;
    result += abs(ballance);
  SARL_LATTICE_END;

  return -result;
};
