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

int main()
{
  double interest_rate = 6.30 / 100.0;
  double fir = interest_rate / 26.0;
  
  double principle = 283200;
  double repayment[] = { 1500.0, 500.0 };
  int    repayment_c = 2;
  double period[] = { 3 * 26 , 4 * 26, 6 * 26, 7 * 26, 50 * 26 };
  int    period_c    = 5;
      
  double interest  = 0;
  int    repayment_index = 0;
  int    period_index = 0;
  
  int count = 0;
  while( principle > 0 ) {
    if ( count == period[period_index] ) {
      repayment_index = (repayment_index + 1) % repayment_c;
      period_index = (period_index + 1) % period_c;
    }
    interest  = principle * fir;
    principle = principle + interest - repayment[repayment_index];
    printf("%d\t%d\t%5.0f\t%5.0f\n", 
      (count / 26)+1, (count % 26)+1, principle, interest);
    count++;
  };
};

  
  
