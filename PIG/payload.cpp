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

/* The Payload stores a sequence of Tuples which use both interval and
   prefix compression.

   A Tuple is encoded using from 1 to 16=4*4 characters. The tuple
   is stored as the difference from the last tuple. This involves doing 
   arithmatic on potentially large numbers (4 times the word size).

   A terminator is used for payload which are not complete. The terminator is
   a zero. Since two lengths can fit in a character the lengths are stored
   first.

        |len|len|code|code|

   An interval node is an offset in which all entries in between are members.

   Insert requires decoding to the insertion point and then encoding after
   the insertion point.

   Insertion will always either use less space or an equal amount of space.
   Insertion at the end of an interval may increase the amount of space.

   Insertion is only into one payload so doing a complete copy is not
   too bad.

   A termination condition is either that the payload has come to an
   end or that we have run out of space. Can more than one value get
   pushed off the end?

   An insertion of a large interval earlier could displace several
   small intervals on the end. Thus the function should insert a
   sequence tuples.

   Storing the length is quite a hassle. Another mechanism is to
   store the length in the low 4 bytes. That means shifting the remaining 
   bytes which is also a hassle.

   |len|code|

   for(x=1;x<len;x++) {
     code[x] = (data[x] >> 4) | (data[x+1] << 4);
   }

   len = 0 -> total length = 1 byte
   len = 1 -> total length = 2 bytes

   A zero byte indicates an end of sequence

   Need to detect an overflow when encoding. There must always be room
   for a termination which is two zero bytes.

   We have a problem that the encoding uses the top four bytes, thus
   with a max value of 15 it signifies that 15 bytes are used in
   addition to the first byte. The maximum offset uses 4 * 4 bytes, for
   a total of 16 bytes plus an extra byte for the length.

   Thus we need to be able to represent a maximum of 17 bytes. Either
   we can use an extra byte which allows for interval values to be
   encoded for a certain range, thus if i > 17 then i-17 equals the number
   of bytes in the interval.

   Currently there has been no accounting for intervals, but there should
   be an accounting for intervals.

   In the latter part of the insertion algorithm we should accumulate intervals
   since a sequence of src and value types may combine to form an interval set.

   Currently prefix compression is not occuring between the end value from the
   previous interval and start value of the subsequent one.
*/

#include "payload.h"
#include "tuple.h"

#include <assert.h>
#include <iostream>

using namespace std;
  
inline void decode_tuple(PayloadStream& src_st, Tuple& tuple)
{
  if ( src_st.peek() & 0x2 ) { // test for multibyte byte
    src_st.copy_to((Byte *)tuple, (src_st.pop() >> 2)+1);
  }
  else { // otherwise single byte
    tuple[0] = src_st.pop() >> 2;
  }
}

void decode_interval(PayloadStream& src_st, Interval& interval)
{
  // on exact overflow return a zero
  interval.clear();
  
  if ( src_st.offset >= src_st.length ) {
    return;
  }

  // |i| i=0 not interval, i=1 is interval
  // |s| s=0 single byte value, s=1 multibyte value
  // multibyte value: len = first_byte >> 2

  Tuple value = { 0, 0, 0, 0 };

  // remember the first byte
  Byte first_byte = src_st.peek();

  // decode the start entry
  decode_tuple(src_st, value);
  assign_eq(interval.start,interval.end);
  plus_equals_modulo(interval.start, value);
  
  // test for an interval entry
  if ( first_byte & 0x1 ) {
    clear(value);
    decode_tuple(src_st, value);
    assign_eq(interval.end,interval.start);
    plus_equals_modulo(interval.end, value);
  }
  else {
    assign_eq(interval.end,interval.start);
  }

  // account for prefix coding
  if ( ! is_null(interval.start) ) {
    plus_equals_modulo(interval.end, src_st.previous.end);
    plus_equals_modulo(interval.start, src_st.previous.end);
    assign_eq(src_st.previous.start, interval.start);
    assign_eq(src_st.previous.end, interval.end);
  }

  if ( is_null(src_st.range.start) ) {
    assign_eq(src_st.range.start, interval.start);
  }
  assign_eq(src_st.range.end, interval.end);
};

void encode_tuple(PayloadStream& src_st, Tuple& tuple, int is_interval)
{
  Byte *ptr = ((Byte *)&tuple);
  int i;
  for(i=15;i>0 && ptr[i]==0; i--) ;
  
  // test for single byte value
  if ( i == 0 && (ptr[i] & 0xc) == 0 ) {
    src_st.push( 
      (0x1 & is_interval) |
      ptr[i] << 2
    );
  }
  else {
    src_st.push(
      (0x1 & is_interval) |
      (0x2) |
      (i << 2)
    );
    src_st.copy_from(ptr, i+1);
  }
};

void encode_interval(PayloadStream& dest_st, Interval& interval)
{
  unsigned int offset = dest_st.offset;
  Tuple end_value;
  
  if ( cmp(interval.start,interval.end) != 0 ) {
    encode_tuple(dest_st, interval.start, 0x1);
    if ( dest_st.overflow ) {
      dest_st.offset = offset;
      dest_st.push(0); // termination
      return;
    }
    assign_eq(end_value, interval.end);
    minus_equals_modulo(end_value, interval.start);
  }
  else {
    assign_eq(end_value, interval.end);
  }

  encode_tuple(dest_st, end_value, 0);
  if ( dest_st.overflow ) {
    dest_st.offset = offset;
    dest_st.push(0); // null terminator
  }
};

void encode_interval(
  PayloadStream& dest_st, 
  Interval interval,
  PayloadStream& overflow_st)
{
  Interval comp_interval; 

  // once an overflow has occured it should continue to occur
  // compensation for prefix encoding is performed
  // the stream field named previous is updated

  if ( dest_st.overflow ) {
    assign_eq(comp_interval, interval);
    if ( ! is_null(comp_interval.start) ) {
      minus_equals_modulo(comp_interval.start, overflow_st.previous.end);
      minus_equals_modulo(comp_interval.end, overflow_st.previous.end);
    };
    encode_interval(overflow_st, comp_interval);
    assign_eq(overflow_st.previous.start, interval.start);
    assign_eq(overflow_st.previous.end, interval.end);
    assign_eq(overflow_st.range.end, interval.end);
  }
  else {
    assign_eq(comp_interval, interval);
    if ( ! is_null(comp_interval.start) ) {
      minus_equals_modulo(comp_interval.start, dest_st.previous.end);
      minus_equals_modulo(comp_interval.end, dest_st.previous.end);
    };
    if ( dest_st.offset == 0 ) {
      assign_eq(dest_st.range.start, interval.start);
    };
    encode_interval(dest_st, comp_interval);
    if ( dest_st.overflow ) {
      assign_eq(comp_interval, interval);
      if ( ! is_null(comp_interval.start) ) {
	minus_equals_modulo(comp_interval.start, overflow_st.previous.end);
	minus_equals_modulo(comp_interval.end, overflow_st.previous.end);
      };
      encode_interval(overflow_st, comp_interval);
      assign_eq(overflow_st.previous.start, interval.start);
      assign_eq(overflow_st.previous.end, interval.end);
      assign_eq(overflow_st.range.start, interval.start);
      assign_eq(overflow_st.range.end, interval.end);
    }
    else {
      assign_eq(dest_st.previous.start, interval.start);
      assign_eq(dest_st.previous.end, interval.end);
      assign_eq(dest_st.range.end, interval.end);
    }
  }

};

int interval_combine(Interval const& x, Interval const& y, Interval& result)
{
  // make sure x is first
  if ( cmp(x.start, y.start) <= 0 ) {
    Tuple x_end_plus_one = { 0, 0, 0, 1 };
    plus_equals_modulo(x_end_plus_one, x.end);
    if ( cmp(y.start, x_end_plus_one) <= 0 ) {
      assign_eq(result.start,x.start);
      if ( cmp(y.end, x.end) >= 0 ) {
        assign_eq(result.end,y.end);
      }
      else {
        assign_eq(result.end,x.end);
      }
      return 1;
    }
    else {
      result.clear();
      return 0;
    }
  }
  else {
    return interval_combine(y, x, result);
  }
};
  
void payload_insert(
  PayloadStream& dest_st, 
  PayloadStream& src_st, 
  PayloadStream& values_st, 
  PayloadStream& overflow_st)
{
  Interval src_interval, value_interval;

  decode_interval(src_st, src_interval);
  decode_interval(values_st, value_interval);

  while ( !is_null(src_interval.start) || !is_null(value_interval.start) ) { 

    if ( is_null(src_interval.start) ) {
      while( !is_null(value_interval.start) ) {
        encode_interval(dest_st, value_interval, overflow_st);
        decode_interval(values_st, value_interval);
      }
    }
    else if ( is_null(value_interval.start) ) {
      while( !is_null(src_interval.start) ) {
        encode_interval(dest_st, src_interval, overflow_st);
        decode_interval(src_st, src_interval);
      }
    }
    else {
      Interval comb, comb2;
      if ( interval_combine(src_interval, value_interval, comb) ) {
        decode_interval(src_st, src_interval);
	if ( interval_combine(src_interval, comb, comb2) ) {
	    encode_interval(dest_st, comb2, overflow_st);
	    decode_interval(src_st, src_interval);
	    decode_interval(values_st, value_interval);
	}
	else {
	    encode_interval(dest_st, comb, overflow_st);
	    decode_interval(values_st, value_interval);
	}
      }
      else {
        if ( cmp(src_interval.start, value_interval.start) <= 0 ) {
          encode_interval(dest_st, src_interval, overflow_st);
          decode_interval(src_st, src_interval);
        }
        else {
          encode_interval(dest_st, value_interval, overflow_st);
          decode_interval(values_st, value_interval);
        };
      }
    }
  };

  if ( dest_st.offset < dest_st.length ) {
    dest_st.push(0); // null terminate
  };
};

void payload_remove(
  PayloadStream& dest_st, 
  PayloadStream& src_st, 
  PayloadStream& values_st,
  PayloadStream& overflow_st)
{
  // find any intersection between src_st and values_st
  Interval src, value;
  Tuple static one = { 0, 0, 0, 1 };

  decode_interval(src_st, src);
  decode_interval(values_st, value);
  
  while( !is_null(src.start) ) {
    
    if ( is_null(value.start) || cmp(src.end, value.start) < 0 ) {
      encode_interval(dest_st, src, overflow_st);
      decode_interval(src_st, src);
    }
    else if ( cmp(value.end, src.start) < 0 ) {
      decode_interval(values_st, value);
    }
    else if ( cmp(src.start, value.start) <= 0 ) {
      Interval interval;
      assign_eq(interval.start,src.start);
      assign_eq(interval.end,value.start);
      minus_equals(interval.end, one);
      if ( cmp(interval.end, interval.start) >= 0 ) {
        encode_interval(dest_st, interval, overflow_st);
      }
      if ( cmp(src.end, value.end) > 0 ) {
        assign_eq(interval.start,value.end);
        assign_eq(interval.end, src.end);
        plus_equals(interval.start, one);
        if ( cmp(interval.end, interval.start) >= 0 ) {
          encode_interval(dest_st, interval, overflow_st);
        };
      };
      decode_interval(src_st, src);
    }
    else {
      assert( cmp(value.start, src.start) <= 0 );
      if ( cmp(src.end, value.end) > 0 ) {
        Interval interval;
        assign_eq(interval.start, value.end);
        plus_equals(interval.start, one);
        assign_eq(interval.end, src.end);
        encode_interval(dest_st, interval, overflow_st);
      }
      decode_interval(src_st, src);
    }
  }

  if ( dest_st.offset < dest_st.length ) {
    dest_st.push(0); // null terminate
  };
}

void payload_write_tuple(PayloadStream& st, Tuple const& x)
{
  Interval interval;
  assign_eq(interval.start, x);
  assign_eq(interval.end, x);
  encode_interval(st, interval);
  st.push(0);
};



void payload_insert(
  PayloadStream& dest, 
  PayloadStream& src, 
  Tuple const& tuple, 
  PayloadStream& overflow)
{
  Byte values[64];
  PayloadStream values_st(values, sizeof(values));
  payload_write_tuple(values_st, tuple);
  values_st.offset = 0; // reset
  payload_insert(dest, src, values_st, overflow);
};

void payload_remove(
  PayloadStream& dest, PayloadStream& src, Tuple const& tuple, PayloadStream& overflow)
{
  Byte values[64];
  Interval value_interval;
  assign_eq(value_interval.start, tuple);
  assign_eq(value_interval.end, tuple);
  PayloadStream values_st(values, sizeof(values));
  encode_interval(values_st, value_interval);
  values_st.push(0); // null terminate
  values_st.offset = 0; // reset
  payload_remove(dest, src, values_st, overflow);
};

int pop_interval(Payload payload, int length, Interval& interval)
{
  PayloadStream s(payload, length);
  decode_interval(s, interval);
  return s.offset;
};

void test_plus_minus()
{
  Tuple x = { 0, 0, 0, 0 };
  Tuple y = { 1, 1, 1, 5 };
  Tuple z = { 1, 1, 2, 1 };

  cout << "x: "; write_tuple(cout, x); cout << ", ";
  cout << "y: "; write_tuple(cout, y); cout << ", ";
  cout << "z: "; write_tuple(cout, z); cout << ", ";

  minus_equals_modulo(z, y);
  cout << "z -= y: "; write_tuple(cout, z); cout << endl;

  minus_equals_modulo(z, x);
  cout << "z -= x: "; write_tuple(cout, z); cout << endl;

  minus_equals_modulo(y, x);
  cout << "y -= x: "; write_tuple(cout, y); cout << endl;

  plus_equals_modulo(z, x);
  cout << "z + x: "; write_tuple(cout, z); cout << endl;

  plus_equals_modulo(z, y);
  cout << "z + y: "; write_tuple(cout, z); cout << endl;
};



void test_encode_decode()
{
  Byte payload[1024];
  PayloadStream writer(payload, sizeof(payload));
  PayloadStream reader(payload, sizeof(payload));
  
  Tuple write_value = { 0, 0, 1, 0 };
  Tuple read_value = { 0, 0, 0, 0 };
  
  encode_tuple(writer, write_value, 0);
  decode_tuple(reader, read_value);
  
  cout << "write_value: "; write_tuple(cout, write_value); cout << ", ";
  cout << "read_value: "; write_tuple(cout, read_value); cout << endl;
};

void test_interval_encode_decode()
{
  Byte payload[1024];
  PayloadStream writer(payload, sizeof(payload));
  PayloadStream reader(payload, sizeof(payload));
  
  Interval read_value = 
    {
      { 0, 0, 0, 0 }, { 0, 0, 0, 0 } 
    };
  
  Interval write_value = 
    {
      { 1, 1, 1, 1 }, { 1, 1, 2, 1 } 
    };
  
  encode_interval(writer, write_value);
  decode_interval(reader, read_value);
  
  cout << "write_value: "; write_interval(cout, write_value); cout << ", ";
  cout << "read_value: "; write_interval(cout, read_value); cout << endl;
};



  
