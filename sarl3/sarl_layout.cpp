#include "sarl_layout.h"
#include "sarl_bit_set.h"
#include "sarl_power_map.h"
#include "sarl_int_array.h"
#include "sarl_array.h"
#include "sarl_context.h"
#include "sarl_point_set.h"
#include "sarl_lattice.h"
#include "sarl_diagram.h"
#include "sarl_metrics.h"
#include "sarl_point_set.h"
#include "sarl_sort.h"

/* This module is about automatic layout of concept lattices. It uses
 * the incremental layout approach.
 */

/* Macros */

#define SARL_LAYOUT_SCALE 1

/* Prototypes */

static void 
  calculate_diagram(
    sarl_diagram_t *diagram, sarl_unsigned_int num_attributes, 
    sarl_int_array_t* attributes, sarl_array_t *attr_vectors, 
    sarl_int_array_t *vector, sarl_lattice_t *lattice
  );

static void 
  incremental_search(sarl_search_point_t *sol, sarl_unsigned_int i);

static void 
  create_attr_vectors(sarl_search_point_t *sol);

static void 
  create_attr_vectors(sarl_search_point_t *sol);

static void 
  sarl_setup_search_point(sarl_search_point_t *sol, sarl_context_t *context);

static sarl_solution_t* 
  sarl_solution_best(sarl_search_point_t *sol);

/* Definitions */

sarl_int_array_t* sarl_order_attributes_by_implication(
  sarl_power_map_t *imp, 
  sarl_bit_set_t   *M /* attributes */
)
{
  sarl_bit_set_t   *rest = sarl_bit_set_dup(M);
  sarl_bit_set_t   *downset, *cand;
  sarl_int_array_t *result = sarl_int_array_default_new();
  sarl_bit_set_t   *image;
  
  sarl_unsigned_int m;

  downset =  sarl_bit_set_default_new();

  while(! sarl_bit_set_is_empty(rest)) {
    sarl_bit_set_clear_all(downset);
    SARL_BIT_SET_FOR_EACH(m, rest);
      image = sarl_power_map_get(imp, m);
      if ( image != 0 ) {
        sarl_bit_set_or_equals(downset, image);
      }
    SARL_BIT_SET_END;
    cand = sarl_bit_set_minus(rest, downset);
  
    m = sarl_bit_set_first(cand);
    sarl_int_array_push(result, m);
    sarl_bit_set_clear(rest, m);
  }

  return result;
};

sarl_int_array_t *
  sarl_context_order_attributes_by_extent_count(sarl_context_t *context)
{
  sarl_int_array_t* extent_counts = 
    sarl_int_array_new(sarl_bit_set_last(context->M));
  sarl_unsigned_int m, i, num_attributes;

  SARL_BIT_SET_FOR_EACH(m, context->M);
    sarl_int_array_set(
      extent_counts, m, 
      1 + sarl_bit_set_count(sarl_context_attribute_extent(context, m))
    );
  SARL_BIT_SET_END;

  sarl_int_array_t *perm = sarl_int_array_sort(extent_counts);
  sarl_int_array_t *filt_perm = 
    sarl_int_array_new(num_attributes = sarl_bit_set_count(context->M));

  // filter out the zeros and reverse the order because we want largest 
  // to smallest
  sarl_unsigned_int ix = 0;
  SARL_INT_ARRAY_FOR_EACH_INDEX(i, m, perm);
    if ( m && sarl_bit_set_is_set(context->M, m) ) {
      sarl_int_array_set(filt_perm, num_attributes - ix, m);
      ++ix;
    }
  SARL_INT_ARRAY_END;

  return filt_perm;
};

void create_lattices(sarl_search_point_t *sol, sarl_context_t *context)
{
  sarl_unsigned_int m;
  sarl_bit_set_t    *ms = sarl_bit_set_default_new();
  sarl_context_t    *subcontext;
  sol->lattices = sarl_array_default_new();

  SARL_INT_ARRAY_FOR_EACH(m, sol->attributes);
    sarl_bit_set_set(ms, m);
    subcontext = sarl_context_attribute_subcontext(context, ms);
    sarl_array_push(sol->lattices, sarl_lattice_new(subcontext));
  SARL_INT_ARRAY_END;
};

sarl_unsigned_int sat(sarl_search_point_t *sol, sarl_unsigned_int i)
{
  sarl_unsigned_int  concept;
  sarl_lattice_t    *lattice = 
    (sarl_lattice_t *) sarl_array_get(sol->lattices, i);
  
  calculate_diagram(
    sol->diagram, i, sol->attributes, sol->attr_vectors, sol->vector, lattice);

  sarl_point_set_clear(sol->point_set);
  SARL_LATTICE_FOR_EACH(concept, lattice);
    sarl_point_t *p = sarl_diagram_get_concept_pos(sol->diagram, concept);
    if ( sarl_point_set_is_member(sol->point_set, p) ) {
      return 0;
    }
    sarl_point_set_insert(sol->point_set, p);
  SARL_LATTICE_END;
  
  return 1;
};

sarl_unsigned_int sarl_metrics_is_better(
  sarl_int_array_t *a, sarl_int_array_t *b)
{
  sarl_unsigned_int i, better_on_one = 0;
  for(i=0;i<a->length&&i<b->length;++i) {
    if ( SARL_UNSIGN_TO_SIGN(a->table[i]) > 
      SARL_UNSIGN_TO_SIGN(b->table[i]) 
    ) {
      return 0;
    }
    if ( SARL_UNSIGN_TO_SIGN(a->table[i]) < 
      SARL_UNSIGN_TO_SIGN(b->table[i]) ) {
      better_on_one = 1;
    }
  }
  return better_on_one;
};

sarl_unsigned_int sarl_metrics_cmp(sarl_int_array_t *a, sarl_int_array_t *b)
{
  sarl_unsigned_int i, a_better = 0, b_better = 0;
  for(i=0;i<a->length&&i<b->length;++i) {
    if ( SARL_UNSIGN_TO_SIGN(a->table[i]) > 
      SARL_UNSIGN_TO_SIGN(b->table[i]) 
    ) {
      a_better = 1;
    }
    if ( SARL_UNSIGN_TO_SIGN(a->table[i]) < SARL_UNSIGN_TO_SIGN(b->table[i]) ) {
      b_better = 1;
    }
  }
  if ( a_better && b_better ) return 1;  /* incomparable   */
  else if ( a_better ) return 2;         /* a is better    */
  else if ( b_better ) return 3;         /* b is better    */
  else return 4;                         /* they are equal */
};

void store_solution(sarl_search_point_t *sol)
{
  sarl_solution_t *s = sarl_solution_new(sol->vector);

  sarl_unsigned_int metric_index = 0;
  sarl_signed_int last_metric = 0;

  #define ADD_DIAGRAM_METRIC(__metric)  \
    sarl_int_array_set(s->metrics, ++metric_index, \
      last_metric = SARL_SIGN_TO_UNSIGN(__metric(sol->diagram)))

  ADD_DIAGRAM_METRIC(num_edge_crossings_metric);
  // planar metric, if the number of edge crossings is zero
  sarl_int_array_set(s->metrics, ++metric_index, \
    last_metric = (last_metric == 0));
  ADD_DIAGRAM_METRIC(num_sym_children_metric);
  ADD_DIAGRAM_METRIC(num_sym_children_nz_metric);
  ADD_DIAGRAM_METRIC(num_reflections_metric);
  ADD_DIAGRAM_METRIC(num_vectors_metric);
  ADD_DIAGRAM_METRIC(num_chains_metric);
  ADD_DIAGRAM_METRIC(total_edge_length_metric);
  ADD_DIAGRAM_METRIC(ave_path_width_metric);
  ADD_DIAGRAM_METRIC(num_meet_vectors_metric);
  ADD_DIAGRAM_METRIC(num_well_placed_children_metric);
  ADD_DIAGRAM_METRIC(num_ok_placed_children_metric);
  ADD_DIAGRAM_METRIC(children_ballance_metric);
  ADD_DIAGRAM_METRIC(num_three_chains_metric);
  ADD_DIAGRAM_METRIC(num_unique_gradients_metric);
  ADD_DIAGRAM_METRIC(num_unique_gradients_abs_metric);

  sol->num_metrics = metric_index;
  ++sol->sol_count;

  /*
  fprintf(stderr, "solution:");

  fprintf("metrics=");
  SARL_INT_ARRAY_FOR_EACH(i, s->metrics);
    fprintf(stderr, "  %d, ", SARL_UNSIGN_TO_SIGN(i));
  SARL_INT_ARRAY_END;

  fprintf(stderr, " vectors=");
  for(i=0;i<sol->num_attributes;++i) {
    fprintf(stderr, "%u, ", sol->vector->table[i]);
  };

  fprintf(stderr, "\n");
  */

  sarl_unsigned_int i, is_stored = 0;
  sarl_solution_t *tmp;

  for(i=1;i<=sol->solutions->length;++i) {
    tmp = (sarl_solution_t *)(sol->solutions->table[i-1]);
    if ( tmp ) {
      sarl_unsigned_int cmp = sarl_metrics_cmp(s->metrics, tmp->metrics);
      if ( ! is_stored ) {
        if ( cmp == 2 ) {
          /* it is better, slot it in here */
          sol->solutions->table[i-1] = s;
          is_stored = 1;
        }
        else if ( cmp == 3 || cmp == 4 ) {
          /* worse or equal, we don't want it, and it can't 
             displace anything else anyway 
          */
          is_stored = 1;
          break;
        }
      }
      else {
        if ( cmp == 2 ) {
          sol->solutions->table[i-1] = 0;
        }
      }
    }
  }
  if ( ! is_stored ) {
    sarl_array_push(sol->solutions, s);
  }

  // make the entires in the array continuous
  sarl_unsigned_int prev = 0;
  while(prev < sol->solutions->length) {
    for(;prev<sol->solutions->length && sol->solutions->table[prev];++prev);
    for(i=prev+1;i<sol->solutions->length&& sol->solutions->table[i] == 0;++i);
    if ( i<sol->solutions->length ) {
      sol->solutions->table[prev] = sol->solutions->table[i];
      sol->solutions->table[i] = 0;
    }
    ++prev;
  }

  /*
    fprintf(stderr, "Current Number of Solutions: %u (%u)\n", 
    sarl_array_count(sol->solutions), sol->sol_count);
  */
  
  if (sol->sol_count >= sol->max_sol_count) {
    sol->terminate = 1;
    fprintf(stderr, "terminate! num_solutions=%d\n", 
      sarl_array_count(sol->solutions));
  }
};

sarl_diagram_t* sarl_solution_best_diagram(sarl_search_point_t *sol)
{
  sarl_solution_t *s = (sarl_solution_t *)sarl_array_first(sol->solutions);
  if ( s == 0 ) {
    sarl_internal_error;
    return 0;
  }
  else {
    /* FIXME: manips in the viewer won't work need to calculate 
       concept offsets 
    */
    sarl_lattice_t *lattice = 
      (sarl_lattice_t*)sarl_array_get(sol->lattices, sol->num_attributes);
    calculate_diagram(
      sol->diagram, sol->num_attributes, sol->attributes, sol->attr_vectors, 
      s->vectors, lattice
    );
    return sol->diagram;
  }
} 

void incremental_search(sarl_search_point_t *sol, sarl_unsigned_int i) 
{
  if ( i > sol->num_attributes ) {
    store_solution(sol);
  }
  else {
    sarl_unsigned_int msb;
    for(msb=1;!sol->terminate&&msb<=sol->base;++msb) {
      sarl_int_array_set(sol->vector, i, msb);
      if ( sat(sol, i) ) {
        incremental_search(sol, i+1);
      }
    }
  }
}
    
void create_attr_vectors(sarl_search_point_t *sol)
{
  sarl_unsigned_int i;
  sarl_signed_int x = 0, y = SARL_LAYOUT_SCALE;
  sol->attr_vectors = sarl_array_new(sol->base);
  for(i=1;i<=sol->base;++i) {
    sarl_array_push(sol->attr_vectors, sarl_point_new(x, y));
    if ( x > 0 )
      x *= -1;
    else 
      x = (-x) + SARL_LAYOUT_SCALE;
  }
};

void sarl_setup_search_point(sarl_search_point_t *sol, sarl_context_t *context)
{
//  sarl_power_map_t *imp = 
//    sarl_power_map_inverse(sarl_context_attribute_imps(context));
//  sol->attributes = sarl_order_attributes_by_implications(imp, context->M);
  sol->attributes = sarl_context_order_attributes_by_extent_count(context);
  sol->num_attributes = sarl_bit_set_count(context->M);
  sol->diagram = sarl_diagram_new_from_context(context);
  sol->point_set = sarl_point_set_default_new();
  sol->solutions = sarl_array_default_new();

  create_lattices(sol, context);
  create_attr_vectors(sol);
};

void calculate_diagram(
  sarl_diagram_t *diagram, sarl_unsigned_int num_attributes, 
  sarl_int_array_t *attributes, sarl_array_t *attr_vectors, 
  sarl_int_array_t *vector,
  sarl_lattice_t *lattice)
{
  sarl_unsigned_int  i, concept, m, v;
  sarl_bit_set_t     *intent;

  // set the lattice
  diagram->lattice = lattice;

  // copy the attribute vectors
  for(i=1;i<=num_attributes;++i) {
    m = sarl_int_array_get(attributes, i);
    v = sarl_int_array_get(vector, i);
    if ( m != 0 ) {
      sarl_point_assign_eq(
        diagram->meet_vectors + (m-1), 
        (sarl_point_t *)attr_vectors->table[v-1]
      );
    };
  };

  // run the additivity calculation
  memset(diagram->concept_pos, 0, sizeof(sarl_point_t)*diagram->length);
  sarl_unsigned_int c;
  SARL_POWER_MAP_FOR_EACH(c, intent, diagram->lattice->concept_intents);
    SARL_BIT_SET_FOR_EACH(m, intent);
      sarl_point_plus_equals(
        diagram->concept_pos + (c-1), 
        diagram->meet_vectors + (m-1)
      );
    SARL_BIT_SET_END;
  SARL_POWER_MAP_END;

  // assign y position based on the rank
  SARL_POWER_MAP_FOR_EACH(concept, intent, lattice->concept_intents);
    sarl_diagram_get_concept_pos(diagram, concept)->y = 
      sarl_int_array_get(lattice->concept_uprank, concept);
  SARL_POWER_MAP_END;
};

sarl_diagram_t* sarl_diagram_layout_from_solution(
  sarl_lattice_t* lattice, 
  sarl_search_point_t *sol,
  sarl_solution_t* s
);

sarl_diagram_t* sarl_diagram_layout_from_lattice(sarl_lattice_t* lattice)
{
  fprintf(stdout, "Diagram Layout: Number of concepts=%d\n", 
    sarl_lattice_count(lattice));

  if ( sarl_lattice_count(lattice) > 40 ) {
    return sarl_diagram_new_from_lattice(lattice);
  };

  sarl_search_point_t *sol = sarl_search_point_new();

  sarl_context_t* reduced_context = sarl_lattice_reduced_context(lattice);

  sol->base = 10;
  sol->sol_count = 0;
  sol->max_sol_count = 40000;
  sarl_setup_search_point(sol, reduced_context);
  incremental_search(sol, 1);

  sarl_solution_t* s = sarl_solution_best(sol);
  return sarl_diagram_layout_from_solution(lattice, sol, s);
};

sarl_array_t* 
  sarl_diagram_layout_array_from_lattice(
    sarl_lattice_t* lattice, sarl_array_t *metrics)
{
  fprintf(stdout, "Diagram Layout: Number of concepts=%d\n", 
    sarl_lattice_count(lattice));

  sarl_array_t *layout_array = sarl_array_default_new();

  if ( sarl_lattice_count(lattice) > 40 ) {
    sarl_array_push(layout_array, sarl_diagram_new_from_lattice(lattice));
    return layout_array;
  };

  sarl_search_point_t *sol = sarl_search_point_new();
  sarl_context_t* reduced_context = sarl_lattice_reduced_context(lattice);

  sol->base = 10;
  sol->sol_count = 0;
  sol->max_sol_count = 4000;
  sarl_setup_search_point(sol, reduced_context);
  incremental_search(sol, 1);


  {
    sarl_solution_t* s;
    sarl_unsigned_int i = 0;

    SARL_ARRAY_FOR_EACH(sarl_solution_t, s, sol->solutions);
      ++i;
      sarl_array_push(
        layout_array, sarl_diagram_layout_from_solution(lattice, sol, s));
      sarl_array_push(metrics, s->metrics);
    SARL_ARRAY_END;
  }    
  return layout_array;
};

sarl_diagram_t* sarl_diagram_layout_from_solution(
  sarl_lattice_t *lattice, 
  sarl_search_point_t *sol,
  sarl_solution_t* s)
{
  sarl_diagram_t* result;

  if ( s != 0 ) {
    result = sarl_diagram_new(lattice->num_concepts);
    result->lattice = lattice;
    
    // copy the attribute vectors
    sarl_unsigned_int i, m, v;
    for(i=0;i<sol->num_attributes;++i) {
      m = sol->attributes->table[i];
      v = s->vectors->table[i];
      if ( m != 0 ) {
        sarl_point_assign_eq(
          result->meet_vectors + (m-1), 
          (sarl_point_t *)sol->attr_vectors->table[v-1]
        );
      };
    };
    
    // determine the x-coordinates relatively
    sarl_diagram_calculate_positions(result);
    
    // update according to rank assignment
    sarl_unsigned_int c, rank;
    SARL_LATTICE_FOR_EACH(c, lattice);
    rank = sarl_int_array_get(lattice->concept_uprank, c);
    result->concept_vectors[c-1].y = rank - result->concept_pos[c-1].y;
    result->concept_pos[c-1].y = rank;
    SARL_LATTICE_END;
    
    sarl_diagram_scale_diagram(result, SARL_DIAGRAM_SCALE);
  }
  else {
    result = sarl_diagram_new_from_lattice(lattice);
  }
    
  return result;
};

sarl_array_t* sarl_diagram_layout_array_from_context(
  sarl_context_t* context,  sarl_array_t *metrics)
{
  sarl_lattice_t* lattice = sarl_lattice_new(context);
  return sarl_diagram_layout_array_from_lattice(lattice, metrics);
};

sarl_diagram_t* sarl_diagram_layout_from_context(sarl_context_t* context)
{
  sarl_lattice_t* lattice = sarl_lattice_new(context);
  return sarl_diagram_layout_from_lattice(lattice);
};

sarl_int_array_t *sarl_collect_metric_maxes(sarl_array_t* sols, sarl_unsigned_int num_metrics)
{
  sarl_solution_t *s;
  sarl_int_array_t *maxes = sarl_int_array_default_new();
  sarl_unsigned_int i, first = 1;

  SARL_ARRAY_FOR_EACH(sarl_solution_t, s, sols);
    if ( first ) {
      first = 0;
      for(i=1;i<=num_metrics;i++) {
        sarl_int_array_set(
          maxes,
          i,
          sarl_int_array_get(s->metrics, i)
        );
      };
    }
    else {
      for(i=1;i<=num_metrics;i++) {
        sarl_int_array_set(
          maxes,
          i,
          SARL_SIGN_TO_UNSIGN(
            sarl_max_of(
              SARL_UNSIGN_TO_SIGN(sarl_int_array_get(maxes, i)), 
              SARL_UNSIGN_TO_SIGN(sarl_int_array_get(s->metrics, i))
            )
          )
        );
      };  
    };
  SARL_ARRAY_END;

  return maxes;
};

sarl_unsigned_int sarl_score_solution(
  sarl_int_array_t *metrics, sarl_int_array_t *maxes, sarl_unsigned_int num_metrics)
{
  sarl_signed_int max;
  sarl_signed_int metric;
  sarl_unsigned_int count = 0, i;

  for(i=1;i<=num_metrics;i++) {
    max = SARL_UNSIGN_TO_SIGN(sarl_int_array_get(maxes, i));
    metric = SARL_UNSIGN_TO_SIGN(sarl_int_array_get(metrics, i));
    if ( metric > max ) {
      sarl_internal_error;
    }
    else if ( metric == max ) {
      ++count;
    }
  };

  return count;
};

void sarl_print_solution(
  sarl_solution_t *s, sarl_int_array_t *maxes, sarl_unsigned_int number)
{
  fprintf(stdout, "solution %3d: ", number);
  sarl_unsigned_int index;
  sarl_unsigned_int len = sarl_int_array_count(s->metrics);
  for(index=1;index<=len;++index) {
    sarl_unsigned_int x  = sarl_int_array_get(s->metrics, index);
    sarl_unsigned_int mx = sarl_int_array_get(maxes, index);
    fprintf(stdout, "[%3d] %3d, ", SARL_UNSIGN_TO_SIGN(mx), SARL_UNSIGN_TO_SIGN(x));
  }
  fprintf(stdout, "\n");
};
        
sarl_solution_t* sarl_solution_best(sarl_search_point_t *sol)
{
  sarl_int_array_t *maxes = 
    sarl_collect_metric_maxes(sol->solutions, sol->num_metrics);
  sarl_solution_t *s = 0, *best_s = 0;
  sarl_unsigned_int score = 0, best_score = 0, i = 0, best_i = 0;

  SARL_ARRAY_FOR_EACH(sarl_solution_t, s, sol->solutions);
    ++i;
    sarl_print_solution(s, maxes, i);
    score = sarl_score_solution(s->metrics, maxes, sol->num_metrics);
    if ( score > best_score ) {
      best_score = score;
      best_s = s;
      best_i = i;
    };
  SARL_ARRAY_END;

  fprintf(stderr, "Selected diagram %u with score %u\n", best_i, best_score);
  return best_s;
};





