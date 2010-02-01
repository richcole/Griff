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
#include "payload.h"
#include "tuple.h"
#include <iostream>

using namespace std;

void report_payload(std::string const& s, Payload p, int pl)
{
  PayloadStream st(p, pl);

  Interval interval;

  cout << s << ":" << endl;
  for(
    decode_interval(st, interval);
    ! is_null(interval.start);
    decode_interval(st, interval)
  ) {
    cout << "  [";
    write_tuple(cout, interval.start);
    cout << "-";
    write_tuple(cout, interval.end);
    cout << "]" << endl;
  }
};

int main()
{
  unsigned char payload_1[128];
  unsigned char payload_2[128];
  unsigned char payload_3[128];

  unsigned char *src;
  unsigned char *dest;
  unsigned char *overflow;
  unsigned char *temp;

  Tuple insert_tuples[] = 
    {
      { 1, 1, 1, 1 },
      { 1, 1, 1, 2 },
      { 1, 1, 1, 3 },
      { 1, 1, 1, 5 },
      { 1, 1, 1, 6 },
      { 1, 1, 2, 1 },
      { 1, 1, 2, 3 },
      { 1, 1, 1, 4 },
      { 1, 1, 1, 1 },
      { 1, 1, 1, 6 },
      { 1, 1, 2, 1 },
      { 1, 1, 1, 8 }
    };

  Tuple remove_tuples[] = 
    {
      { 1, 1, 1, 4 },
      { 1, 1, 2, 1 }
    };

  int remove_length = sizeof(remove_tuples)/sizeof(*remove_tuples);
  int insert_length = sizeof(insert_tuples)/sizeof(*insert_tuples);
  int i;

  // run a couple of private tests
  test_encode_decode();
  test_interval_encode_decode();
  test_plus_minus();

  memset(payload_1, 0, sizeof(payload_1));
  memset(payload_3, 0, sizeof(payload_3));

  src = payload_1;
  dest = payload_2;
  overflow = payload_3;
  
  for(i=0;i<insert_length;++i) {
    PayloadStream dest_st(dest, sizeof(payload_1));
    PayloadStream src_st(src, sizeof(payload_1));
    PayloadStream overflow_st(overflow, sizeof(payload_3));
    cout << "Insert: "; write_tuple(cout, insert_tuples[i]); cout << endl;
    payload_insert(dest_st, src_st, insert_tuples[i], overflow_st);
    report_payload("dest", dest, sizeof(payload_1));
    report_payload("overflow", overflow, sizeof(payload_3));
    temp = dest;
    dest = src;
    src = temp;
  };

  for(i=0;i<remove_length;++i) {
    PayloadStream dest_st(dest, sizeof(payload_1));
    PayloadStream src_st(src, sizeof(payload_1));
    PayloadStream overflow_st(overflow, sizeof(payload_3));
    cout << "Remove: "; write_tuple(cout, remove_tuples[i]); cout << endl;
    payload_remove(dest_st, src_st, remove_tuples[i], overflow_st);
    report_payload("dest", dest, sizeof(payload_1));
    report_payload("overflow", overflow, sizeof(payload_3));
    temp = dest;
    dest = src;
    src = temp;
  };
};

