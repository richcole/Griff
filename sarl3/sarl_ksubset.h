#ifndef SARL_KSUBSETS_H
#define SARL_KSUBSETS_H

#include "sarl_int_array.h"
#include "sarl_types.h"

typedef struct _sarl_ksubset_t sarl_ksubset_t;

struct _sarl_ksubset_t {
  sarl_unsigned_int k;
  sarl_unsigned_int n;
  sarl_unsigned_int j;
  sarl_int_array_t *c;
};

// get a new instance
sarl_ksubset_t* sarl_ksubset_new (sarl_unsigned_int n, sarl_unsigned_int k);

// generate next one
bool sarl_ksubset_has_next (sarl_ksubset_t *s);

// get as list
//sarl_int_array_t* sarl_ksubset_get_list (sarl_ksubset_t *);

// get a bit set
//sarl_bit_set_t* sarl_ksubset_get_bitset (sarl_ksubset_t *s);

#define SARL_KSUBSET_FOR_EACH(arr, n, k) \
  { sarl_ksubset_t *__ksubset = sarl_ksubset_new(n, k); \
    do { arr = __ksubset->c;

#define SARL_KSUBSET_END \
  } while ( sarl_ksubset_has_next(__ksubset) ); }

#define SARL_KSUBSET_FOR_EACH_BIT_SET(subset, bset, k) \
  { sarl_int_array_t *__arr; \
    sarl_bit_set_t   *__bsubset = sarl_bit_set_default_new(); \
    SARL_KSUBSET_FOR_EACH(__arr, sarl_bit_set_count(bset), k); \
    sarl_bit_set_assign_k_from_array_plus_one(__bsubset, k, __arr, bset); \
    subset = __bsubset;

#define SARL_KSUBSET_END_BIT_SET \
  SARL_KSUBSET_END \
  }



#endif

  
  

