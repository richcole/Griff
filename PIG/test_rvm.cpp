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
#include <rvm.h>

int main()
{
  // init
  rvm_options_t options;
  rvm_return_t  ret_value;
  
  options.log_dev = "test.log";
  options.truncate = 0;
  options.recovery_buf_len = 0;
  options.flush_buf_len = 0;
  options.max_read_len = 0;
  options.flags = 0;

  ret_value = RVM_INIT(options);

  if ( ret_value != RVM_SUCCESS ) {
    fprintf(stderr, "Error loading intialization options.\n");
  };
  
    
  
  
};
