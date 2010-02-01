#ifndef SARL_LATTICE_VIEW_INTERP_H
#define SARL_LATTICE_VIEW_INTERP_H

#include "sarl_types.h"
#include "sarl_exceptions.h"
#include "sarl_array.h"
#include "sarl_label.h"

#include <gc/gc.h>
#include <string.h>

typedef struct _SarlLatticeView SarlLatticeView;

struct _sarl_concept_interp_t {
  sarl_contingent_interp_t *object_interp;
  sarl_contingent_interp_t *attribute_interp;
  sarl_bit_set_t           *realised_concepts;
};

struct _sarl_contingent_interp_t {
  sarl_string_table_t  *names;
  sarl_power_map_t     *contingents;
};

struct _sarl_lattice_view_interp_t {
  sarl_array_t          *labels;
  sarl_concept_interp_t *interp;
};

inline sarl_concept_interp_t* sarl_concept_interp_new()
{
  sarl_concept_interp_t *cc = (sarl_concept_interp_t *)
    GC_MALLOC(sizeof(sarl_concept_interp_t));
  sarl_return_zero_error_if_zero(cc);
  memset(cc, sizeof(*cc), 0);

  return cc;
};


inline sarl_contingent_interp_t* sarl_contingent_interp_new()
{
  sarl_contingent_interp_t *cc = (sarl_contingent_interp_t *)
    GC_MALLOC(sizeof(sarl_contingent_interp_t));
  sarl_return_zero_error_if_zero(cc);
  memset(cc, sizeof(*cc), 0);

  return cc;
};


inline sarl_lattice_view_interp_t* sarl_lattice_view_interp_new(
  sarl_concept_interp_t* interp
)
{
  sarl_lattice_view_interp_t *cc = (sarl_lattice_view_interp_t *)
    GC_MALLOC(sizeof(sarl_lattice_view_interp_t));
  sarl_return_zero_error_if_zero(cc);
  memset(cc, sizeof(*cc), 0);

  cc->labels = sarl_array_default_new();
  cc->interp = interp;

  return cc;
};

void sarl_lattice_view_interp_dispose(
  sarl_lattice_view_interp_t *interp
);

void sarl_lattice_view_interp_recalc_labels_for(
  SarlLatticeView                       *view, 
  sarl_lattice_view_interp_t            *view_interp,
  sarl_contingent_interp_t              *cont_interp,
  SarlLabelKind                          kind,
  sarl_signed_int                        ox, 
  sarl_signed_int                        oy
);

void sarl_lattice_view_set_current_interp(
  SarlLatticeView *view, 
  sarl_unsigned_int interp_index
);

void sarl_lattice_view_recalc_contingent_interp(
  SarlLatticeView *view, 
  sarl_lattice_view_interp_t *interp
);

sarl_unsigned_int sarl_label_view_add_interp(
  SarlLatticeView *view,
  sarl_concept_interp_t *interp
);

void sarl_label_view_remove_interp(
  SarlLatticeView *view,
  sarl_unsigned_int interp_index
);

void sarl_lattice_view_interp_rescale_labels(
  SarlLatticeView *view);

void sarl_lattice_view_interp_draw_labels(
  SarlLatticeView *view, 
  GdkDrawable *dest, GdkGC *gc);

sarl_unsigned_int sarl_lattice_view_label_at(
  SarlLatticeView       *view,                                               
  sarl_signed_int       x,
  sarl_signed_int       y
);

#endif
