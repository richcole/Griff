#ifndef SARL_TYPES_H
#define SARL_TYPES_H

#include <sys/types.h>

typedef size_t  sarl_unsigned_int;
typedef ssize_t sarl_signed_int;
typedef char*   sarl_string_ptr;

typedef struct _sarl_bit_set_t sarl_bit_set_t;
typedef struct _sarl_power_set_t sarl_power_set_t;
typedef struct _sarl_power_map_t sarl_power_map_t;
typedef struct _sarl_context_t sarl_context_t;
typedef struct _sarl_clarified_context_t sarl_clarified_context_t;
typedef struct _sarl_lattice_t sarl_lattice_t;
typedef struct _sarl_rect_t sarl_rect_t;
typedef struct _sarl_point_t sarl_point_t;
typedef struct _sarl_diagram_t sarl_diagram_t;
typedef struct _sarl_string_table_t sarl_string_table_t;
typedef struct _sarl_int_array_t sarl_int_array_t;
typedef struct _sarl_array_t sarl_array_t;
typedef struct _sarl_solution_t sarl_solution_t;
typedef struct _sarl_search_point_t sarl_search_point_t;
typedef struct _sarl_int_set_t sarl_int_set_t;
typedef struct _sarl_hash_map_t sarl_hash_map_t;
typedef struct _sarl_hash_map_t sarl_point_set_t; /* alias for hash_map */
typedef struct _sarl_sat_solver_t sarl_sat_solver_t;
typedef struct _sarl_clause_t sarl_clause_t;
typedef struct _sarl_lattice_map_t sarl_lattice_map_t;
typedef struct _sarl_concept_interp_t sarl_concept_interp_t;
typedef struct _sarl_contingent_interp_t sarl_contingent_interp_t;
typedef struct _sarl_lattice_view_interp_t sarl_lattice_view_interp_t;
typedef struct _sarl_clause_factoring_t sarl_clause_factoring_t;
typedef struct _sarl_implication_set_t sarl_implication_set_t;


#define sarl_unsigned_int_num_bits (sizeof(sarl_unsigned_int) * 8)
#define sarl_max_of(x,y) (((x) > (y)) ? (x) : (y))
#define sarl_min_of(x,y) (((x) < (y)) ? (x) : (y))

#define sarl_in_range(v, lower, upper) \
  (((v) < (lower)) ? (lower) : ((v) > (upper) ? (upper) : (v)))

#define sarl_is_table_entry_valid(p) (((sarl_unsigned_int)p)>0x1)
#define sarl_potential_code_leak(x)
#define sarl_pi 3.14159
#define sarl_pi_on_2 1.57080

#define SARL_C_HEADER_BEGIN 
#define SARL_C_HEADER_END   

#endif
