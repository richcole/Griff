#include "sarl_ksubset.h"
#include "sarl_assert.h"
#include "sarl_bit_set.h"

sarl_unsigned_int n_choose_k(sarl_unsigned_int n, sarl_unsigned_int k)
{
  sarl_signed_int result = 1;
  sarl_unsigned_int j;

  j = n;
  while(j > k) { result *= j; --j; };

  j = n - k;
  while(j > 0) { result /= j; --j; };

  return result;
};

int main() {
  sarl_unsigned_int t = 5;
  sarl_unsigned_int n = 10;
  sarl_unsigned_int i, j;

  sarl_ksubset_t *s;
  
  s = sarl_ksubset_new (n, t);

  sarl_unsigned_int count = 0;

  do {
    for (i=1;i<=t;i++) {
      //       printf ("%u ", sarl_int_array_get(s->c, i));
    }
    //     printf ("\n");
    ++count;
  } while (sarl_ksubset_has_next(s));

  sarl_int_array_t *a = 0;
  for (i = 0; i<=10; i++) {
    SARL_KSUBSET_FOR_EACH(a, n, i);
      for (j=1; j<=i; j++) {
        printf ("%u ", sarl_int_array_get(a, j));
      }
      printf ("\n");
    SARL_KSUBSET_END;
    printf ("------\n");
  }


  sarl_test_assert(count == n_choose_k(n, t));

  sarl_int_array_t *arr = 0;
  count = 0;
  SARL_KSUBSET_FOR_EACH(arr, n, t);
    ++count;
  SARL_KSUBSET_END;

  sarl_test_assert(count == n_choose_k(n, t));

  sarl_bit_set_t *subset, *bset = sarl_bit_set_default_new();
  {
    sarl_unsigned_int i;
    for(i=1;i<=12;i+=2) sarl_bit_set_set(bset, i);
  }

  count = 0;
  SARL_KSUBSET_FOR_EACH_BIT_SET(subset, bset, t);
    ++count;
    sarl_test_assert(sarl_bit_set_count(subset) == t);
  SARL_KSUBSET_END_BIT_SET;

  sarl_test_assert(count == n_choose_k(sarl_bit_set_count(bset), t));

  return 0;
}
