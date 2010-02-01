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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

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

#define EXCEPTION_FILE_EOF "File.Eof"

char *strerror(int errnum);

value open_input_file(value file_name)
{
  CAMLparam1(file_name);

  FILE* input_file = fopen(String_val(file_name), "r");

  if ( input_file == 0 ) {
    failwith( strerror(errno) );
  }

  return_pointer(input_file);
}

value open_output_file(value file_name)
{
  CAMLparam1(file_name);

  FILE* output_file = fopen(String_val(file_name), "w");

  if ( output_file == 0 ) {
    failwith( strerror(errno) );
  }

  return_pointer(output_file);
}

value input_int(value file)
{
  CAMLparam1(file);
  
  int ret_value = 0;
  if ( 1 != fread(&ret_value, sizeof(ret_value), 1, (FILE*)file) ) {
    if ( ferror((FILE*)file) ) {
      failwith( strerror(errno) );
    }
    else {
      raise_constant(*caml_named_value(EXCEPTION_FILE_EOF));
    }
  }

  return_value(Val_int(ret_value));
};

void output_int(value file, value int_val)
{
  int ret_value;

  CAMLparam2(file, int_val);
  
  ret_value = Int_val(int_val);

  if ( 1 != fwrite(&ret_value, sizeof(ret_value), 1, (FILE*)file) ) {
    if ( ferror((FILE*)file) ) {
      failwith( strerror(errno) );
    }
    else {
      raise_constant(*caml_named_value(EXCEPTION_FILE_EOF));
    }
  }

  return_void;
};

value input_float(value file)
{
  CAMLparam1(file);
  
  double ret_value = 0;
  if ( 1 != fread(&ret_value, sizeof(ret_value), 1, (FILE*)file) ) {
    if ( ferror((FILE*)file) ) {
      failwith( strerror(errno) );
    }
    else {
      raise_constant(*caml_named_value(EXCEPTION_FILE_EOF));
    }
  }

  return_value(copy_double(ret_value));
};

void output_float(value file, value float_val)
{
  CAMLparam2(file, float_val);
  
  double ret_value = Double_val(float_val);
  if ( 1 != fwrite(&ret_value, sizeof(ret_value), 1, (FILE*)file) ) {
    if ( ferror((FILE*)file) ) {
      failwith( strerror(errno) );
    }
    else {
      raise_constant(*caml_named_value(EXCEPTION_FILE_EOF));
    }
  }

  return_void;
};

void close_output_file(value file)
{
  CAMLparam1(file);

  if ( 0 != fclose((FILE*)file) ) {
    failwith( strerror(errno) );
  }

  return_void;
}

void close_input_file(value file)
{
  CAMLparam1(file);

  if ( 0 != fclose((FILE*)file) ) {
    failwith( strerror(errno) );
  }

  return_void;
}

value end_of_file(value file)
{
  CAMLparam1(file);
  
  int ret_value = feof((FILE*)file);
  return_value(ret_value ? Val_true : Val_false);
};

value tell_percent(value file)
{
  CAMLparam1(file);
  
  FILE *fp = (FILE*)file;
  struct stat stat_buf;
  long   curr_pos = ftell(fp);
  fstat(fileno(fp), &stat_buf); 

  return_value( ((double) curr_pos) / (double) stat_buf.st_size );
};



  
  
