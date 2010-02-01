#include <stdlib.h>
#include <ext/hash_map>

typedef size_t rj_ref_t;

using namespace __gnu_cxx;

struct rj_object_t {
  rj_ref_t class_index;
  rj_ref_t fields[1]; 
};

typedef hash_map<rj_ref_t, rj_object_t *> method_table_t;

struct rj_class_t {
  method_table_t methods;
};

typedef hash_map<string, rj_ref_t> string_table_t;
typedef hash_map<rj_ref_t, rj_class_t *> class_map_t;

string_table_t string_table;
class_map_t class_map;

/* Whether or not to operate a stack for temporaries. The stack could
   contain self, arg1, arg2, arg3 ... . A problem with the stack is
   that it is linear, but copies only need to be made when a reference
   to a block is constructed.
*/

inline rj_object_t* rj_ptr(rj_ref_t object) {
  return (rj_object_t *)object;
}

inline rj_class_t* rj_class_ptr(rj_ref_t clazz) {
  return (rj_class_t *)clazz;
}

inline rj_ref_t rj_ref(rj_object_t* ptr) {
  return (rj_ref_t)ptr;
}

inline rj_ref_t rj_get_index(string const& name) {
  string_table_t::const_iterator it = string_table.find(name);
  if ( it != string_table.end() ) {
    return it->second;
  }
  else {
    rj_ref_t index = string_table.size();
    string_table.insert(string_table_t::value_type(name, index));
    return index;
  }
}

inline rj_class_t* rj_class_get(rj_ref_t class_index) {
  class_table_t::const_iterator it = class_table.find(class_index);
  if ( it != class_table.end() ) {
    it->second;
  }
  else {
    rj_class_t *clazz = new rj_class_t();
    class_table.insert(class_table_t::value_type(class_index, clazz);
    return clazz;
  }
}

inline rj_class_t* rj_class_get_by_name(string const& class_name) {
  rj_ref_t class_ref = rj_get_index(name);
  return rj_class_get(class_ref);
}

inline rj_ref_t rj_class_get_or_create_method(rj_ref_t clazz, rj_ref_t method) {
  method_table_t::const_iterator it = 
    rj_class_ptr(clazz)->methods.find(method);
  if ( it != rj_class_ptr(clazz)->methods.end() ) {
    return rj_ref(it->second);
  }
  else {
    rj_object_t *method_object = rj_object_create();
    // FIXME: initialize the method object here
  }
}

inline rj_ref_t rj_class_get_method(rj_ref_t clazz, rj_ref_t method)
{
  method_table_t::const_iterator it = 
    rj_class_ptr(clazz)->methods.find(method);
  if ( it != rj_class_ptr(clazz)->methods.end() ) {
    return rj_ref(it->second);
  }
  else {
    return 0;
  }
};

inline rj_ref_t rj_object_get_method(rj_ref_t object, rj_ref_t method) 
{
  return rj_class_get_method(  rj_ptr(object)->class_index, method);
};

inline rj_ref_t* rj_method_get_pc(rj_ref_t method) {
  return (rj_ref_t *)(rj_ptr(method)->fields[1]);
}

inline rj_ref_t rj_method_activation_size(rj_ref_t method)
{
  return rj_ptr(method)->fields[0];
}

inline rj_ref_t rj_stack_top(rj_ref_t *stack) {
  return stack[0];
}

inline void rj_stack_push(rj_ref_t *& stack, rj_ref_t item) {
  *(--stack) = item;
}

inline rj_ref_t rj_stack_pop(rj_ref_t *& stack) {
  return *(stack++);
}

inline rj_ref_t rj_stack_allocate(rj_ref_t *& stack, rj_ref_t count) {
  stack -= count;
}

inline rj_ref_t rj_stack_free(rj_ref_t *& stack, rj_ref_t count) {
  stack += count;
}

inline rj_ref_t rj_stack_get_field(rj_ref_t* stack, rj_ref_t index)
{
  return stack[index+2];
};

inline void rj_object_assign_field(
  rj_ref_t object, rj_ref_t index, rj_ref_t value)
{
  rj_ptr(object)->fields[index] = value;
}

const rj_ref_t RJ_STACK_SIZE = 1024 * 1024;

const rj_ref_t MAJOR_OP_SHIFT         = 28;
const rj_ref_t MAJOR_OP_MASK          = 0xf << MAJOR_OP_SHIFT;
const rj_ref_t MINOR_OP_MASK          = ~(MAJOR_OP_MASK);
const rj_ref_t INST_METHOD_CALL_TEMP  = 0x1;
const rj_ref_t INST_METHOD_CALL_FIELD = 0x2;
const rj_ref_t INST_ASSIGN_LOCAL      = 0x3;
const rj_ref_t INST_ASSIGN_FIELD      = 0x4;
const rj_ref_t INST_RETURN            = 0x5;

const rj_ref_t METHOD_NOT_FOUND_EXCEPTION = 0x1;

void rj_execute(rj_ref_t *pc)
{
  
  rj_ref_t stack_buffer[RJ_STACK_SIZE];
  rj_ref_t *stack = stack_buffer + (RJ_STACK_SIZE-1);
  rj_ref_t num_args, object, method, activation_size;
  rj_ref_t* next_pc = 0;
  rj_ref_t flags = 0, finished = 0;

  while ( finished == 0 ) {

    switch ( (pc[0] & MAJOR_OP_MASK) >> MAJOR_OP_SHIFT ) {
    case INST_METHOD_CALL_TEMP:
      /* the caller is a temporary */
      num_args = pc[0] & MINOR_OP_MASK;
      method   = rj_object_get_method(rj_stack_top(stack), pc[1]);
      next_pc  = pc + 2;
      if ( method != 0 ) {
        activation_size = rj_method_activation_size(method);
        rj_stack_allocate(stack, activation_size);
        rj_stack_push(stack, (rj_ref_t)next_pc);
        rj_stack_push(stack, activation_size);
        pc = rj_method_get_pc(method);
      }
      else {
        flags &= METHOD_NOT_FOUND_EXCEPTION;
      }
      break;
    case INST_METHOD_CALL_FIELD:
      /* the caller is a field */
      num_args = pc[0] & MINOR_OP_MASK;
      object = rj_stack_get_field(stack, pc[2]);
      method = rj_object_get_method(object, pc[1]);
      next_pc  = pc + 2;
      if ( method != 0 ) {
        activation_size = rj_method_activation_size(method); 
        rj_stack_allocate(stack, activation_size);
        rj_stack_push(stack, (rj_ref_t)next_pc);
        rj_stack_push(stack, activation_size);
        pc = rj_method_get_pc(method);
      }
      else {
        flags &= METHOD_NOT_FOUND_EXCEPTION;
      }
      break;
    case INST_ASSIGN_LOCAL:
      stack[pc[0] & MINOR_OP_MASK] = rj_stack_pop(stack);
      break;
    case INST_ASSIGN_FIELD:
      object = rj_stack_pop(stack);
      rj_object_assign_field(object, pc[0] & MINOR_OP_MASK, rj_stack_top(stack));
      break;
    case INST_RETURN:
      activation_size = rj_stack_pop(stack);
      next_pc = (rj_ref_t*)rj_stack_pop(stack);
      rj_stack_free(stack, activation_size);
      pc = next_pc;
      break;
    }
  }
}

    
  




  
