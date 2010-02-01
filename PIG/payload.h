#ifndef PAYLOAD_H
#define PAYLOAD_H

#include "tuple.h"
#include "interval.h"
#include <assert.h>

struct PayloadStream {
  Payload payload;
  int     offset;
  int     length;
  int     overflow;

  Interval previous;
  Interval range;

  PayloadStream(Payload a_payload, int a_length) 
  {
    init(a_payload, a_length);
  };

  void init(Payload a_payload, int a_length)
  {
    payload = a_payload;
    length  = a_length;
    offset = overflow = 0;
    previous.clear();
    range.clear();
  };
  
  Byte pop() {
    assert(offset < length);
    return payload[offset++];
  }

  Byte peek() {
    return payload[offset];
  }

  void push(Byte c) {
    if ( offset >= length ) {
      overflow = 1;
    }
    else {
      payload[offset++] = c;
    }
  }

  void copy_to(Byte *ptr, int len) {
    assert( offset + len < length );
    memcpy(ptr, payload + offset, len);
    offset += len;
  }

  void copy_from(Byte *ptr, int len) {
    if ( offset + len >= length ) {
      overflow = 1;
    }
    else {
      memcpy(payload + offset, ptr, len);
      offset += len;
    }
  }
    
};

void payload_insert(
  PayloadStream& dest_st, 
  PayloadStream& src_st, 
  PayloadStream& values_st, 
  PayloadStream& overflow_st
);

void payload_insert(
  PayloadStream& dest, PayloadStream& src, 
  Tuple const& tuple, PayloadStream& overflow
);

void payload_remove(
  PayloadStream& dest, PayloadStream& src, 
  Tuple const& tuple, PayloadStream& overflow
);

void payload_write_tuple(PayloadStream& st, Tuple const& x);

void decode_interval(PayloadStream& st, Interval& interval);

// test interface

void test_encode_decode();
void test_interval_encode_decode();
void test_plus_minus();

#endif
