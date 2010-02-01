#include "sarl_context.h"
#include "sarl_string_table.h"
#include <ctype.h>

void strip(char *s) {
  char *last = s;
  while(*s) {
    if ( ! isspace(*s) ) last = s+1;
    ++s;
  }
  *last = 0;
};


sarl_unsigned_int 
  sarl_context_load_from_cxt(
    sarl_context_t *context, 
    sarl_string_table_t *objects, 
    sarl_string_table_t *attributes, FILE *inp
  )
{
  char buf[4096*4], *fgets_result;
  sarl_unsigned_int i, g, m, num_objects, num_attributes;

  // header
  fgets_result = fgets(buf, sizeof(buf), inp);
  if ( fgets_result == 0 || strcmp(buf, "B\n") != 0 ) {
    return 0;
  };

  // title
  fgets_result = fgets(buf, sizeof(buf), inp);
  if ( fgets_result == 0 ) {
    return 0;
  };

  // num objects
  fgets_result = fgets(buf, sizeof(buf), inp);
  if ( fgets_result == 0 || 1 != sscanf(buf, "%u", &num_objects) ) {
    return 0;
  };

  // num attributes
  fgets_result = fgets(buf, sizeof(buf), inp);
  if ( fgets_result == 0 || 1 != sscanf(buf, "%u", &num_attributes) ) {
    return 0;
  };

  for(i=1;i<=num_objects;++i) {
    fgets_result = fgets(buf, sizeof(buf), inp);
    if ( fgets_result == 0 ) {
      return 0;
    }
    else {
      strip(buf);
      sarl_string_table_set(objects, i, buf);
    }
  }

  for(i=1;i<=num_attributes;++i) {
    fgets_result = fgets(buf, sizeof(buf), inp);
    if ( fgets_result == 0 ) {
      return 0;
    }
    else {
      strip(buf);
      sarl_string_table_set(attributes, i, buf);
    }
  }

  for(m=1;m<=num_attributes;++m) {
    sarl_context_insert_attribute(context, m);
  }

  for(g=1;g<=num_objects;++g) {
    fgets_result = fgets(buf, sizeof(buf), inp);
    if ( fgets_result == 0 || num_attributes > strlen(buf) ) {
      return 0;
    }
    sarl_context_insert_object(context, g);
    for(m=1;m<=num_attributes;++m) {
      if ( buf[m-1] == 'x' ) {
        sarl_context_insert(context, g, m);
      }
      else if ( buf[m-1] == '.' ) {
      }
      else {
        return 0;
      }
    }
  };

  return 1;
};

void sarl_context_print_to_cxt(sarl_context_t *context, 
  sarl_string_table_t *gs, sarl_string_table_t *ms, FILE *file)
{
  sarl_unsigned_int g, m;

  fprintf(file, "B\n");
  fprintf(file, "\n");
  fprintf(file, "%u\n%u\n", 
    sarl_bit_set_count(context->G),
    sarl_bit_set_count(context->M)
  );

  SARL_BIT_SET_FOR_EACH(g, context->G);
    fprintf(file, "%s\n", sarl_string_table_get(gs, g));
  SARL_BIT_SET_END;

  SARL_BIT_SET_FOR_EACH(m, context->M);
    fprintf(file, "%s\n", sarl_string_table_get(ms, m));
  SARL_BIT_SET_END;

  SARL_BIT_SET_FOR_EACH(g, context->G);
  SARL_BIT_SET_FOR_EACH(m, context->M);
  if ( sarl_bit_set_is_set(sarl_context_object_intent(context, g), m) ) {
    fprintf(file, "x");
  }
  else {
    fprintf(file, ".");
  }
  SARL_BIT_SET_END;
  fprintf(file, "\n");
  SARL_BIT_SET_END;
};  
