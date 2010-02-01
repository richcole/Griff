#ifndef SARL_CONTEXT_FORMATS_H

#include "sarl_types.h"

void sarl_context_print_to_cxt(sarl_context_t *context, 
  sarl_string_table_t *gs, sarl_string_table_t *ms, FILE *file);

sarl_unsigned_int 
  sarl_context_load_from_cxt(
    sarl_context_t *context, 
    sarl_string_table_t *objects, 
    sarl_string_table_t *attributes, FILE *inp
  );


#endif
