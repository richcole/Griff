#ifndef SARL_STRING_TABLE_H
#define SARL_STRING_TABLE_H

#include "sarl_types.h"
#include "sarl_exceptions.h"

#include <gc/gc.h>
#include <string.h>

struct _sarl_string_table_t {
  sarl_unsigned_int length;
  sarl_string_ptr *table;
};
typedef struct _sarl_string_table_t sarl_string_table_t;

inline sarl_string_ptr sarl_string_ptr_dup(sarl_string_ptr str)
{
  sarl_unsigned_int length = strlen(str);
  sarl_string_ptr dup_str = (sarl_string_ptr)GC_MALLOC_ATOMIC(length+1);
  if ( dup_str ) {
    if ( str ) { 
      memcpy(dup_str, str, length); 
    };
    dup_str[length] = 0;
  }
  return dup_str;
};

inline sarl_string_table_t* sarl_string_table_new(sarl_unsigned_int length)
{
  sarl_string_table_t* stab = 
    (sarl_string_table_t *)GC_MALLOC(sizeof(sarl_string_table_t));

  if (stab) {
    stab->length = length;
    stab->table = (sarl_string_ptr *)GC_MALLOC(sizeof(sarl_string_ptr)*length);
    if (stab->table) {
      memset(stab->table, 0, sizeof(sarl_string_ptr)*length);
    }
    else {
      stab = 0;
      sarl_out_of_memory_error;
    }
  }
  else {
    sarl_out_of_memory_error;
  }
  return stab;
};

inline sarl_string_table_t* sarl_string_table_default_new() {
  return sarl_string_table_new(12);
};

inline void sarl_string_table_extend(
  sarl_string_table_t* stab, sarl_unsigned_int length
)
{
  sarl_string_ptr* new_table = (sarl_string_ptr *)
    GC_MALLOC(sizeof(sarl_string_ptr)*length);
  if (new_table != 0) {
    memcpy(new_table, stab->table, sizeof(sarl_bit_set_t *)*stab->length); 
    // free(stab->table);
    stab->table = new_table;
    stab->length = length;
  }
  else {
    sarl_out_of_memory_error;
  }
};

inline sarl_unsigned_int sarl_string_table_count(sarl_string_table_t *stab)
{
  sarl_unsigned_int index, prev = 0;
  if ( stab == 0 ) {
    sarl_null_pointer_error;
  }
  else {
    for(index=0;index<stab->length;++index) {
      if ( stab->table[index] != 0 ) prev = index+1;
    }
  }
  return prev;
};

inline void sarl_string_table_set(
  sarl_string_table_t* stab, sarl_unsigned_int index, sarl_string_ptr str
)
{
  if ( stab == 0 ) {
    sarl_null_pointer_error;
  }
  else {
    if ( index > stab->length ) {
      sarl_unsigned_int new_length = stab->length*2;
      while( index >= new_length ) new_length = ( (new_length) * 3) / 2;
      sarl_string_table_extend(stab, new_length);
    }
    if ( index <= stab->length ) {
      stab->table[index-1] = str ? sarl_string_ptr_dup(str) : 0;
    }
    else {
      sarl_internal_error;
    }
  }
}

inline void sarl_string_table_push(sarl_string_table_t *stab, sarl_string_ptr str)
{
  if ( stab == 0 ) {
    sarl_null_pointer_error;
  }
  else {
    sarl_string_table_set(stab, sarl_string_table_count(stab)+1, str);
  }
}

inline sarl_string_ptr sarl_string_table_get(
  sarl_string_table_t* stab, sarl_unsigned_int index
)
{
  if ( stab == 0 ) {
    sarl_null_pointer_error;
    return 0;
  }
  else {
    if ( index > stab->length ) {
      return 0;
    }
    else {
      return stab->table[index-1];
    }
  }
}

inline void sarl_string_table_populate(
  sarl_string_table_t* stab, 
  sarl_unsigned_int first_index, 
  sarl_unsigned_int last_index)
{
  char buf[1024];
  sarl_unsigned_int i;
  for(i=first_index;i<=last_index;++i) {
    sprintf(buf, "%u", i);
    sarl_string_table_set(stab, i, buf);
  }
};


void sarl_string_table_print_bit_set_to_file(sarl_string_table_t *st, sarl_bit_set_t *bs, FILE *outp);


#endif
