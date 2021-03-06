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
#include "tuple.h"
#include <assert.h>

int main()
{
  Tuple t[4] = 
    {
      { 4, 8,  0, 1 },
      { 5, 1,  0, 8 },
      { 5, 1,  3, 1 },
      { 7, 11, 0, 1 }
    };
  int i;
  
  for(i=0;i<3;++i) {
    assert(cmp(t[i],t[i+1]) < 0);
    assert(cmp(t[i],t[i]) == 0);
    assert(cmp(t[i+1],t[i]) > 0);
  };
  assert(cmp(t[3],t[3])==0);
};


  
  
