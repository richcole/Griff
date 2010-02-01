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

#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/memory.h>
#include <caml/fail.h>
#include <caml/bigarray.h>
#include <caml/callback.h>
#include <caml/custom.h>

#define return_pointer(x) CAMLreturn((value) x)
#define return_value(x) CAMLreturn(x)
#define return_void CAMLreturn0

char *strerror(int errnum);

typedef unsigned int *BitVect; 

#define min(x,y) ((x) < (y) ? (x) : (y))
#define max(x,y) ((x) > (y) ? (x) : (y))

#define bit_vect_length(bit_vect) (((BitVect)bit_vect)[0])
#define bit_vect_word_size (sizeof(unsigned int)*8)
#define bit_vect_word(bit_vect,i) (((BitVect)bit_vect)[i+1])
#define bit_vect_offset(i) \
  ((i)%bit_vect_word_size)
#define bit_vect_index(i) \
  ((i)/bit_vect_word_size)
#define bit_vect_bit(bit_vect,i) \
  ((((BitVect)bit_vect)_word(((BitVect)bit_vect),bit_vect_index(i))>>bit_vect_offset(i))&0x1)
#define bit_vect_data(bit_vect) (((BitVect)bit_vect)+1)

value ml_bit_vect_new_with_bits(value num_bits)
{
  CAMLparam1(num_bits);

  unsigned int num_words = ((Int_val(num_bits)-1)/bit_vect_word_size)+1;
  
  value result = (value)calloc(bit_vect_word_size, num_words+1);
  bit_vect_length(result) = num_words;
  
  return_pointer(result);
}

value ml_bit_vect_new_with_length(value num_words)
{
  CAMLparam1(num_words);

  value result = (value)calloc(sizeof(BitVect[0]), Int_val(num_words)+1);
  bit_vect_length(result) = Int_val(num_words);
  
  return_pointer(result);
}

value ml_bit_vect_set(value bs, value i)
{
  CAMLparam2(bs, i);

  value result = ml_bit_vect_new_with_bits(Val_int(
    max(bit_vect_length(bs)*bit_vect_word_size, Int_val(i)+1)
  ));
  memcpy(
    bit_vect_data(result),
    bit_vect_data(bs),
    bit_vect_word_size*min(bit_vect_length(result),bit_vect_length(bs))
  );

  bit_vect_word(result, bit_vect_index(Int_val(i))) |= 
    0x1 << bit_vect_offset(Int_val(i));

  return_pointer(result);
};

value ml_bit_vect_get(value bs, value i)
{
  CAMLparam2(bs, i);

  if ( bit_vect_index(Int_val(i)) < bit_vect_length(bs) ) {
    CAMLreturn(Val_bool(
      bit_vect_word(bs, bit_vect_index(Int_val(i))) & (0x1 << bit_vect_offset(Int_val(i)))
    ));
  }
  else {
    CAMLreturn(Val_false);
  }
};

value ml_bit_vect_length(value bs)
{
  CAMLparam1(bs);

  return Val_int(bit_vect_length(bs) * bit_vect_word_size);
};

value ml_bit_vect_clear(value bs, value i)
{
  CAMLparam2(bs, i);

  value result = ml_bit_vect_new_with_bits(Val_int(
    max(bit_vect_length(bs)*bit_vect_word_size, Int_val(i))
  ));
  memcpy(
    bit_vect_data(result),
    bit_vect_data(bs),
    bit_vect_word_size*min(bit_vect_length(result),bit_vect_length(bs))
  );

  bit_vect_word(result, bit_vect_index(Int_val(i))) &= 
    ~(0x1 << bit_vect_offset(Int_val(i)));

  return_pointer(result);
};

value ml_bit_vect_and(value s, value t)
{
  CAMLparam2(s,t);
  
  value result = ml_bit_vect_new_with_length(Val_int(
    min(bit_vect_length(s),bit_vect_length(t))
  ));
  unsigned int i;
  for(i=0;i<bit_vect_length(result);++i) {
    bit_vect_word(result,i) =
      bit_vect_word(s,i) & bit_vect_word(t,i);
  }

  return_pointer(result);
};

value ml_bit_vect_or(value s, value t)
{
  CAMLparam2(s,t);
  
  value result = ml_bit_vect_new_with_length(Val_int(
    max(bit_vect_length(s),bit_vect_length(t))
  ));
  unsigned int i;
  for(i=0;i<bit_vect_length(result);++i) {
    bit_vect_word(result,i) =
      bit_vect_word(s,i) | bit_vect_word(t,i);
  }

  return_pointer(result);
}

value ml_bit_vect_minus(value s, value t)
{
  CAMLparam2(s,t);
  
  value result = ml_bit_vect_new_with_length(Val_int(
    max(bit_vect_length(s),bit_vect_length(t))
  ));
  unsigned int i;
  for(i=0;i<bit_vect_length(result);++i) {
    bit_vect_word(result,i) =
      bit_vect_word(s,i) & ~bit_vect_word(t,i);
  }

  return_pointer(result);
}
  
  
  
