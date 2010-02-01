#include <gc/gc.h>
#include "sarl_ksubset.h"
#include "sarl_int_array.h"
#include "sarl_types.h"



// get a new instance
sarl_ksubset_t* sarl_ksubset_new (sarl_unsigned_int n, sarl_unsigned_int k) {
  sarl_unsigned_int i;

  // alloc memory
  sarl_ksubset_t* s = 
    (sarl_ksubset_t *) GC_MALLOC(sizeof(sarl_ksubset_t));
  // generate array
  s->c = sarl_int_array_new(k+2); 
  // TODO: k+1 is enough, k+2 just for convenience: 1..k,k+1 instead 1..k-1,k

  // initialise array
  for (i=1; i<=k; i++) {
    sarl_int_array_set(s->c, i, i-1);
  }
  sarl_int_array_set(s->c, k+1, n);
  // initialise k and n
  s->k = k;
  s->n = n;
  
  return s;
}


// generate next one
bool sarl_ksubset_has_next (sarl_ksubset_t *s) {
  int gt;
  
  if (s->k == 0) {
    return false;
  }
  
  // trivial case of k=1
  if (s->k == 1) {
    if (sarl_int_array_get(s->c, s->k) + 1 < s->n) {
      sarl_int_array_incr(s->c, s->k);
      return true;
    }
  } else {
    
    // step R3
    if (s->k % 2 == 1) {
      // k is odd
      if (sarl_int_array_get(s->c, 1) + 1 < sarl_int_array_get(s->c, 2)) {
	sarl_int_array_incr(s->c, 1);
	return true;
      } else {
	s->j = 2;
	gt = 4;
      }
    } else {
      // k is even
      if (sarl_int_array_get(s->c, 1) > 0) {
	sarl_int_array_decr(s->c, 1);
	return true;
      } else {
	s->j = 2;
	gt = 5;
      }
    }
    
    
    while (gt < 6) {
      // step R4
      if (gt==4) {
	if (sarl_int_array_get(s->c, s->j) >= s->j) {
	  sarl_int_array_set(s->c, s->j, sarl_int_array_get(s->c, s->j) - 1);
	  sarl_int_array_set(s->c, s->j-1, s->j - 2);
	  return true;
	} else {
	  s->j++;
	  // jump to R5
	  gt = 5;
	}
      }
      
      // step R5    
      if (gt==5) {
	if (sarl_int_array_get(s->c, s->j) + 1 < 
	    sarl_int_array_get(s->c, s->j+1)) {
	  sarl_int_array_set(s->c, s->j-1, sarl_int_array_get(s->c, s->j));
	  sarl_int_array_set(s->c, s->j, sarl_int_array_get(s->c ,s->j) + 1);
	  return true;
	} else {
	  s->j++;
	  if (s->j <= s->k) {
	    // jump to R4
	    gt = 4;
	  } else {
	    gt = 6;
	  }
	}
      }
    }
  }
  return false;
}

