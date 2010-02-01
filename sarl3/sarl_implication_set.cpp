#include "sarl_implication_set.h"
#include "sarl_exceptions.h"

#include "sarl_power_set.h"
#include "sarl_power_map.h"
#include "sarl_bit_set.h"
#include "sarl_context.h"

#include <memory.h>
#include <gc/gc.h>

sarl_implication_set_t* sarl_implication_set_default_new()
{
  sarl_implication_set_t *cc = (sarl_implication_set_t *)
    GC_MALLOC(sizeof(sarl_implication_set_t));
  sarl_return_zero_error_if_zero(cc);
  memset(cc, sizeof(*cc), 0);

  cc->premises = sarl_power_set_default_new();
  cc->conclusions = sarl_power_map_default_new();
  cc->intents = sarl_power_set_default_new();

  return cc;
};

sarl_bit_set_t *
  sarl_implication_set_closure(
    sarl_implication_set_t* imps, sarl_bit_set_t *A
  ) 
{
  sarl_bit_set_t *p;
  sarl_bit_set_t *B = sarl_bit_set_dup(A);
  sarl_bit_set_t *conc;
  sarl_unsigned_int index;

  A = sarl_bit_set_dup(A); // copy A so as not to modify the argument
  while(1) {
    SARL_POWER_SET_FOR_EACH_INDEX(index, p, imps->premises);
      if ( sarl_bit_set_is_subset(p, A) ) {
        conc = sarl_power_map_get(imps->conclusions, index);
        sarl_bit_set_or_equals(B, conc);
      };
    SARL_POWER_SET_END;

    if ( sarl_bit_set_is_equal(A, B) ) {
      return B;
    }
    else {
      sarl_bit_set_assign_equals(A, B);
    }
  };

  // code cannot reach this point, return is inside the loop
}

void sarl_bit_set_clear_range(
  sarl_bit_set_t *A, sarl_unsigned_int first, sarl_unsigned_int last
)
{
  sarl_unsigned_int i;
  for(i=first;i<=last;++i) {
    sarl_bit_set_clear(A, i);
  };
};

sarl_bit_set_t* sarl_bit_set_plus_i(
  sarl_bit_set_t *A, sarl_unsigned_int i, sarl_unsigned_int n)
{
  sarl_bit_set_t *A_plus_i = sarl_bit_set_dup(A);
  sarl_bit_set_clear_range(A_plus_i, i+1, n);
  sarl_bit_set_set(A_plus_i, i);

  return A_plus_i;
};

inline sarl_unsigned_int sarl_bit_set_leq_i(
  sarl_bit_set_t *A, sarl_unsigned_int i, sarl_bit_set_t *B)
{
  // is the smallest element in the symmetric difference equal to i
  sarl_bit_set_t *B_minus_A = sarl_bit_set_sym_diff(B, A);
  return sarl_bit_set_first(B_minus_A) == i;
};

sarl_bit_set_t* sarl_next_psudo_intent(
  sarl_implication_set_t *imps, sarl_bit_set_t *A, sarl_context_t *K)
{
  sarl_unsigned_int i;
  sarl_bit_set_t *A_plus_i, *A_plus_i_closure;
  sarl_unsigned_int n = sarl_bit_set_last(K->M);

  for(i=n;i>0;--i) {
    if ( ! sarl_bit_set_is_set(A, i) ) {
      A_plus_i = sarl_bit_set_plus_i(A, i, n);
      A_plus_i_closure = sarl_implication_set_closure(imps, A_plus_i);
      if ( sarl_bit_set_leq_i(A, i, A_plus_i_closure) ) {
        return A_plus_i_closure;
      }
    }
  }

  return 0;
};

void sarl_implication_set_insert(sarl_implication_set_t *imps, sarl_bit_set_t *premise, sarl_bit_set_t *conclusion)
{
  if ( sarl_bit_set_is_equal(premise, conclusion) ) {
    sarl_power_set_insert(imps->intents, premise);
  }
  else {
    sarl_unsigned_int index = sarl_power_set_insert(imps->premises, premise);
    sarl_power_map_set(imps->conclusions, index, sarl_bit_set_minus(conclusion, premise));
  }
};
    
sarl_implication_set_t *sarl_context_psudo_intents(sarl_context_t *K)
{
  sarl_implication_set_t *imps = sarl_implication_set_default_new();
  sarl_bit_set_t *A = sarl_bit_set_default_new(), *B;

  while( 0 != (B = sarl_next_psudo_intent(imps, A, K)) ) {
    sarl_implication_set_insert(
      imps, B, sarl_context_attribute_closure(K, B)
    );
    A = B;
  };

  return imps;
};
