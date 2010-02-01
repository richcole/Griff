#ifndef SARL_IMPLICATION_SET_H
#define SARL_IMPLICATION_SET_H

#include "sarl_types.h"

struct _sarl_implication_set_t {
  sarl_power_set_t *premises;
  sarl_power_map_t *conclusions;
  sarl_power_set_t *intents;
};

sarl_implication_set_t* sarl_implication_set_default_new();

/* return the stem base of attribute implications for a context. */
sarl_implication_set_t *sarl_context_psudo_intents(sarl_context_t *K);

/* find the closure of a set A with implications imps */
sarl_bit_set_t *
  sarl_implication_set_closure(
    sarl_implication_set_t* imps, sarl_bit_set_t *A
  );

#endif
