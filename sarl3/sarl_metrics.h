#ifndef SARL_METRICS_H
#define SARL_METRICS_H

#include "sarl_types.h"

sarl_signed_int num_edge_crossings_metric(sarl_diagram_t *diagram);
sarl_signed_int num_sym_children_nz_metric(sarl_diagram_t *diagram);
sarl_signed_int num_sym_children_metric(sarl_diagram_t *diagram);
sarl_signed_int num_reflections_metric(sarl_diagram_t *diagram);
sarl_signed_int num_vectors_metric(sarl_diagram_t *diagram);
sarl_signed_int num_unique_gradients_metric(sarl_diagram_t *diagram);
sarl_signed_int num_unique_gradients_abs_metric(sarl_diagram_t *diagram);
sarl_signed_int num_meet_vectors_metric(sarl_diagram_t *diagram);
sarl_signed_int num_chains_metric(sarl_diagram_t *diagram);
sarl_signed_int num_three_chains_metric(sarl_diagram_t *diagram);
sarl_signed_int total_edge_length_metric(sarl_diagram_t *diagram);
void each_path(
  sarl_diagram_t *diagram, sarl_unsigned_int x, sarl_int_array_t *xs,
  void *arg, void (*block)(void *arg, sarl_int_array_t *path)
);
void measure_path_width(void *arg, sarl_int_array_t *path);
sarl_signed_int ave_path_width_metric(sarl_diagram_t *diagram);
sarl_signed_int vector_size_metric(sarl_int_array_t *a);

sarl_signed_int num_well_placed_children_metric(sarl_diagram_t *diagram);
sarl_signed_int num_ok_placed_children_metric(sarl_diagram_t *diagram);
sarl_signed_int children_ballance_metric(sarl_diagram_t *diagram);

#endif
