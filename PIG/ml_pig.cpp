/* 
Copyright 2004, Richard Cole

This file is part of Griff.

Griff is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your
option) any later version.

Griff is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with Griff; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "tree.h"
#include "tree_keys.h"
#include "mmap_alloc.h"
#include "hash.h"

#include <fcntl.h>
#include <unistd.h>
 
extern "C" {
#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/memory.h>
#include <caml/fail.h>
#include <caml/bigarray.h>
#include <caml/callback.h>
#include <caml/custom.h>

  value ml_pig_create_tree(value file_name);
  value ml_pig_create_tree_new(value file_name);

  value ml_pig_create_lex(value file_name);
  value ml_pig_create_lex_new(value file_name);

  value ml_pig_insert(value pig_value, value tuple);
  value ml_pig_remove(value pig_value, value tuple);
  value ml_pig_first_gte(value pig_value, value x);
  value ml_pig_succ(value pig_value, value tuple);

  value ml_pig_lex_index(value pig_value, value s);
  value ml_pig_lex_word(value pig_value, value i);
  value ml_pig_lex_index_count(value pig_value);

  value ml_do_pig_finalize(value pig_value);
}
#undef alloc


char* not_found_exception_name = "Not_found" ;

char *strerror(int errnum);

struct pig_t 
{
  mmap_file *mp;
  int fd;
};

void ml_pig_finalize(value pig_value)
{
  pig_t *pig = (pig_t *)Data_custom_val(pig_value);
  if ( pig->mp != 0 ) {
    mmap_finalize(pig->mp);
    pig->mp = 0;
  }
};

value ml_do_pig_finalize(value pig_value)
{
  CAMLparam1(pig_value);
  pig_t *pig = (pig_t *)Data_custom_val(pig_value);
  if ( pig->mp != 0 ) {
    mmap_finalize(pig->mp);
    pig->mp = 0;
  }
  CAMLreturn(Val_unit);
};

value create_pig(value filename)
{
  CAMLparam1(filename);
  CAMLlocal1(pig_value);
  
  pig_value = alloc_final(sizeof(pig_t), ml_pig_finalize, 1, 100); 
  pig_t *pig = (pig_t *)Data_custom_val(pig_value);
 
  pig->mp = 0;
  pig->fd = open(String_val(filename), O_RDWR | O_CREAT, 0666);

  if ( pig->fd == -1 ) {
    failwith( strerror(errno) );
  }

  pig->mp = mmap_init(pig->fd);
  CAMLreturn(pig_value);
};

void init_pig_tree(pig_t* pig)
{
  pig->mp = mmap_init(pig->fd);
  if ( mmap_getkey(pig->mp, PAYLOAD_SIZE_KEY) == 0 ) {
    mmap_setkey(pig->mp, PAYLOAD_SIZE_KEY, 256);
    TreePtr tree = TreePtr::alloc(pig->mp);
    tree.ptr(pig->mp)->_root = TreeNodePtr::null();
    tree.setkey(pig->mp, TREE_KEY);
  }
};
 
value ml_pig_create_tree(value file_name)
{
  CAMLparam1(file_name);
  CAMLlocal1(pig_value);

  pig_value = create_pig(file_name);
  pig_t* pig = (pig_t *)Data_custom_val(pig_value);
  if ( pig->fd == -1 ) {
    failwith( strerror(errno) );
  }
  tree_init(pig->mp);
  CAMLreturn(pig_value);
}

value ml_pig_create_tree_new(value file_name)
{
  CAMLparam1(file_name);
  CAMLlocal1(pig_value);

  unlink(String_val(file_name));

  pig_value = create_pig(file_name);
  pig_t* pig = (pig_t *)Data_custom_val(pig_value);
  if ( pig->fd == -1 ) {
    failwith( strerror(errno) );
  }
  tree_init(pig->mp);
  CAMLreturn(pig_value);
}

value ml_pig_insert(value pig_value, value tuple)
{
  CAMLparam2(pig_value, tuple);

#ifdef DEBUG_INSERT
  static int count = 0;
  ++count;
#endif

  pig_t *pig = (pig_t *)Data_custom_val(pig_value);

  Tuple t;
  t[0] = Int_val(Field(tuple, 0));
  t[1] = Int_val(Field(tuple, 1));
  t[2] = Int_val(Field(tuple, 2));
  t[3] = Int_val(Field(tuple, 3));

  tree_get_tree(pig->mp).ptr(pig->mp)->insert(pig->mp, t);

  #ifdef DEBUG_INSERT
  bool fail_test = false;

  TreeNodeIterator it = 
    tree_get_tree(pig->mp).ptr(pig->mp)->first_gte(t, pig->mp);

  if ( it.is_null() ) {
    fail_test = true;
  }
  else {
    Tuple s;
    assign_eq(s,it.val());
    if ( cmp(s,t) != 0 ) {
      fail_test = true;
    }
  }

  if ( fail_test ) {
    value* id = caml_named_value(not_found_exception_name);
    if ( id == 0 ) {
      fprintf(stderr, "Can't get exception.\n");
    }
    raise_constant(*id);
  }
  #endif

  CAMLreturn(Val_unit);
};

value ml_pig_remove(value pig_value, value tuple)
{
  CAMLparam2(pig_value, tuple);

  pig_t *pig = (pig_t *)Data_custom_val(pig_value);

  Tuple t;
  t[0] = Int_val(Field(tuple, 0));
  t[1] = Int_val(Field(tuple, 1));
  t[2] = Int_val(Field(tuple, 2));
  t[3] = Int_val(Field(tuple, 3));

  tree_get_tree(pig->mp).ptr(pig->mp)->remove(pig->mp, t);

  CAMLreturn(Val_unit);
};

value ml_pig_first_gte(value pig_value, value x)
{
  CAMLparam2(pig_value, x);
  CAMLlocal1(r);
  
  Tuple t;
  t[0] = Int_val(Field(x, 0));
  t[1] = Int_val(Field(x, 1));
  t[2] = Int_val(Field(x, 2));
  t[3] = Int_val(Field(x, 3));

  pig_t *pig = (pig_t *)Data_custom_val(pig_value);

  TreeNodeIterator it = tree_get_tree(pig->mp).ptr(pig->mp)->first_gte(t, pig->mp);

  if ( it.is_null() ) {
    value* id = caml_named_value(not_found_exception_name);
    if ( id == 0 ) {
      fprintf(stderr, "Can't get exception.\n");
    }
    raise_constant(*id);
  }
  else {
    assign_eq(t,it.val());
    r = alloc_tuple(4);
    Store_field(r, 0, Val_int(t[0]));
    Store_field(r, 1, Val_int(t[1]));
    Store_field(r, 2, Val_int(t[2]));
    Store_field(r, 3, Val_int(t[3]));
  };

  CAMLreturn(r);
};

value ml_pig_succ(value pig_value, value tuple)
{
  CAMLparam2(pig_value, tuple);
  CAMLlocal1(r);
  
  Tuple t;
  t[0] = Int_val(Field(tuple, 0));
  t[1] = Int_val(Field(tuple, 1));
  t[2] = Int_val(Field(tuple, 2));
  t[3] = Int_val(Field(tuple, 3));

  pig_t *pig = (pig_t *)Data_custom_val(pig_value);

  TreeNodeIterator it = tree_get_tree(pig->mp).ptr(pig->mp)->first_gte(t, pig->mp);

  if ( it.is_null() || (it.next(), it.is_null()) ) {
    raise_constant(*caml_named_value(not_found_exception_name));
  }
  else {
    assign_eq(t, it.val());
    r = alloc_tuple(4);
    Store_field(r, 0, Val_int(t[0]));
    Store_field(r, 1, Val_int(t[1]));
    Store_field(r, 2, Val_int(t[2]));
    Store_field(r, 3, Val_int(t[3]));
    CAMLreturn(r);
  };
};

value ml_pig_create_lex(value file_name)
{
  CAMLparam1(file_name);
  CAMLlocal1(pig_value);
  
  pig_value = create_pig(file_name);
  pig_t *pig = (pig_t *)Data_custom_val(pig_value);
  hash_init(pig->mp);
  
  CAMLreturn(pig_value);
}

value ml_pig_create_lex_new(value file_name)
{
  CAMLparam1(file_name);
  value pig_value;
  
  unlink(String_val(file_name));
  pig_value = create_pig(file_name);
  pig_t *pig = (pig_t *)Data_custom_val(pig_value);
  hash_init(pig->mp);
  
  CAMLreturn(pig_value);
}

value ml_pig_lex_index(value pig_value, value s)
{
  CAMLparam2(pig_value, s);

  pig_t *pig = (pig_t *)Data_custom_val(pig_value);
  size_t index = hash_get_index(pig->mp, String_val(s), string_length(s));
  CAMLreturn(Val_int(index));
};

value ml_pig_lex_word(value pig_value, value i)
{
  CAMLparam2(pig_value, i);
  CAMLlocal1(s);
  
  pig_t *pig = (pig_t *)Data_custom_val(pig_value);
  size_t index = hash_get_word(pig->mp, Int_val(i));
  size_t len = *size_ref_from_index(pig->mp, index);
  s = alloc_string(len);
  memcpy(String_val(s), ptr_from_index(pig->mp, index), len);
  CAMLreturn(s);
};

value ml_pig_lex_index_count(value pig_value)
{
  CAMLparam1(pig_value);
  
  pig_t *pig = (pig_t *)Data_custom_val(pig_value);
  size_t index_count = hash_get_index_count(pig->mp);
  CAMLreturn(Val_int(index_count));
};

