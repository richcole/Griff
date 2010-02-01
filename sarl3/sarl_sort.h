#ifndef SARL_SORT_H
#define SARL_SORT_H

#include "sarl_types.h"
#include "sarl_exceptions.h"
#include "sarl_finalization.h"

#include "sarl_int_array.h"
#include <gc/gc.h>
#include <string.h>

inline sarl_int_array_t* sarl_int_array_sort(sarl_int_array_t *list)
{
  // return a permutation that causes list to be sorted.
  // do a bubble sort because it is easy to remember the swaps

  sarl_unsigned_int i, j, len = sarl_int_array_count(list);
  sarl_int_array_t  *perm = sarl_int_array_new(len);
  for(i=1;i<=len;++i) {
    sarl_int_array_set(perm, i, i);
  };
    
  for(i=1;i<=len;++i) {
    for(j=1;j<=len-i;++j) {
      if ( sarl_int_array_get(list, j+1) < sarl_int_array_get(list, j) ) {
        sarl_int_array_swap(list, j+1, j);
        sarl_int_array_swap(perm, j+1, j);
      }
    }
  }

  return perm;
};

#endif
