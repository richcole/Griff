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
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
  int fd = open("test.chunk", O_RDWR, 0666);
  char c = 0;
  void *p = 0;
  void *q = 0;

  lseek(fd, 1024 * 8 - 1, SEEK_SET);
  write(fd, &c, 1);

  p = mmap(0, 1024 * 8, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  if ( p == 0 ) {
    fprintf(stderr, "Error, first mmap failed.\n");
    exit(-1);
  };
  
  lseek(fd, 1024 * 16 - 1, SEEK_SET);
  write(fd, &c, 1);
  
  q = mremap(p, 1024 * 8, 1024 * 16, 0);

  if ( p == 0 ) {
    fprintf(stderr, "Error, second mmap failed.\n");
    exit(-1);
  };

  if ( p != q ) {
    fprintf(stderr, "Error, remapping moved the data.");
    exit(-1);
  };

  return 0;
};

    
  
