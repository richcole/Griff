#include "sarl_string_table.h"

#include "sarl_power_set.h"
#include "sarl_assert.h"
#include <sys/time.h>
#include <time.h>

int main(int argc, char **argv)
{
  fprintf(stdout, "Running %s...\n", argv[0]);

  sarl_string_table_t *stab = sarl_string_table_default_new();
  sarl_string_table_set(stab, 1, "one");
  sarl_string_table_set(stab, 1, "one");
  sarl_string_table_set(stab, 3, "three");
  sarl_string_table_set(stab, 7, "three");
  sarl_string_table_set(stab, 6, "7");
  sarl_string_table_set(stab, 4, "");
  sarl_string_table_set(stab, 5, 0);

  sarl_test_assert( strcmp(sarl_string_table_get(stab, 1), "one") == 0 );
  sarl_test_assert( sarl_string_table_get(stab, 2) == 0 );
  sarl_test_assert( strcmp(sarl_string_table_get(stab, 3), "three") == 0 );
  sarl_test_assert( strcmp(sarl_string_table_get(stab, 7), "three") == 0 );
  sarl_test_assert( strcmp(sarl_string_table_get(stab, 6), "7") == 0 );
  sarl_test_assert( strcmp(sarl_string_table_get(stab, 4), "") == 0 );
  sarl_test_assert( sarl_string_table_get(stab, 5) == 0 );
  sarl_test_assert( sarl_string_table_get(stab, 12) == 0 );

  sarl_string_table_set(stab, 12, "twelve");

  sarl_test_assert( strcmp(sarl_string_table_get(stab, 1), "one") == 0 );
  sarl_test_assert( sarl_string_table_get(stab, 2) == 0 );
  sarl_test_assert( strcmp(sarl_string_table_get(stab, 3), "three") == 0 );
  sarl_test_assert( strcmp(sarl_string_table_get(stab, 7), "three") == 0 );
  sarl_test_assert( strcmp(sarl_string_table_get(stab, 6), "7") == 0 );
  sarl_test_assert( strcmp(sarl_string_table_get(stab, 4), "") == 0 );
  sarl_test_assert( sarl_string_table_get(stab, 5) == 0 );
  sarl_test_assert( strcmp(sarl_string_table_get(stab, 12), "twelve") == 0 );



  stab = sarl_string_table_default_new();
  sarl_string_table_push(stab, "one");
  sarl_string_table_push(stab, "two");
  sarl_string_table_push(stab, "three");

  sarl_test_assert( strcmp(sarl_string_table_get(stab, 1), "one") == 0 );
  sarl_test_assert( strcmp(sarl_string_table_get(stab, 2), "two") == 0 );
  sarl_test_assert( strcmp(sarl_string_table_get(stab, 3), "three") == 0 );
  sarl_test_assert( sarl_string_table_count(stab) == 3); 

  fprintf(stdout, "Finished.\n");



};
