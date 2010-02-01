#ifndef SARL_LATTICE_MAP_H
#define SARL_LATTICE_MAP_H

#include <gc/gc.h>
#include "sarl_types.h"
#include "sarl_int_array.h"

/* a lattice map is a mapping from the concepts of one lattice to the concepts fo another lattice */

struct _sarl_lattice_map_t {
  sarl_lattice_t    *domain;
  sarl_lattice_t    *range;
  sarl_int_array_t  *map;
  sarl_int_array_t  *comap;
};

inline sarl_lattice_map_t* sarl_lattice_map_new(
  sarl_lattice_t* domain, sarl_lattice_t *range) 
{
  sarl_lattice_map_t *lattice_map = (sarl_lattice_map_t *)
    GC_MALLOC(sizeof(sarl_lattice_map_t));

  sarl_return_zero_error_if_zero(lattice_map);

  lattice_map->domain = domain;
  lattice_map->range = range;
  lattice_map->map = sarl_int_array_default_new();

  return lattice_map;
};


#endif
