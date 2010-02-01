#include "sarl_string_table.h"
#include "sarl_bit_set.h"

void sarl_string_table_print_bit_set_to_file(sarl_string_table_t *st, sarl_bit_set_t *bs, FILE *outp)
{
  sarl_unsigned_int i, is_first = 1;
  char *s;
  fprintf(outp, "{");

  SARL_BIT_SET_FOR_EACH(i, bs);
  if ( is_first ) {
    is_first = 0;
  }
  else {
    fprintf(outp, ", ");
  }
  s = sarl_string_table_get(st, i);
  if ( s ) {
    fprintf(outp, "%s", s);
  }
  SARL_BIT_SET_END;
  fprintf(outp, "}");
};
