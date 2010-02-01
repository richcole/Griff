#include "sarl_lattice_view_gtk_rb.h"
#include "sarl_gtk_rb.h"
#include "sarl_lattice_view_gtk.h"

extern "C" {
#include "rbglib.h"
#include "rbgobject.h"
#include "rbgtk.h" 
}

static VALUE
sarl_lattice_view_initialize_rb(
  VALUE self, VALUE diagram_value, VALUE gst_value, VALUE mst_value)
{
  sarl_diagram_t *diagram = 0;
  sarl_string_table_t *gst = 0, *mst = 0;

  Data_Get_Struct(diagram_value, sarl_diagram_t, diagram);
  Data_Get_Struct(gst_value, sarl_string_table_t, gst);
  Data_Get_Struct(mst_value, sarl_string_table_t, mst);
  RBGTK_INITIALIZE(self, sarl_lattice_view_new(diagram, gst, mst));
  return Qnil;
}

/*
VALUE sarl_lattice_view_set_font_size_rb(VALUE self, VALUE font_size)
{
  SarlLatticeView *view = SARL_LATTICE_VIEW(RVAL2GOBJ(self));
  sarl_lattice_view_set_font_size(view, NUM2INT(font_size));
  return self;
};

VALUE sarl_lattice_view_get_font_size_rb(VALUE self, VALUE font_size)
{
  SarlLatticeView *view = SARL_LATTICE_VIEW(RVAL2GOBJ(self));
  return INT2FIX(sarl_lattice_view_get_font_size(view));
};
*/

static VALUE
sarl_lattice_view_set_diagram_rb(
  VALUE self, VALUE diagram_value, VALUE gst_value, VALUE mst_value)
{
  SarlLatticeView *view;
  sarl_diagram_t *diagram = 0;
  sarl_string_table_t *gst = 0, *mst = 0;

  view = SARL_LATTICE_VIEW(RVAL2GOBJ(self));
  Data_Get_Struct(diagram_value, sarl_diagram_t, diagram);
  Data_Get_Struct(gst_value, sarl_string_table_t, gst);
  Data_Get_Struct(mst_value, sarl_string_table_t, mst);
  sarl_lattice_view_set_diagram(view, diagram, gst, mst);
  return Qnil;
}

static VALUE
  sarl_lattice_view_center_labels_rb(VALUE self)
{
  SarlLatticeView *view = SARL_LATTICE_VIEW(RVAL2GOBJ(self));
  sarl_lattice_view_center_labels(view);
  sarl_lattice_view_update(view);
  return Qnil;
};

static VALUE
  sarl_lattice_view_recalc_labels_rb(VALUE self)
{
  SarlLatticeView *view = SARL_LATTICE_VIEW(RVAL2GOBJ(self));
  return Qnil;
};  

static VALUE
  sarl_lattice_view_save_image_rb(VALUE self, VALUE filename, VALUE type)
{
  SarlLatticeView *view = SARL_LATTICE_VIEW(RVAL2GOBJ(self));
  sarl_lattice_view_save_image(
    view, RSTRING(filename)->ptr, RSTRING(type)->ptr
  );
  return Qnil;
};

void 
Init_sarl_lattice_view_gtk(VALUE mSarlGtk)
{
  VALUE gSarlLatticeView = 
    G_DEF_CLASS(SARL_LATTICE_VIEW_TYPE, "LatticeView", mSarlGtk);

  rb_define_method(gSarlLatticeView, 
    "initialize", (VALUE (*)(...)) sarl_lattice_view_initialize_rb, 3);

  rb_define_method(gSarlLatticeView, 
    "set_diagram",(VALUE (*)(...)) sarl_lattice_view_set_diagram_rb, 3);

  rb_define_method(gSarlLatticeView, 
    "center_labels",(VALUE (*)(...)) sarl_lattice_view_center_labels_rb, 0);

  rb_define_method(gSarlLatticeView, 
    "save_image", (VALUE (*)(...)) sarl_lattice_view_save_image_rb, 2);

  rb_define_method(gSarlLatticeView, 
    "recalc_labels", (VALUE (*)(...)) sarl_lattice_view_recalc_labels_rb, 0);

/*
  rb_define_method(gSarlLatticeView, 
    "set_font_size", (VALUE (*)(...)) sarl_lattice_view_set_font_size_rb, 1);
  rb_define_method(gSarlLatticeView, 
    "get_font_size", (VALUE (*)(...)) sarl_lattice_view_get_font_size_rb, 0);

  rb_define_alias(gSarlLatticeView, "font_size=", "set_font_size");
  rb_define_alias(gSarlLatticeView, "font_size", "get_font_size");
*/
}
