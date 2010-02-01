#ifndef SARL_LAYOUT_H
#define SARL_LAYOUT_H

#include "sarl_types.h"
#include "sarl_int_array.h"

#include <gc/gc.h>
#include <string.h>

typedef struct _solution_t solution_t;
typedef struct _search_point_t search_point_t;

struct _sarl_search_point_t { 
  sarl_array_t       *lattices;      /* a lattice for each M_i = {1..i}  */
  sarl_int_array_t   *vector;        /* assignment of M -> vector_number */
  sarl_int_array_t   *attributes;    /* the attributes sorted appropriately */
  sarl_array_t       *solutions;     /* a list of solutions              */
  sarl_array_t       *attr_vectors;  /* array of sarl_point_t            */
  sarl_diagram_t     *diagram;       /* reused during the computation    */
  sarl_point_set_t   *point_set;     /* reused during the computation    */
  sarl_unsigned_int  terminate;      /* whether or not to terminate the search */
  sarl_unsigned_int  base;           /* how many attribute vectors are there */
  sarl_unsigned_int  num_attributes; /* how many attributes there are    */
  sarl_unsigned_int  sol_count;      /* how many solutions considered */
  sarl_unsigned_int  max_sol_count;  /* terminate after how many  */
  sarl_unsigned_int  num_metrics;    /* number of metrics */
};

struct _sarl_solution_t {
  sarl_int_array_t  *metrics;
  sarl_int_array_t  *vectors;
};

sarl_int_array_t* sarl_order_attributes_by_implication(
  sarl_power_map_t  *imp, 
  sarl_bit_set_t    *M /* attributes */
);

sarl_int_array_t* sarl_context_order_attributes_by_extent_count(sarl_context_t *context);

void sarl_layout_next(sarl_search_point_t *sol, sarl_unsigned_int i);

inline sarl_search_point_t* sarl_search_point_new()
{
  sarl_search_point_t* sp = (sarl_search_point_t *)
    GC_MALLOC(sizeof(sarl_search_point_t));
  memset(sp, 0, sizeof(*sp));
  sp->vector = sarl_int_array_default_new();
  return sp;
};

inline sarl_solution_t* sarl_solution_new(sarl_int_array_t *vectors)
{
  sarl_solution_t* s = (sarl_solution_t *)
    GC_MALLOC(sizeof(sarl_solution_t));
  memset(s, 0, sizeof(*s));
  s->metrics = sarl_int_array_default_new();
  s->vectors = sarl_int_array_dup(vectors);
  
  return s;
};

sarl_diagram_t* sarl_diagram_layout_from_context(sarl_context_t* context);
sarl_diagram_t* sarl_diagram_layout_from_lattice(sarl_lattice_t* lattice);

sarl_array_t* sarl_diagram_layout_array_from_context(
  sarl_context_t* context,  sarl_array_t *metrics);

sarl_array_t* sarl_diagram_layout_array_from_lattice(
  sarl_lattice_t* lattice,  sarl_array_t *metrics);

#endif
