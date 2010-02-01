#include "sarl_rb.h"
#include <ruby.h>

#include "sarl_bit_set.h"
#include "sarl_diagram.h"
#include "sarl_context.h"
#include "sarl_lattice.h"
#include "sarl_string_table.h"
#include "sarl_layout.h"

#include "sarl_int_array.h"
#include "sarl_clarified_context.h"

#include "sarl_diagram_examples.h"
#include "sarl_context_examples.h"

#include "sarl_sat.h"
#include "sarl_point_set.h"

#include "sarl_rb_get_object.h"

VALUE rb_mSarl;                 /* module Sarl */
VALUE rb_cSarlBitSet;           /* class SarlBitSet */
VALUE rb_cSarlDiagram;          /* class SarlDiagram */
VALUE rb_cSarlContext;          /* class SarlDiagram */
VALUE rb_cSarlLattice;          /* class SarlLattice */
VALUE rb_cSarlStringTable;      /* class SarlStringTable */
VALUE rb_cSarlPowerSet;         /* class SarlStringTable */
VALUE rb_cSarlPowerMap;         /* class SarlStringTable */
VALUE rb_cSarlClarifiedContext; /* class SarlClarifiedContext */
VALUE rb_cSarlIntArray;         /* class SarlIntArray */
VALUE rb_cSarlClause;
VALUE rb_cSarlSatSolver;
VALUE rb_cSarlPoint;

#define sarl_must_be(obj, type) \
  if ( ! rb_obj_is_kind_of(obj, type) ) { \
    rb_raise (rb_eTypeError, "expected " #type "."); \
  };

// ---------------------------------------------------------------------
// SarlBitSet Wrapper
// ---------------------------------------------------------------------

static VALUE sarl_bit_set_dump_rb(VALUE a_value)
{
  sarl_bit_set_t *a_set;
  Data_Get_Struct(a_value, sarl_bit_set_t, a_set);
  sarl_unsigned_int i;
  fprintf(stderr, "dump: length=%u, bits=", a_set->length);
  for(i=0;i<a_set->length;++i) {
    fprintf(stderr, "%x", a_set->bits[i]);
    if ( i != a_set->length-1 ) fprintf(stderr, ", ");
  }
  fprintf(stderr, ", count=%u\n", sarl_bit_set_count(a_set));
  return Qnil;
};

static VALUE sarl_bit_set_rb_alloc(VALUE klass)
{
  sarl_bit_set_t* bset = sarl_bit_set_default_new();
  //  return Data_Wrap_Struct(klass, 0, sarl_free_ruby_object, bset);
  return sarl_get_ruby_object(klass, bset);
};

static VALUE sarl_bit_set_initialize_copy_rb(VALUE a_value, VALUE b_value)
{
  sarl_bit_set_t *a_set, *b_set;
  Data_Get_Struct(a_value, sarl_bit_set_t, a_set);
  Data_Get_Struct(b_value, sarl_bit_set_t, b_set);

  if ( a_set == 0 ) { 
    a_set = sarl_bit_set_default_new(); 
    sarl_set_ruby_object(a_value, a_set);
  }

  if ( b_set == 0 ) { 
    b_set = sarl_bit_set_default_new(); 
    sarl_set_ruby_object(b_value, b_set);
  }

  sarl_bit_set_assign_equals(a_set, b_set);
  return a_value;
};

static VALUE sarl_bit_set_set_rb(VALUE bset_value, VALUE index)
{
  sarl_bit_set_t *bset;
  Data_Get_Struct(bset_value, sarl_bit_set_t, bset);

  if ( bset == 0 ) { 
    bset = sarl_bit_set_default_new(); 
    sarl_set_ruby_object(bset_value, bset);
  }

  sarl_bit_set_set(bset, NUM2INT(index));
  return Qnil;
};

static VALUE sarl_bit_set_clear_rb(VALUE bset_value, VALUE index)
{
  sarl_bit_set_t *bset;
  Data_Get_Struct(bset_value, sarl_bit_set_t, bset);

  if ( bset != 0 ) {
    sarl_bit_set_clear(bset, NUM2INT(index));
  }

  return Qnil;
};

static VALUE sarl_bit_set_is_set_rb(VALUE bset_value, VALUE index)
{
  sarl_bit_set_t *bset;
  Data_Get_Struct(bset_value, sarl_bit_set_t, bset);

  sarl_unsigned_int result = 
    bset && sarl_bit_set_is_set(bset, NUM2INT(index));

  return result ? Qtrue : Qfalse; 
};

static VALUE sarl_bit_set_hash_rb(VALUE bset_value)
{
  sarl_bit_set_t *bset;
  Data_Get_Struct(bset_value, sarl_bit_set_t, bset);
  sarl_unsigned_int result = bset ? sarl_bit_set_hash(bset) : 0;
  return INT2FIX(result);
};

static VALUE sarl_bit_set_is_subset_rb(VALUE a_value, VALUE b_value)
{
  sarl_bit_set_t *a_set, *b_set;
  Data_Get_Struct(a_value, sarl_bit_set_t, a_set);
  Data_Get_Struct(b_value, sarl_bit_set_t, b_set);
  sarl_unsigned_int result = 
    (a_set && b_set && sarl_bit_set_is_subset(a_set, b_set))
    || 
    (a_set == 0 && sarl_bit_set_count(b_set) > 1);
  return result;
};

static VALUE sarl_bit_set_is_subseteq_rb(VALUE a_value, VALUE b_value)
{
  sarl_bit_set_t *a_set, *b_set;
  Data_Get_Struct(a_value, sarl_bit_set_t, a_set);
  Data_Get_Struct(b_value, sarl_bit_set_t, b_set);
  sarl_unsigned_int result = 
    (a_set && 
      (
        (b_set && sarl_bit_set_is_subseteq(a_set, b_set)) ||
        (sarl_bit_set_count(a_set) == 0)
      )
    ) || 
    (a_set == 0);
  return result; 
};

static VALUE sarl_bit_set_and_rb(VALUE a_value, VALUE b_value)
{
  sarl_bit_set_t *a_set, *b_set, *result;
  Data_Get_Struct(a_value, sarl_bit_set_t, a_set);
  Data_Get_Struct(b_value, sarl_bit_set_t, b_set);

  if ( a_set == 0 ) { 
    a_set = sarl_bit_set_default_new(); 
    sarl_set_ruby_object(a_value, a_set);
  }
  if ( b_set == 0 ) { 
    b_set = sarl_bit_set_default_new(); 
    sarl_set_ruby_object(b_value, b_set);
  }

  result = sarl_bit_set_and(a_set, b_set);
  return sarl_get_ruby_object(rb_cSarlBitSet, result); 
};

static VALUE sarl_bit_set_or_rb(VALUE a_value, VALUE b_value)
{
  sarl_bit_set_t *a_set, *b_set, *result;
  Data_Get_Struct(a_value, sarl_bit_set_t, a_set);
  Data_Get_Struct(b_value, sarl_bit_set_t, b_set);

  if ( a_set == 0 ) { 
    a_set = sarl_bit_set_default_new(); 
    sarl_set_ruby_object(a_value, a_set);
  }
  if ( b_set == 0 ) { 
    b_set = sarl_bit_set_default_new(); 
    sarl_set_ruby_object(b_value, b_set);
  }

  result = sarl_bit_set_or(a_set, b_set);
  VALUE result_value = sarl_get_ruby_object(rb_cSarlBitSet, result);
  return result_value;
};

static VALUE sarl_bit_set_and_equals_rb(VALUE a_value, VALUE b_value)
{
  sarl_bit_set_t *a_set, *b_set;
  Data_Get_Struct(a_value, sarl_bit_set_t, a_set);
  Data_Get_Struct(b_value, sarl_bit_set_t, b_set);

  if ( a_set == 0 ) { 
    a_set = sarl_bit_set_default_new(); 
    sarl_set_ruby_object(a_value, a_set);
  }
  if ( b_set == 0 ) { 
    b_set = sarl_bit_set_default_new(); 
    sarl_set_ruby_object(b_value, b_set);
  }

  sarl_bit_set_and_equals(a_set, b_set);
  return Qnil;
};

static VALUE sarl_bit_set_or_equals_rb(VALUE a_value, VALUE b_value)
{
  sarl_bit_set_t *a_set, *b_set;
  Data_Get_Struct(a_value, sarl_bit_set_t, a_set);
  Data_Get_Struct(b_value, sarl_bit_set_t, b_set);

  if ( a_set == 0 ) { 
    a_set = sarl_bit_set_default_new(); 
    sarl_set_ruby_object(a_value, a_set);
  }
  if ( b_set == 0 ) { 
    b_set = sarl_bit_set_default_new(); 
    sarl_set_ruby_object(b_value, b_set);
  }

  sarl_bit_set_or_equals(a_set, b_set);

  return Qnil;
};

static VALUE sarl_bit_set_assign_equals_rb(VALUE a_value, VALUE b_value)
{
  sarl_bit_set_t *a_set, *b_set;
  Data_Get_Struct(a_value, sarl_bit_set_t, a_set);
  Data_Get_Struct(b_value, sarl_bit_set_t, b_set);

  if ( a_set == 0 ) { 
    a_set = sarl_bit_set_default_new(); 
    sarl_set_ruby_object(a_value, a_set);
  }
  if ( b_set == 0 ) { 
    b_set = sarl_bit_set_default_new(); 
    sarl_set_ruby_object(b_value, b_set);
  }

  sarl_bit_set_assign_equals(a_set, b_set);
  return Qnil;
};

static VALUE sarl_bit_set_count_rb(VALUE a_value)
{
  sarl_bit_set_t *a_set;
  Data_Get_Struct(a_value, sarl_bit_set_t, a_set);
  return INT2FIX(a_set ? sarl_bit_set_count(a_set) : 0);
};

static VALUE sarl_bit_set_first_rb(VALUE a_value)
{
  sarl_bit_set_t *a_set;
  Data_Get_Struct(a_value, sarl_bit_set_t, a_set);
  return INT2FIX(a_set ? sarl_bit_set_first(a_set) : 0);
};

static VALUE sarl_bit_set_last_rb(VALUE a_value)
{
  sarl_bit_set_t *a_set;
  Data_Get_Struct(a_value, sarl_bit_set_t, a_set);
  return INT2FIX(a_set ? sarl_bit_set_last(a_set) : 0);
};

static VALUE sarl_bit_set_dup_rb(VALUE a_value)
{
  sarl_bit_set_t *a_set;
  Data_Get_Struct(a_value, sarl_bit_set_t, a_set);
  return sarl_get_ruby_object(rb_cSarlBitSet, a_set ? sarl_bit_set_dup(a_set) : sarl_bit_set_default_new());
};

static VALUE sarl_bit_set_each_rb(VALUE self)
{
  sarl_bit_set_t *bset;
  sarl_unsigned_int g;
  Data_Get_Struct(self, sarl_bit_set_t, bset);

  if ( bset ) {
    if ( rb_block_given_p() ) {
      SARL_BIT_SET_FOR_EACH(g, bset);
      rb_yield(INT2FIX(g));
      SARL_BIT_SET_END;
    }
    else {
      rb_raise (rb_eRuntimeError, "must be called with a block");
    }
  }

  return Qnil;
};

// ---------------------------------------------------------------------
// SarlSat Wrapper
// ---------------------------------------------------------------------

static VALUE sarl_sat_solver_rb_alloc(VALUE klass)
{
  sarl_sat_solver_t* solver = sarl_sat_solver_default_new();
  return sarl_get_ruby_object(klass, solver);
};

static VALUE sarl_sat_solver_add_clause_rb(VALUE self, VALUE clause_v)
{
  sarl_sat_solver_t *solver;
  sarl_clause_t *clause;

  if ( ! rb_obj_is_kind_of(clause_v, rb_cSarlClause) ) {
    rb_raise (rb_eTypeError, "expected Sarl::Clause for second argument.");
  }
  else {
    Data_Get_Struct(self, sarl_sat_solver_t, solver);
    Data_Get_Struct(clause_v, sarl_clause_t, clause);
    sarl_sat_solver_add_clause(solver, clause);
  }

  return Qnil;
};

// ---------------------------------------------------------------------
// SarlIntArray Wrapper
// ---------------------------------------------------------------------

static VALUE sarl_int_array_rb_alloc(VALUE klass)
{
  sarl_int_array_t* int_array = sarl_int_array_default_new();
  return sarl_get_ruby_object(klass, int_array);
};

static VALUE sarl_int_array_each_rb(VALUE self)
{
  sarl_int_array_t *array;
  sarl_unsigned_int g;
  Data_Get_Struct(self, sarl_int_array_t, array);

  if ( rb_block_given_p() ) {
    SARL_INT_ARRAY_FOR_EACH(g, array);
      rb_yield(INT2FIX(g));
    SARL_INT_ARRAY_END;
  }
  else {
    rb_raise (rb_eRuntimeError, "must be called with a block");
  }

  return Qnil;
};

static VALUE sarl_int_array_get_rb(VALUE self, VALUE index_value)
{
  sarl_int_array_t *array;
  Data_Get_Struct(self, sarl_int_array_t, array);

  return INT2FIX(sarl_int_array_get(array, FIX2INT(index_value)));
};

static VALUE sarl_int_array_set_rb(VALUE self, 
  VALUE index_value, VALUE value_value)
{
  sarl_int_array_t *array;
  Data_Get_Struct(self, sarl_int_array_t, array);

  sarl_int_array_set(array, FIX2INT(index_value), FIX2INT(value_value));
  return Qnil;
};

static VALUE sarl_int_array_length_rb(VALUE self)
{
  sarl_int_array_t *array;
  Data_Get_Struct(self, sarl_int_array_t, array);

  return INT2FIX(array->length);
};

// ---------------------------------------------------------------------
// SarlPowerSet Wrapper
// ---------------------------------------------------------------------

static VALUE sarl_power_set_rb_alloc(VALUE klass)
{
  sarl_power_set_t* power_set = sarl_power_set_default_new();
  return sarl_get_ruby_object(klass, power_set);
};

static VALUE sarl_power_set_insert_rb(VALUE self, VALUE set_value)
{
  sarl_power_set_t* power_set;
  sarl_bit_set_t* bset;

  if ( ! rb_obj_is_kind_of(self, rb_cSarlPowerSet) ) {
    rb_raise (rb_eTypeError, "expected power set for self.");
  }
  else if ( ! rb_obj_is_kind_of(set_value, rb_cSarlBitSet) ) {
    rb_raise (rb_eTypeError, "expected bit set for argument 1.");
  }
  else {
    Data_Get_Struct(self, sarl_power_set_t, power_set);
    Data_Get_Struct(set_value, sarl_bit_set_t, bset);
    sarl_power_set_insert(power_set, bset);
  }
  return self;
}

static VALUE sarl_power_set_closure_rb(VALUE self, VALUE attributes_value)
{
  sarl_power_set_t* power_set;
  sarl_bit_set_t* attributes;

  if ( ! rb_obj_is_kind_of(self, rb_cSarlPowerSet) ) {
    rb_raise (rb_eTypeError, "expected power set as self.");
  }
  else if ( ! rb_obj_is_kind_of(attributes_value, rb_cSarlBitSet) ) {
    rb_raise (rb_eTypeError, "expected bit set for argument 1.");
  }
  else {
    Data_Get_Struct(self, sarl_power_set_t, power_set);
    Data_Get_Struct(attributes_value, sarl_bit_set_t, attributes);

    return sarl_get_ruby_object(
      rb_cSarlPowerSet, 
      sarl_power_set_closure(power_set, attributes)
    );
  }
  return Qnil;
}

static VALUE sarl_power_set_count_rb(VALUE self)
{
  sarl_power_set_t* power_set;
  Data_Get_Struct(self, sarl_power_set_t, power_set);
  return INT2FIX(sarl_power_set_count(power_set));
}

static VALUE sarl_power_set_each_rb(VALUE self)
{
  sarl_power_set_t *ps;
  sarl_bit_set_t* gs;
  Data_Get_Struct(self, sarl_power_set_t, ps);

  if ( rb_block_given_p() ) {
    SARL_POWER_SET_FOR_EACH(gs, ps);
      rb_yield(sarl_get_ruby_object(rb_cSarlBitSet, gs));
    SARL_POWER_SET_END;
  }
  else {
    rb_raise (rb_eRuntimeError, "must be called with a block");
  }

  return Qnil;
};

// ---------------------------------------------------------------------
// SarlPowerMap Wrapper
// ---------------------------------------------------------------------

static VALUE sarl_power_map_rb_alloc(VALUE klass)
{
  sarl_power_map_t* power_map = sarl_power_map_default_new();
  return sarl_get_ruby_object(klass, power_map);
};

static VALUE sarl_power_map_get_rb(VALUE self, VALUE index)
{
  sarl_power_map_t* power_map;
  Data_Get_Struct(self, sarl_power_map_t, power_map);

  return sarl_get_ruby_object(
    rb_cSarlBitSet, sarl_power_map_get(power_map, FIX2INT(index))
  );
}

static VALUE sarl_power_map_set_rb(VALUE self, VALUE index, VALUE bset_value)
{
  sarl_power_map_t* power_map;
  sarl_bit_set_t* bset;

  Data_Get_Struct(self, sarl_power_map_t, power_map);
  Data_Get_Struct(bset_value, sarl_bit_set_t, bset);

  sarl_power_map_set(power_map, FIX2INT(index), bset);
  return self;
}

static VALUE sarl_power_map_count_rb(VALUE self)
{
  sarl_power_map_t* power_map;
  Data_Get_Struct(self, sarl_power_map_t, power_map);
  return 0; // FIXME  return INT2FIX(sarl_power_map_count(power_map));
}

static VALUE sarl_power_map_each_rb(VALUE self)
{
  sarl_power_map_t *ps;
  sarl_bit_set_t* gs;
  sarl_unsigned_int index;
  Data_Get_Struct(self, sarl_power_map_t, ps);

  if ( rb_block_given_p() ) {
    SARL_POWER_MAP_FOR_EACH(index, gs, ps);
      rb_yield_values(
        2, INT2FIX(index), sarl_get_ruby_object(rb_cSarlBitSet, gs)
      );
    SARL_POWER_MAP_END;
  }
  else {
    rb_raise (rb_eRuntimeError, "must be called with a block");
  }

  return Qnil;
};

// ---------------------------------------------------------------------
// SarlClarifiedContext Wrapper
// ---------------------------------------------------------------------

static VALUE sarl_clarified_context_rb_alloc(VALUE klass)
{
  return Data_Wrap_Struct(klass, 0, sarl_free_ruby_object, 0);
};

static VALUE sarl_clarified_context_initialize_rb(VALUE self, VALUE obj)
{
  sarl_clarified_context_t *cc;
  Data_Get_Struct(self, sarl_clarified_context_t, cc);

  sarl_must_be(obj, rb_cSarlContext);

  sarl_context_t *context;
  Data_Get_Struct(obj, sarl_context_t, context);
  sarl_clarified_context_t *new_cc = sarl_context_clarify(context);
  sarl_set_ruby_object(self, new_cc);

  return self;
};

static VALUE sarl_clarified_context_context_rb(VALUE self)
{
  sarl_clarified_context_t *cc;
  Data_Get_Struct(self, sarl_clarified_context_t, cc);

  return sarl_get_ruby_object(rb_cSarlContext, cc->context);
};

static VALUE sarl_clarified_context_object_map_rb(VALUE self)
{
  sarl_clarified_context_t *cc;
  Data_Get_Struct(self, sarl_clarified_context_t, cc);

  return sarl_get_ruby_object(rb_cSarlIntArray, cc->object_map);
};

static VALUE sarl_clarified_context_attribute_map_rb(VALUE self)
{
  sarl_clarified_context_t *cc;
  Data_Get_Struct(self, sarl_clarified_context_t, cc);

  return sarl_get_ruby_object(rb_cSarlIntArray, cc->attribute_map);
};

static VALUE sarl_clarified_context_object_comap_rb(VALUE self)
{
  sarl_clarified_context_t *cc;
  Data_Get_Struct(self, sarl_clarified_context_t, cc);

  return sarl_get_ruby_object(rb_cSarlPowerMap, cc->object_comap);
};

static VALUE sarl_clarified_context_attribute_comap_rb(VALUE self)
{
  sarl_clarified_context_t *cc;
  Data_Get_Struct(self, sarl_clarified_context_t, cc);

  return sarl_get_ruby_object(rb_cSarlPowerMap, cc->attribute_comap);
};

// ---------------------------------------------------------------------
// SarlContext Wrapper
// ---------------------------------------------------------------------

static VALUE sarl_context_rb_alloc(VALUE klass)
{
  sarl_context_t* context = sarl_context_new();
  return sarl_get_ruby_object(klass, context);
};

static VALUE sarl_context_insert_rb(VALUE self, VALUE g, VALUE m)
{
  sarl_context_t* context;

  Data_Get_Struct(self, sarl_context_t, context);
  sarl_context_insert(context, NUM2INT(g), NUM2INT(m));

  return self;
}

static VALUE sarl_context_remove_rb(VALUE self, VALUE g, VALUE m)
{
  sarl_context_t* context;

  Data_Get_Struct(self, sarl_context_t, context);
  sarl_context_remove(context, NUM2INT(g), NUM2INT(m));

  return self;
}

static VALUE sarl_context_insert_object_rb(VALUE self, VALUE g)
{
  sarl_context_t* context;

  Data_Get_Struct(self, sarl_context_t, context);
  sarl_context_insert_object(context, NUM2INT(g));

  return self;
}

static VALUE sarl_context_insert_attribute_rb(VALUE self, VALUE m)
{
  sarl_context_t* context;

  Data_Get_Struct(self, sarl_context_t, context);
  sarl_context_insert_attribute(context, NUM2INT(m));

  return self;
}

static VALUE sarl_context_remove_object_rb(VALUE self, VALUE g)
{
  sarl_context_t* context;

  Data_Get_Struct(self, sarl_context_t, context);
  sarl_context_remove_object(context, NUM2INT(g));

  return self;
}

static VALUE sarl_context_remove_attribute_rb(VALUE self, VALUE m)
{
  sarl_context_t* context;

  Data_Get_Struct(self, sarl_context_t, context);
  sarl_context_remove_attribute(context, NUM2INT(m));

  return self;
}

static VALUE sarl_context_create_b3_rb()
{
  return sarl_get_ruby_object(rb_cSarlContext, sarl_context_create_b3());
};

static VALUE sarl_context_objects_rb(VALUE self)
{
  sarl_context_t* context;
  Data_Get_Struct(self, sarl_context_t, context);
  return sarl_get_ruby_object(rb_cSarlBitSet, context->G);
};

static VALUE sarl_context_attributes_rb(VALUE self)
{
  sarl_context_t* context;
  Data_Get_Struct(self, sarl_context_t, context);
  return sarl_get_ruby_object(rb_cSarlBitSet, context->M);
};

static VALUE sarl_context_object_intent_rb(VALUE self, VALUE g_value)
{
  sarl_context_t* context;
  Data_Get_Struct(self, sarl_context_t, context);
  return sarl_get_ruby_object(rb_cSarlBitSet, 
    sarl_context_object_intent(context, FIX2INT(g_value))
  );
};

static VALUE sarl_context_attribute_extent_rb(VALUE self, VALUE m_value)
{
  sarl_context_t* context;
  Data_Get_Struct(self, sarl_context_t, context);
  return sarl_get_ruby_object(rb_cSarlBitSet, 
    sarl_context_attribute_extent(context, FIX2INT(m_value))
  );
};

static VALUE sarl_context_attribute_imps_rb(VALUE self)
{
  sarl_context_t* context;
  Data_Get_Struct(self, sarl_context_t, context);
  return sarl_get_ruby_object(rb_cSarlPowerMap, 
    sarl_context_attribute_imps(context)
  );
};

static VALUE sarl_context_object_imps_rb(VALUE self)
{
  sarl_context_t* context;
  Data_Get_Struct(self, sarl_context_t, context);
  return sarl_get_ruby_object(rb_cSarlPowerMap, 
    sarl_context_object_imps(context)
  );
};

static VALUE sarl_context_top_intent_rb(VALUE self)
{
  sarl_context_t* context;
  Data_Get_Struct(self, sarl_context_t, context);
  return sarl_get_ruby_object(rb_cSarlBitSet, 
    sarl_context_top_intent(context)
  );
};

static VALUE sarl_context_is_maximal_intent_rb(VALUE self, VALUE value)
{
  sarl_context_t* context;
  sarl_bit_set_t* bset;

  sarl_must_be(value, rb_cSarlBitSet);

  Data_Get_Struct(self, sarl_context_t, context);
  Data_Get_Struct(value, sarl_bit_set_t, bset);

  return 
    sarl_context_is_maximal_intent(context, bset) ? Qtrue : Qfalse;
};

static VALUE sarl_context_attribute_subcontext_rb(VALUE self, VALUE value)
{
  sarl_context_t* context;
  sarl_bit_set_t* bset;

  sarl_must_be(value, rb_cSarlBitSet);

  Data_Get_Struct(self, sarl_context_t, context);
  Data_Get_Struct(value, sarl_bit_set_t, bset);

  return sarl_get_ruby_object(rb_cSarlContext, 
    sarl_context_attribute_subcontext(context, bset)
  );
};

static VALUE sarl_context_object_subcontext_rb(VALUE self, VALUE value)
{
  sarl_context_t* context;
  sarl_bit_set_t* bset;

  sarl_must_be(value, rb_cSarlBitSet);

  Data_Get_Struct(self, sarl_context_t, context);
  Data_Get_Struct(value, sarl_bit_set_t, bset);

  return sarl_get_ruby_object(rb_cSarlContext, 
    sarl_context_object_subcontext(context, bset)
  );
};

// ---------------------------------------------------------------------
// SarlStringTable Wrapper
// ---------------------------------------------------------------------

static VALUE sarl_string_table_rb_alloc(VALUE klass)
{
  sarl_string_table_t* string_table = sarl_string_table_default_new();
  return sarl_get_ruby_object(klass, string_table);
};

static VALUE sarl_string_table_set_rb(VALUE self, VALUE index, VALUE name)
{
  sarl_string_table_t* string_table;
  VALUE str = StringValue(name);
  Data_Get_Struct(self, sarl_string_table_t, string_table);
  sarl_string_table_set(string_table, NUM2INT(index), RSTRING(str)->ptr);

  return self;
}

static VALUE sarl_string_table_push_rb(VALUE self, VALUE name)
{
  sarl_string_table_t* string_table;
  VALUE str = StringValue(name);
  Data_Get_Struct(self, sarl_string_table_t, string_table);
  sarl_string_table_push(string_table, RSTRING(str)->ptr);

  return self;
}

static VALUE sarl_string_table_get_rb(VALUE self, VALUE index)
{
  sarl_string_table_t* string_table;
  Data_Get_Struct(self, sarl_string_table_t, string_table);
  return rb_str_new2(sarl_string_table_get(string_table, NUM2INT(index)));
}

static VALUE sarl_string_table_count_rb(VALUE self, VALUE index)
{
  sarl_string_table_t* string_table;
  Data_Get_Struct(self, sarl_string_table_t, string_table);
  return INT2FIX(sarl_string_table_count(string_table));
}

static VALUE sarl_string_table_populate_rb(VALUE self, VALUE lower, VALUE upper)
{
  sarl_string_table_t* string_table;
  Data_Get_Struct(self, sarl_string_table_t, string_table);
  sarl_string_table_populate(string_table, NUM2INT(lower), NUM2INT(upper));
  return Qnil;
}

static VALUE sarl_string_table_each_rb(int argc, VALUE *argv, VALUE self)
{
  sarl_string_table_t* string_table;
  sarl_unsigned_int i;

  if ( rb_block_given_p() ) {
    Data_Get_Struct(self, sarl_string_table_t, string_table);
    
    for(i=0;i<string_table->length;++i) {
      if ( string_table->table[i] ) {
        rb_yield(rb_str_new2(string_table->table[i]));
      }
    }
  }
  else {
    rb_raise (rb_eRuntimeError, "must be called with a block");
  }

  return Qnil;
}

static VALUE sarl_string_table_each_pair_rb(int argc, VALUE *argv, VALUE self)
{
  sarl_string_table_t* string_table;
  sarl_unsigned_int i;

  if ( rb_block_given_p() ) {
    Data_Get_Struct(self, sarl_string_table_t, string_table);

    for(i=0;i<string_table->length;++i) {
      if ( string_table->table[i] ) {
        rb_yield_values(2, INT2FIX(i), rb_str_new2(string_table->table[i]));
      }
    }
  }
  else {
    rb_raise (rb_eRuntimeError, "must be called with a block");
  }

  return Qnil;
}

static VALUE sarl_string_table_each_index_rb(int argc, VALUE *argv, VALUE self)
{
  sarl_string_table_t* string_table;
  sarl_unsigned_int i;
  Data_Get_Struct(self, sarl_string_table_t, string_table);

  if ( rb_block_given_p() ) {
    for(i=0;i<string_table->length;++i) {
      if ( string_table->table[i] ) {
        rb_yield(INT2FIX(i));
      }
    }
  }
  else {
    rb_raise (rb_eRuntimeError, "must be called with a block");
  }

  return Qnil;
}

// ---------------------------------------------------------------------
// SarlLattice Wrapper
// ---------------------------------------------------------------------

static VALUE sarl_lattice_rb_alloc(VALUE klass)
{
  return Data_Wrap_Struct(klass, 0, sarl_free_ruby_object, 0);
};

static VALUE sarl_lattice_initialize_rb(VALUE self, VALUE obj)
{
  sarl_lattice_t* lattice;
  sarl_context_t* context;

  sarl_lattice_t* new_lattice;

  Data_Get_Struct(self, sarl_lattice_t, lattice);

  if ( rb_obj_is_kind_of(obj, rb_cSarlContext) ) {
    Data_Get_Struct(obj, sarl_context_t, context);
    new_lattice = sarl_lattice_new(context);
    sarl_set_ruby_object(self, new_lattice);
  }
  else {
    rb_raise(rb_eTypeError, "Invalid type to lattice initialize. Must be a context.");
  }

  return self;
};

static VALUE sarl_lattice_count_rb(VALUE self)
{
  sarl_lattice_t* lattice;
  Data_Get_Struct(self, sarl_lattice_t, lattice);
  return INT2FIX(sarl_lattice_count(lattice));
};

static VALUE sarl_lattice_context_rb(VALUE self)
{
  sarl_lattice_t* lattice;
  Data_Get_Struct(self, sarl_lattice_t, lattice);
  return sarl_get_ruby_object(rb_cSarlContext, lattice->context);
};

static VALUE sarl_lattice_upset_rb(VALUE self)
{
  sarl_lattice_t* lattice;
  Data_Get_Struct(self, sarl_lattice_t, lattice);
  return sarl_get_ruby_object(rb_cSarlPowerMap, lattice->upset);
};

static VALUE sarl_lattice_downset_rb(VALUE self)
{
  sarl_lattice_t* lattice;
  Data_Get_Struct(self, sarl_lattice_t, lattice);
  return sarl_get_ruby_object(rb_cSarlPowerMap, lattice->downset);
};

static VALUE sarl_lattice_lower_covers_rb(VALUE self)
{
  sarl_lattice_t* lattice;
  Data_Get_Struct(self, sarl_lattice_t, lattice);
  return sarl_get_ruby_object(rb_cSarlPowerMap, lattice->lower_covers);
};

static VALUE sarl_lattice_upper_covers_rb(VALUE self)
{
  sarl_lattice_t* lattice;
  Data_Get_Struct(self, sarl_lattice_t, lattice);
  return sarl_get_ruby_object(rb_cSarlPowerMap, lattice->upper_covers);
};

static VALUE sarl_lattice_extents_rb(VALUE self)
{
  sarl_lattice_t* lattice;
  Data_Get_Struct(self, sarl_lattice_t, lattice);
  return sarl_get_ruby_object(rb_cSarlPowerMap, lattice->concept_extents);
};

static VALUE sarl_lattice_intents_rb(VALUE self)
{
  sarl_lattice_t* lattice;
  Data_Get_Struct(self, sarl_lattice_t, lattice);
  return sarl_get_ruby_object(rb_cSarlPowerMap, lattice->concept_intents);
};

static VALUE sarl_lattice_object_contingents_rb(VALUE self)
{
  sarl_lattice_t* lattice;
  Data_Get_Struct(self, sarl_lattice_t, lattice);
  return sarl_get_ruby_object(rb_cSarlPowerMap, lattice->concept_object_cont);
};

static VALUE sarl_lattice_attribute_contingents_rb(VALUE self)
{
  sarl_lattice_t* lattice;
  Data_Get_Struct(self, sarl_lattice_t, lattice);
  return sarl_get_ruby_object(
    rb_cSarlPowerMap, lattice->concept_attribute_cont);
};

static VALUE sarl_lattice_reduced_context_rb(VALUE self)
{
  sarl_lattice_t* lattice;
  Data_Get_Struct(self, sarl_lattice_t, lattice);
  return sarl_get_ruby_object(
    rb_cSarlLattice, sarl_lattice_reduced_context(lattice));
};

static VALUE sarl_lattice_distributive_closure_context_rb(VALUE self)
{
  sarl_lattice_t* lattice;
  Data_Get_Struct(self, sarl_lattice_t, lattice);
  return sarl_get_ruby_object(
    rb_cSarlContext, sarl_lattice_distributive_closure_context(lattice));
};

static VALUE sarl_lattice_each_rb(VALUE self)
{
  sarl_lattice_t *bset;
  sarl_unsigned_int g;
  Data_Get_Struct(self, sarl_lattice_t, bset);

  if ( rb_block_given_p() ) {
    SARL_LATTICE_FOR_EACH(g, bset);
      rb_yield(INT2FIX(g));
    SARL_LATTICE_END;
  }
  else {
    rb_raise (rb_eRuntimeError, "must be called with a block");
  }

  return Qnil;
};

// ---------------------------------------------------------------------
// SarlLayout Wrapper
// ---------------------------------------------------------------------

static VALUE sarl_diagram_layout_rb(VALUE klass, VALUE obj)
{
  sarl_lattice_t* lattice;
  sarl_context_t* context;
  sarl_diagram_t* new_diagram;

  // VALUE klass = rb_funcall(obj, rb_intern("class"), 0);

  if ( rb_obj_is_kind_of(obj, rb_cSarlLattice) ) {
    Data_Get_Struct(obj, sarl_lattice_t, lattice);
    new_diagram = sarl_diagram_layout_from_lattice(lattice);
  }
  else if ( rb_obj_is_kind_of(obj, rb_cSarlContext) ) {
    Data_Get_Struct(obj, sarl_context_t, context);
    new_diagram = sarl_diagram_layout_from_context(context);
  }
  else {
    rb_raise(rb_eTypeError, 
      "Invalid type to diagram initialize. Must be a context or a lattice");
  }

  if ( rb_cSarlDiagram == 0 ) {
    rb_raise(rb_eTypeError, "Internal Error --- diagram class not initialized.");
  }
  return sarl_get_ruby_object(rb_cSarlDiagram, new_diagram);
}

static VALUE 
  sarl_diagram_layout_array_rb(VALUE klass, VALUE obj, VALUE metrics)
{
  sarl_lattice_t  *lattice;
  sarl_context_t  *context;
  sarl_diagram_t  *diagram;
  sarl_array_t    *diagrams, *metric_array;

  VALUE result = rb_ary_new();

  // VALUE klass = rb_funcall(obj, rb_intern("class"), 0);

  metric_array = sarl_array_default_new();

  if ( ! rb_obj_is_kind_of(metrics, rb_cArray) ) {
    rb_raise(rb_eTypeError, "Second argument should be an array.");
  }

  if ( rb_cSarlDiagram == 0 ) {
    rb_raise(rb_eTypeError, 
      "Internal Error --- diagram class not initialized.");
  }

  if ( rb_obj_is_kind_of(obj, rb_cSarlLattice) ) {
    Data_Get_Struct(obj, sarl_lattice_t, lattice);
    diagrams = sarl_diagram_layout_array_from_lattice(lattice, metric_array);
  }
  else if ( rb_obj_is_kind_of(obj, rb_cSarlContext) ) {
    Data_Get_Struct(obj, sarl_context_t, context);
    diagrams = sarl_diagram_layout_array_from_context(context, metric_array);
  }
  else {
    rb_raise(rb_eTypeError, 
      "Invalid type to diagram initialize. Must be a context or a lattice");
  }

  sarl_unsigned_int index = 1;
  sarl_int_array_t *diagram_metrics;
  SARL_ARRAY_FOR_EACH(sarl_diagram_t, diagram, diagrams);
    rb_ary_push(
      result, sarl_get_ruby_object(rb_cSarlDiagram, diagram)
    );
    diagram_metrics = (sarl_int_array_t *)sarl_array_get(metric_array, index);
    if ( diagram_metrics ) {
      VALUE m = rb_ary_new();
      sarl_unsigned_int mi, mv, len = sarl_int_array_count(diagram_metrics);
      for(mi=1;mi<=len;++mi) {
        mv = sarl_int_array_get(diagram_metrics, mi);
        rb_ary_store(m, (long)mi-1, INT2FIX(SARL_UNSIGN_TO_SIGN(mv)));
      }
      rb_ary_push(metrics, m);
    };
    ++index;
  SARL_ARRAY_END;

  return result;
};

// ---------------------------------------------------------------------
// SarlDiagram Wrapper
// ---------------------------------------------------------------------

static VALUE sarl_diagram_dump_rb(VALUE a_value)
{
  sarl_diagram_t *diagram;
  Data_Get_Struct(a_value, sarl_diagram_t, diagram);
  sarl_unsigned_int i;
  fprintf(stderr, "diagram: length=%u\n", diagram->length);
  fprintf(stderr, "  meet_vectors: ");
  for(i=0;i<diagram->length;++i) {
    fprintf(stderr, "(%d,%d)", 
      diagram->meet_vectors[i].x, 
      diagram->meet_vectors[i].y);
    if ( i != diagram->length-1 ) fprintf(stderr, ", ");
  }
  fprintf(stderr, "\n  concept_vectors: ");
  for(i=0;i<diagram->length;++i) {
    fprintf(stderr, "(%d,%d)", 
      diagram->concept_vectors[i].x, 
      diagram->concept_vectors[i].y);
    if ( i != diagram->length-1 ) fprintf(stderr, ", ");
  }
  fprintf(stderr, "\n  concept_pos: ");
  for(i=0;i<diagram->length;++i) {
    fprintf(stderr, "(%d,%d)", 
      diagram->concept_pos[i].x, 
      diagram->concept_pos[i].y);
    if ( i != diagram->length-1 ) fprintf(stderr, ", ");
  }
  fprintf(stderr, "\n");
  return Qnil;
};

static VALUE sarl_diagram_create_b3_rb(VALUE klass)
{
  sarl_diagram_t* diagram = sarl_diagram_create_b3();
  if ( klass != rb_cSarlDiagram ) {
    fprintf(stderr, "Something is wrong, what is the klass?\n");
  }
  return sarl_get_ruby_object(rb_cSarlDiagram, diagram);
};

static VALUE sarl_diagram_rb_alloc(VALUE klass)
{
  return Data_Wrap_Struct(klass, 0, sarl_free_ruby_object, 0);
};

static VALUE sarl_point_rb_alloc(VALUE klass)
{
  return Data_Wrap_Struct(klass, 0, sarl_free_ruby_object, 0);
};

static VALUE sarl_diagram_initialize_rb(VALUE self, VALUE obj)
{
  sarl_diagram_t* diagram;
  sarl_lattice_t* lattice;
  sarl_context_t* context;

  sarl_diagram_t* new_diagram;

  Data_Get_Struct(self, sarl_diagram_t, diagram);

  if ( rb_obj_is_kind_of(obj, rb_cSarlLattice) ) {
    Data_Get_Struct(obj, sarl_lattice_t, lattice);
    new_diagram = sarl_diagram_new_from_lattice(lattice);
    sarl_set_ruby_object(self, new_diagram);
  }
  else if ( rb_obj_is_kind_of(obj, rb_cSarlContext) ) {
    Data_Get_Struct(obj, sarl_context_t, context);
    new_diagram = sarl_diagram_new_from_context(context);
    sarl_set_ruby_object(self, new_diagram);
  }
  else {
    rb_raise(rb_eTypeError, "Invalid type to diagram initialize. Must be a context or a lattice");
  }

  return self;
}

static VALUE sarl_diagram_initialize_copy_rb(VALUE a_value, VALUE b_value)
{
  sarl_diagram_t *a_set, *b_set;
  Data_Get_Struct(a_value, sarl_diagram_t, a_set);
  Data_Get_Struct(b_value, sarl_diagram_t, b_set);
  rb_raise(rb_eNotImpError, "Not implemented yet!");
  // sarl_diagram_assign_equals(a_set, b_set);
  return a_value;
};

static VALUE sarl_diagram_lattice_rb(VALUE self)
{
  sarl_diagram_t *diagram;
  Data_Get_Struct(self, sarl_diagram_t, diagram);
  return sarl_get_ruby_object(rb_cSarlLattice, diagram->lattice);
}

static VALUE sarl_diagram_get_rb(VALUE self, VALUE concept_value)
{
  sarl_diagram_t *diagram;
  Data_Get_Struct(self, sarl_diagram_t, diagram);
  sarl_point_t *pt = sarl_diagram_get_concept_pos(diagram, FIX2INT(concept_value));
  if ( pt ) {
    return sarl_get_ruby_object(rb_cSarlPoint, pt);
  }
  else {
    return Qnil;
  }
}

static VALUE sarl_point_x_rb(VALUE self)
{
  sarl_point_t *point = 0;
  Data_Get_Struct(self, sarl_point_t, point);
  return point ? (INT2FIX(point->x)) : Qnil;
};

static VALUE sarl_point_y_rb(VALUE self)
{
  sarl_point_t *point = 0;
  Data_Get_Struct(self, sarl_point_t, point);
  return point ? (INT2FIX(point->y)) : Qnil;
};

// ---------------------------------------------------------------------
// Init Function
// ---------------------------------------------------------------------

typedef VALUE (*rfunc)(...);

void
Init_sarlrb()
{
  /* create Sarl module */
  rb_mSarl = rb_define_module ("Sarl");

  // ----------------------------------------------------------------------
  // BitSet
  // ----------------------------------------------------------------------

  /* create the BitSet class */
  rb_cSarlBitSet = rb_define_class_under(rb_mSarl, "BitSet", rb_cObject);
  rb_define_alloc_func(rb_cSarlBitSet, sarl_bit_set_rb_alloc);

  /* define BitSet methods */
  rb_define_method(rb_cSarlBitSet, "initialize_copy", 
    (rfunc)sarl_bit_set_initialize_copy_rb, 1);

  rb_define_method(rb_cSarlBitSet, "set", 
    (rfunc)sarl_bit_set_set_rb, 1);
  rb_define_method(rb_cSarlBitSet, "clear", 
    (rfunc)sarl_bit_set_clear_rb, 1);
  rb_define_method(rb_cSarlBitSet, "is_set", 
    (rfunc)sarl_bit_set_is_set_rb, 1);
  rb_define_method(rb_cSarlBitSet, "hash", 
    (rfunc)sarl_bit_set_hash_rb, 0);
  rb_define_method(rb_cSarlBitSet, "and", 
    (rfunc)sarl_bit_set_and_rb, 1);
  rb_define_method(rb_cSarlBitSet, "or", 
    (rfunc)sarl_bit_set_or_rb, 1);
  rb_define_method(rb_cSarlBitSet, "and_equals", 
    (rfunc)sarl_bit_set_and_equals_rb, 1);
  rb_define_method(rb_cSarlBitSet, "or_equals", 
    (rfunc)sarl_bit_set_or_equals_rb, 1);
  rb_define_method(rb_cSarlBitSet, "assign_equals", 
    (rfunc)sarl_bit_set_assign_equals_rb, 1);
  rb_define_method(rb_cSarlBitSet, "is_subset", 
    (rfunc)sarl_bit_set_is_subset_rb, 1);
  rb_define_method(rb_cSarlBitSet, "is_subseteq", 
    (rfunc)sarl_bit_set_is_subseteq_rb, 1);
  rb_define_method(rb_cSarlBitSet, "count", 
    (rfunc)sarl_bit_set_count_rb, 0);
  rb_define_method(rb_cSarlBitSet, "first", 
    (rfunc)sarl_bit_set_first_rb, 0);
  rb_define_method(rb_cSarlBitSet, "last", 
    (rfunc)sarl_bit_set_last_rb, 0);
  rb_define_method(rb_cSarlBitSet, "dup", 
    (rfunc)sarl_bit_set_dup_rb, 0);
  rb_define_method(rb_cSarlBitSet, "each", 
    (rfunc)sarl_bit_set_each_rb, 0);

  // ----------------------------------------------------------------------
  // Point
  // ----------------------------------------------------------------------

  rb_cSarlPoint = rb_define_class_under(rb_mSarl, "Point", rb_cObject);
  rb_define_alloc_func(rb_cSarlPoint, sarl_point_rb_alloc);

  rb_define_method(rb_cSarlPoint, "x", 
    (rfunc)sarl_point_x_rb, 0);

  rb_define_method(rb_cSarlPoint, "y", 
    (rfunc)sarl_point_y_rb, 0);

  // ----------------------------------------------------------------------
  // Diagram
  // ----------------------------------------------------------------------

  // class
  rb_cSarlDiagram = rb_define_class_under(rb_mSarl, "Diagram", rb_cObject);
  rb_define_alloc_func(rb_cSarlDiagram, sarl_diagram_rb_alloc);

  // methods
  rb_define_method(rb_cSarlDiagram, "initialize", 
    (rfunc)sarl_diagram_initialize_rb, 1);

  rb_define_method(rb_cSarlDiagram, "initialize_copy", 
    (rfunc)sarl_diagram_initialize_copy_rb, 1);

  rb_define_method(rb_cSarlDiagram, "dump", 
    (rfunc)sarl_diagram_dump_rb, 0);

  rb_define_method(rb_cSarlDiagram, "lattice", 
    (rfunc)sarl_diagram_lattice_rb, 0);

  rb_define_method(rb_cSarlDiagram, "get", 
    (rfunc)sarl_diagram_get_rb, 1);

  rb_define_module_function(rb_cSarlDiagram, "create_b3", 
    (rfunc)sarl_diagram_create_b3_rb, 0);

  rb_define_alias(rb_cSarlDiagram, "[]", "get");

  // ----------------------------------------------------------------------
  // Layout
  // ----------------------------------------------------------------------

  rb_define_singleton_method(rb_cSarlDiagram, "layout", 
    (rfunc)sarl_diagram_layout_rb, 1);

  rb_define_singleton_method(rb_cSarlDiagram, "layout_array", 
    (rfunc)sarl_diagram_layout_array_rb, 2);

  // ----------------------------------------------------------------------
  // Context
  // ----------------------------------------------------------------------

  // class
  rb_cSarlContext = rb_define_class_under(rb_mSarl, "Context", rb_cObject);
  rb_define_alloc_func(rb_cSarlContext, sarl_context_rb_alloc);

  rb_define_method(rb_cSarlContext, "insert", 
    (rfunc)sarl_context_insert_rb, 2);

  rb_define_method(rb_cSarlContext, "remove", 
    (rfunc)sarl_context_remove_rb, 2);

  rb_define_method(rb_cSarlContext, "insert_object", 
    (rfunc)sarl_context_insert_object_rb, 1);

  rb_define_method(rb_cSarlContext, "insert_attribute", 
    (rfunc)sarl_context_insert_attribute_rb, 1);

  rb_define_method(rb_cSarlContext, "remove_object", 
    (rfunc)sarl_context_remove_object_rb, 1);

  rb_define_method(rb_cSarlContext, "remove_attribute", 
    (rfunc)sarl_context_remove_attribute_rb, 1);

  rb_define_singleton_method(rb_cSarlContext, "create_b3", 
    (rfunc)sarl_context_create_b3_rb, 0);

  rb_define_method(rb_cSarlContext, "objects", 
    (rfunc)sarl_context_objects_rb, 0);

  rb_define_method(rb_cSarlContext, "attributes", 
    (rfunc)sarl_context_attributes_rb, 0);

  rb_define_method(rb_cSarlContext, "object_intent", 
    (rfunc)sarl_context_object_intent_rb, 1);

  rb_define_method(rb_cSarlContext, "attribute_extent", 
    (rfunc)sarl_context_attribute_extent_rb, 1);

  rb_define_method(rb_cSarlContext, "object_subcontext", 
    (rfunc)sarl_context_object_subcontext_rb, 1);

  rb_define_method(rb_cSarlContext, "object_imps", 
    (rfunc)sarl_context_object_imps_rb, 0);

  rb_define_method(rb_cSarlContext, "attribute_subcontext", 
    (rfunc)sarl_context_attribute_subcontext_rb, 1);

  rb_define_method(rb_cSarlContext, "attribute_imps", 
    (rfunc)sarl_context_attribute_imps_rb, 0);

  rb_define_method(rb_cSarlContext, "top_intent", 
    (rfunc)sarl_context_top_intent_rb, 0);

  rb_define_method(rb_cSarlContext, "is_maximal_intent", 
    (rfunc)sarl_context_is_maximal_intent_rb, 1);

  // ----------------------------------------------------------------------
  // ClarifiedContext
  // ----------------------------------------------------------------------

  // class
  rb_cSarlClarifiedContext = rb_define_class_under(
    rb_mSarl, "ClarifiedContext", rb_cObject);
  rb_define_alloc_func(rb_cSarlClarifiedContext, 
    sarl_clarified_context_rb_alloc);

  // methods
  rb_define_method(rb_cSarlClarifiedContext, "initialize", 
    (rfunc)sarl_clarified_context_initialize_rb, 1);

  rb_define_method(rb_cSarlClarifiedContext, "context", 
    (rfunc)sarl_clarified_context_context_rb, 0);

  rb_define_method(rb_cSarlClarifiedContext, "object_map", 
    (rfunc)sarl_clarified_context_object_map_rb, 0);

  rb_define_method(rb_cSarlClarifiedContext, "attribute_map", 
    (rfunc)sarl_clarified_context_attribute_map_rb, 0);

  rb_define_method(rb_cSarlClarifiedContext, "object_comap", 
    (rfunc)sarl_clarified_context_object_comap_rb, 0);

  rb_define_method(rb_cSarlClarifiedContext, "attribute_comap", 
    (rfunc)sarl_clarified_context_attribute_comap_rb, 0);

  // ----------------------------------------------------------------------
  // Lattice
  // ----------------------------------------------------------------------

  // class
  rb_cSarlLattice = rb_define_class_under(rb_mSarl, "Lattice", rb_cObject);
  rb_define_alloc_func(rb_cSarlLattice, sarl_lattice_rb_alloc);

  // methods
  rb_define_method(rb_cSarlLattice, "initialize", 
    (rfunc)sarl_lattice_initialize_rb, 1);

  rb_define_method(rb_cSarlLattice, "count", 
    (rfunc)sarl_lattice_count_rb, 0);

  rb_define_method(rb_cSarlLattice, "context", 
    (rfunc)sarl_lattice_context_rb, 0);

  rb_define_method(rb_cSarlLattice, "extents", 
    (rfunc)sarl_lattice_extents_rb, 0);

  rb_define_method(rb_cSarlLattice, "intents", 
    (rfunc)sarl_lattice_intents_rb, 0);

  rb_define_method(rb_cSarlLattice, "upset", 
    (rfunc)sarl_lattice_upset_rb, 0);

  rb_define_method(rb_cSarlLattice, "downset", 
    (rfunc)sarl_lattice_downset_rb, 0);

  rb_define_method(rb_cSarlLattice, "upper_covers", 
    (rfunc)sarl_lattice_upper_covers_rb, 0);

  rb_define_method(rb_cSarlLattice, "lower_covers", 
    (rfunc)sarl_lattice_lower_covers_rb, 0);

  rb_define_method(rb_cSarlLattice, "object_contingents", 
    (rfunc)sarl_lattice_object_contingents_rb, 0);

  rb_define_method(rb_cSarlLattice, "attribute_contingents", 
    (rfunc)sarl_lattice_attribute_contingents_rb, 0);

  rb_define_method(rb_cSarlLattice, "reduced_context", 
    (rfunc)sarl_lattice_reduced_context_rb, 0);

  rb_define_method(rb_cSarlLattice, "distributive_closure_context", 
    (rfunc)sarl_lattice_distributive_closure_context_rb, 0);

  rb_define_method(rb_cSarlLattice, "each", 
    (rfunc)sarl_lattice_each_rb, 0);

  // ----------------------------------------------------------------------
  // PowerSet
  // ----------------------------------------------------------------------

  // class
  rb_cSarlPowerSet = rb_define_class_under(rb_mSarl, "PowerSet", rb_cObject);
  rb_define_alloc_func(rb_cSarlPowerSet, sarl_power_set_rb_alloc);

  // methods
  rb_define_method(rb_cSarlPowerSet, "closure", 
    (rfunc)sarl_power_set_closure_rb, 1);

  rb_define_method(rb_cSarlPowerSet, "insert", 
    (rfunc)sarl_power_set_insert_rb, 1);

  rb_define_method(rb_cSarlPowerSet, "count", 
    (rfunc)sarl_power_set_count_rb, 0);

  rb_define_method(rb_cSarlPowerSet, "each", 
    (rfunc)sarl_power_set_each_rb, 0);

  // ----------------------------------------------------------------------
  // PowerMap
  // ----------------------------------------------------------------------

  // class
  rb_cSarlPowerMap = rb_define_class_under(rb_mSarl, "PowerMap", rb_cObject);
  rb_define_alloc_func(rb_cSarlPowerMap, sarl_power_map_rb_alloc);

  // methods
  rb_define_method(rb_cSarlPowerMap, "set", 
    (rfunc)sarl_power_map_set_rb, 2);

  rb_define_method(rb_cSarlPowerMap, "get", 
    (rfunc)sarl_power_map_get_rb, 1);

  rb_define_method(rb_cSarlPowerMap, "count", 
    (rfunc)sarl_power_map_count_rb, 0);

  rb_define_method(rb_cSarlPowerMap, "each", 
    (rfunc)sarl_power_map_each_rb, 0);

  rb_define_alias(rb_cSarlPowerMap, "[]=", "set");
  rb_define_alias(rb_cSarlPowerMap, "[]", "get");

  // ----------------------------------------------------------------------
  // StringTable
  // ----------------------------------------------------------------------

  // class
  rb_cSarlStringTable = rb_define_class_under(rb_mSarl, "StringTable", rb_cObject);
  rb_define_alloc_func(rb_cSarlStringTable, sarl_string_table_rb_alloc);

  // methods
  rb_define_method(rb_cSarlStringTable, "set", 
    (rfunc)sarl_string_table_set_rb, 2);
  rb_define_method(rb_cSarlStringTable, "push", 
    (rfunc)sarl_string_table_push_rb, 1);
  rb_define_method(rb_cSarlStringTable, "get", 
    (rfunc)sarl_string_table_get_rb, 1);
  rb_define_method(rb_cSarlStringTable, "count", 
    (rfunc)sarl_string_table_count_rb, 0);
  rb_define_method(rb_cSarlStringTable, "populate", 
    (rfunc)sarl_string_table_populate_rb, 2);

  // the following take block as arguments
  rb_define_method(rb_cSarlStringTable, "each", 
    (rfunc)sarl_string_table_each_rb, -1);
  rb_define_method(rb_cSarlStringTable, "each_pair", 
    (rfunc)sarl_string_table_each_pair_rb, -1);
  rb_define_method(rb_cSarlStringTable, "each_index", 
    (rfunc)sarl_string_table_each_index_rb, -1);

  rb_define_alias(rb_cSarlStringTable, "[]=", "set");
  rb_define_alias(rb_cSarlStringTable, "[]", "get");
  rb_define_alias(rb_cSarlStringTable, "length", "count");

  // ----------------------------------------------------------------------
  // IntArray
  // ----------------------------------------------------------------------

  /* create the IntArray class */
  rb_cSarlIntArray = rb_define_class_under(rb_mSarl, "IntArray", rb_cObject);
  rb_define_alloc_func(rb_cSarlIntArray, sarl_int_array_rb_alloc);

  /* define IntArray methods */
  rb_define_method(rb_cSarlIntArray, "set", 
    (rfunc)sarl_int_array_set_rb, 2);
  rb_define_method(rb_cSarlIntArray, "get", 
    (rfunc)sarl_int_array_get_rb, 1);

  rb_define_method(rb_cSarlIntArray, "each", 
    (rfunc)sarl_int_array_each_rb, 0);

  rb_define_method(rb_cSarlIntArray, "length", 
    (rfunc)sarl_int_array_length_rb, 0);

  rb_define_alias(rb_cSarlIntArray, "[]=", "set");
  rb_define_alias(rb_cSarlIntArray, "[]", "get");
}


