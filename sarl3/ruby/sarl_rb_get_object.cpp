#include "sarl_rb_get_object.h"

#include <ruby.h>

#include <gc/gc_allocator.h>

#include <ext/hash_map>
#include "sarl_types.h"
#include "sarl_exceptions.h"

using namespace __gnu_cxx;

namespace __gnu_cxx {
  template<> struct hash<void *>
  { size_t operator()(void* __x) const { return ((size_t)__x)>>0x2; } };
}

typedef hash_map<
  void *, VALUE, hash<void *>, std::equal_to<void *>, 
  traceable_allocator<std::pair<void *, VALUE> > 
> SarlRubyPtrMap;

SarlRubyPtrMap global_ptr_map;

void sarl_free_ruby_object(void *ptr)
{
  // fprintf(stderr, "Free Object: %p \n", ptr);
  global_ptr_map.erase(ptr);
};

VALUE sarl_get_ruby_object(VALUE klass, void *ptr)
{
  if ( ptr == 0 ) {
    sarl_null_pointer_error;
    return Qnil;
  }

  SarlRubyPtrMap::iterator it = global_ptr_map.find(ptr);
  if ( it != global_ptr_map.end() ) {
    // fprintf(stderr, "returning duplicate\n", klass, ptr);
    return it->second;
  }
  else {
    // fprintf(stderr, "class=%p, ptr=%p\n", klass, ptr);
    VALUE result = Data_Wrap_Struct(klass, 0, sarl_free_ruby_object, ptr);
    // fprintf(stderr, "result=%p\n", result);
    global_ptr_map.insert(SarlRubyPtrMap::value_type(ptr, result));
    return result;
  }
}

int sarl_exists_ruby_object(void *ptr)
{
  SarlRubyPtrMap::iterator it = global_ptr_map.find(ptr);
  return it != global_ptr_map.end();
};


void sarl_set_ruby_object(VALUE obj, void *ptr)
{
  SarlRubyPtrMap::iterator it = global_ptr_map.find(ptr);
  if ( it != global_ptr_map.end() ) {
    rb_raise(Qnil, "Error, ptr is already allocated to another object");
  }
  else {
    // fprintf(stderr, "class=%p, ptr=%p\n", klass, ptr);
    if ( DATA_PTR(obj) != 0 ) {
      sarl_free_ruby_object(DATA_PTR(obj));
    }
    DATA_PTR(obj) = ptr;
    global_ptr_map.insert(SarlRubyPtrMap::value_type(ptr, obj));
  }
}


