#include "sarl_lattice_view_gtk.h"
#include "sarl_diagram.h"
#include "sarl_lattice.h"
#include "sarl_bit_set.h"
#include "sarl_power_map.h"
#include "sarl_string_table.h"
#include "sarl_lattice_view_gtk_marshaller.h"

#include "sarl_label.h"
#include "sarl_lattice_view_gtk_privates.h"
#include "sarl_lattice_view_macros.h"

#include <gtk/gtksignal.h>

// ----------------------------------------------------------------------
// TODO List
//
// 1. Seperate out the style stuff into a style class, follow the Gnome
//    standard for widget styles, if there is one that can be adopted.
//
// 2. Add object and attribute clarrification options
//
// 3. Add an option to display object and attribute instead of
//    just listing the objects and attributes
//
// 4. Modify the label code to support vertical scrollbars. See if the
//    gtk code supports drawing of the scrollbar. Try to adopt this.
//    Alternatively could try using the code that draws list boxes, but this
//    is likely to be too heavy weight.
//
// 5. Look into making labels GObjects. What is the advantage of this?

// ----------------------------------------------------------------------
// Internationalization
//

#ifdef ENABLE_NLS
  #warning "What does this do?"
  #define P_(String) dgettext(GETTEXT_PACKAGE "-properties",String)
#else 
  #define P_(String) (String)
#endif

// ----------------------------------------------------------------------
// Signals
//

enum {
  SARL_LATTICE_VIEW_SELECT_CONCEPT_SIGNAL,
  SARL_LATTICE_VIEW_SELECT_ATTRIBUTE_SIGNAL,
  SARL_LATTICE_VIEW_SELECT_OBJECT_SIGNAL,
  SARL_LATTICE_VIEW_ACTIVATE_CONCEPT_SIGNAL,
  SARL_LATTICE_VIEW_ACTIVATE_OBJECT_SIGNAL,
  SARL_LATTICE_VIEW_ACTIVATE_ATTRIBUTE_SIGNAL,
  LAST_SIGNAL
};

enum {
  PROP_SLV_DUMMY_START              = 0,
  PROP_SLV_FONT_SIZE,
  PROP_SLV_FONT_DESC,
  PROP_SLV_FONT_NAME,
  PROP_SLV_CONCEPT_RADIUS,
  PROP_SLV_CONCEPT_LINE_WIDTH,
  PROP_SLV_LABEL_LINE_WIDTH,
  PROP_SLV_SELECTED_CONCEPT,
  PROP_SLV_SELECTED_OBJECT,
  PROP_SLV_SELECTED_ATTRIBUTE,
  PROP_SLV_SHOW_ATTRIBUTES,
  PROP_SLV_SHOW_OBJECTS,
  PROP_SLV_DUMMY_END
};

static guint sarl_lattice_view_signals[LAST_SIGNAL] = { 0 };

// ----------------------------------------------------------------------
// Proto
//

static void sarl_lattice_view_class_init(SarlLatticeViewClass *klass);
static void sarl_lattice_view_init(SarlLatticeView *view);

static sarl_unsigned_int 
  sarl_lattice_view_label_at(
    SarlLatticeView *view, sarl_signed_int x, sarl_signed_int y);

static void 
  sarl_lattice_view_recalc_bbox(SarlLatticeView *view);

static void
  sarl_lattice_view_recalc_transform(SarlLatticeView *view);

static void 
  sarl_lattice_view_recalc_labels(SarlLatticeView *view);

static void 
  sarl_lattice_view_recalc_font_size(SarlLatticeView *view);

static void 
  sarl_lattice_view_rescale_labels(SarlLatticeView *view);

static gboolean
sarl_lattice_view_expose_event_callback(
  GtkWidget *widget, GdkEventExpose *event, gpointer data);

static gboolean
sarl_lattice_view_configure_event_callback(
  GtkWidget *widget, GdkEventConfigure *event, gpointer data);

static gboolean
sarl_lattice_view_motion_event_callback(
  GtkWidget *widget, GdkEventMotion *event, gpointer data);

static gboolean
sarl_lattice_view_button_release_event_callback(
  GtkWidget *widget, GdkEventButton *event, gpointer data);

static gboolean
sarl_lattice_view_button_press_event_callback(
                                              GtkWidget *widget, GdkEventButton *event, gpointer data);

static void 
  sarl_lattice_view_destroy(GtkObject *view);

static void 
  sarl_lattice_view_size_request(GtkWidget *widget, GtkRequisition *req);

// ----------------------------------------------------------------------
// Class Decl
//

G_DEFINE_TYPE(SarlLatticeView, sarl_lattice_view, GTK_TYPE_DRAWING_AREA);

static void 
  sarl_lattice_view_set_font_description(
    SarlLatticeView *view, PangoFontDescription* fd
  )
{
  if ( view->p->fd ) {
    pango_font_description_free(view->p->fd);
  }
  view->p->fd = fd;
  g_object_notify(G_OBJECT(view), "font-description");
};

static void sarl_lattice_view_set_property(
  GObject *gobj, guint prop_id, const GValue *value, GParamSpec *spec
)
{
  SarlLatticeView *view = SARL_LATTICE_VIEW(gobj);
  PangoFontDescription *fd;
  
  switch(prop_id) {
  case PROP_SLV_FONT_DESC:
    fd = (PangoFontDescription *)g_value_get_boxed (value);
    fd = pango_font_description_copy(fd);
    sarl_lattice_view_set_font_description(view, fd);
    break;
  case PROP_SLV_FONT_NAME:
    fd = pango_font_description_from_string(g_value_get_string(value));
    sarl_lattice_view_set_font_description(view, fd);
    break;
  case PROP_SLV_FONT_SIZE:
    sarl_lattice_view_set_font_size(view, g_value_get_uint(value));
    break;
  case PROP_SLV_SHOW_ATTRIBUTES:
    sarl_lattice_view_set_show_attributes(view, g_value_get_boolean(value));
    break;
  case PROP_SLV_SHOW_OBJECTS:
    sarl_lattice_view_set_show_objects(view, g_value_get_boolean(value));
    break;
  case PROP_SLV_CONCEPT_RADIUS:
    view->p->concept_radius = g_value_get_uint(value);
    sarl_lattice_view_update(view);
    break;
  case PROP_SLV_CONCEPT_LINE_WIDTH:
    view->p->line_width = g_value_get_uint(value);
    sarl_lattice_view_update(view);
    break;
  case PROP_SLV_LABEL_LINE_WIDTH:
    view->p->label_line_width = g_value_get_uint(value);
    sarl_lattice_view_update(view);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobj, prop_id, spec);
  };
};

static void sarl_lattice_view_get_property(
  GObject *gobj, guint prop_id, GValue *value, GParamSpec *spec
)
{
  SarlLatticeView *view = SARL_LATTICE_VIEW(gobj);

  g_object_freeze_notify(G_OBJECT(gobj));

  switch(prop_id) {
  case PROP_SLV_FONT_NAME:
    g_value_set_string(value, "unknown");
    break;
  case PROP_SLV_FONT_DESC:
    g_value_set_boxed(value, view->p->fd);
    break;
  case PROP_SLV_FONT_SIZE:
    g_value_set_uint(value, view->p->font_size);
    break;
  case PROP_SLV_SHOW_ATTRIBUTES:
    g_value_set_boolean(value, view->p->show_attributes);
    break;
  case PROP_SLV_SHOW_OBJECTS:
    g_value_set_boolean(value, view->p->show_objects);
    break;
  case PROP_SLV_CONCEPT_RADIUS:
    g_value_set_uint(value, view->p->concept_radius);
    break;
  case PROP_SLV_CONCEPT_LINE_WIDTH:
    g_value_set_uint(value, view->p->line_width);
    break;
  case PROP_SLV_LABEL_LINE_WIDTH:
    g_value_set_uint(value, view->p->label_line_width);
    break;
  case PROP_SLV_SELECTED_CONCEPT:
    g_value_set_uint(value, view->selected_concept);
    break;
  case PROP_SLV_SELECTED_ATTRIBUTE:
    g_value_set_uint(value, view->selected_attribute);
    break;
  case PROP_SLV_SELECTED_OBJECT:
    g_value_set_uint(value, view->selected_object);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobj, prop_id, spec);
  };

  g_object_thaw_notify(G_OBJECT(gobj));
};

static void
  sarl_lattice_view_init (SarlLatticeView *view)
{
  view->p = (SarlLatticeViewPriv *)
    GC_MALLOC_UNCOLLECTABLE(sizeof(SarlLatticeViewPriv));

  view->p->diagram = 0;
  view->p->lattice_map = 0;
  view->p->labels = sarl_array_default_new();
  view->p->interps = sarl_array_default_new();
  view->p->st_objects = 0;
  view->p->st_attributes = 0;

  view->p->scale = 1;
  view->p->line_width = 50;
  view->p->label_line_width = 10;
  view->p->concept_radius = 150;
  view->p->font_size = 120;
  view->p->fd = 0;

  view->p->show_attributes = 1;
  view->p->show_objects = 1;

  view->selected_concept = 0;
  view->selected_label = 0;
  view->selected_layout_index = 0;
  view->selected_object = 0;
  view->selected_attribute = 0;
  view->hover_label = 0;
  view->hover_concept = 0;

  view->dragged_concept = 0;
  view->dragged_label = 0;

  gdk_color_set(view->line_good_color,  0,      0,        0,      0);
  gdk_color_set(view->line_bad_color,   0,      0xffff,   0,      0);
  gdk_color_set(view->concept_color,    0,      0,        0,      0);
  gdk_color_set(view->bg_color,         0,      0xffff,   0xffff, 0xffff);

  gtk_widget_modify_bg(GTK_WIDGET(view), GTK_STATE_NORMAL, &view->bg_color);
}

// ----------------------------------------------------------------------
// Class Methods
//

GtkWidget*
sarl_lattice_view_new(
  sarl_diagram_t *diagram, 
  sarl_string_table_t* st_objects,
  sarl_string_table_t* st_attributes)
{
  SarlLatticeView *view = (SarlLatticeView *)
    g_object_new (SARL_LATTICE_VIEW_TYPE, 0);

  view->p->st_objects = st_objects;
  view->p->st_attributes = st_attributes;
  view->p->diagram = diagram;

  sarl_lattice_view_recalc_bbox(view);
  sarl_lattice_view_recalc_labels(view);
  sarl_lattice_view_recalc_font_size(view);
  sarl_lattice_view_rescale_labels(view);
  sarl_lattice_view_truncate_labels(view);
  sarl_lattice_view_center_labels(view);

  GtkWidget *widget = GTK_WIDGET(view);

  view->p->fd = pango_font_description_from_string("Sans 12");

  g_signal_connect(G_OBJECT(view), "expose_event",  
    G_CALLBACK (sarl_lattice_view_expose_event_callback), 0);

  g_signal_connect(G_OBJECT(view), "configure_event",  
    G_CALLBACK (sarl_lattice_view_configure_event_callback), 0);

  g_signal_connect(G_OBJECT(view), "motion_notify_event",
    G_CALLBACK (sarl_lattice_view_motion_event_callback), 0);

  g_signal_connect(GTK_OBJECT(view), "button_press_event",
    G_CALLBACK (sarl_lattice_view_button_press_event_callback), 0);

  g_signal_connect(GTK_OBJECT(view), "button_release_event",
    G_CALLBACK (sarl_lattice_view_button_release_event_callback), 0);

  gtk_widget_set_events(GTK_WIDGET(view), 
    GDK_EXPOSURE_MASK       | GDK_LEAVE_NOTIFY_MASK         | 
    GDK_BUTTON_PRESS_MASK   | GDK_BUTTON_RELEASE_MASK       |
    GDK_POINTER_MOTION_MASK | GDK_BUTTON1_MOTION_MASK       | 
    GDK_POINTER_MOTION_HINT_MASK
  );

  return widget;
}

void sarl_lattice_view_destroy(GtkObject *object)
{
  SarlLatticeView *view = SARL_LATTICE_VIEW(object);

  // FIXME: find some way to call the parent class destroy method

  view->selected_concept = 0;
  view->selected_label = 0;
  view->selected_layout_index = 0;
  view->selected_object = 0;
  view->selected_attribute = 0;
  view->hover_label = 0;
  view->hover_concept = 0;

  view->dragged_concept = 0;
  view->dragged_label = 0;

  if ( view->p && view->p->fd ) {
    pango_font_description_free(view->p->fd);
    view->p->fd = 0;
  }

  if ( view->p ) {
    GC_FREE(view->p);
    view->p = 0;
  }

};

void sarl_lattice_view_dispose_labels(sarl_array_t *labels)
{
  sarl_label_t* label = 0;
  SARL_ARRAY_FOR_EACH(sarl_label_t, label, labels);
    sarl_label_dispose(label);
  SARL_ARRAY_END;
  sarl_array_clear(labels);
};

void sarl_lattice_view_recalc_labels_for(
  SarlLatticeView *view, SarlLabelKind kind,
  sarl_power_map_t* cont_map, sarl_string_table_t* st, 
  sarl_signed_int ox, sarl_signed_int oy)
{
  sarl_label_t*       label;
  sarl_bit_set_t*     cont;
  sarl_point_t*       p;
  sarl_unsigned_int   ob;
  sarl_unsigned_int   index;

  GtkWidget *widget = GTK_WIDGET(view);

  SARL_POWER_MAP_FOR_EACH(index, cont, cont_map);
    if ( sarl_bit_set_count(cont) > 0 ) {
      p = sarl_diagram_get_concept_pos(view->p->diagram, index);
      label = sarl_label_new(index, kind, ox, oy);
      sarl_array_push(view->p->labels, label);
      SARL_BIT_SET_FOR_EACH(ob, cont);
      sarl_array_push(label->layouts, 
        gtk_widget_create_pango_layout(widget, 
          sarl_string_table_get(st, ob)
      ));
      label->before = gtk_widget_create_pango_layout(widget, "... %d before");
      label->after = gtk_widget_create_pango_layout(widget, "... %d after");
      SARL_BIT_SET_END;
    }
  SARL_POWER_MAP_END;
};

void sarl_lattice_view_recalc_labels(SarlLatticeView *view)
{
  sarl_lattice_view_dispose_labels(view->p->labels);

  if ( view->p->lattice_map == 0 ) {
    sarl_lattice_view_recalc_labels_for(
      view, SARL_LABEL_OBJECT_KIND, 
      view->p->diagram->lattice->concept_object_cont, 
      view->p->st_objects, 0, view->p->concept_radius
    );
    sarl_lattice_view_recalc_labels_for(
      view, SARL_LABEL_ATTRIBUTE_KIND,
      view->p->diagram->lattice->concept_attribute_cont, 
      view->p->st_attributes, 0, -view->p->concept_radius
    );
  }
  else {
    // need to document what happens when the lattice draws an embedding
    sarl_not_implemented_error;
  };
};

void
sarl_lattice_view_clear(SarlLatticeView *view)
{
  sarl_lattice_view_dispose_labels(view->p->labels);
  sarl_array_clear(view->p->interps);

  view->p->diagram = 0;
  view->p->st_objects = 0;
  view->p->st_attributes = 0;

  view->selected_concept = 0;
  view->selected_label = 0;
  view->selected_layout_index = 0;
  view->selected_object = 0;
  view->selected_attribute = 0;
  view->hover_label = 0;
  view->hover_concept = 0;

  view->dragged_concept = 0;
  view->dragged_label = 0;
}

void sarl_lattice_view_print_bbox(char *s, sarl_rect_t* bbox) {
  fprintf(stderr, "  %s: %d, %d -> %d, %d\n", s, 
          bbox->left, bbox->top, bbox->width, bbox->height);
};

void sarl_lattice_view_set_diagram(
  SarlLatticeView *view, sarl_diagram_t *diagram,
  sarl_string_table_t* objects, sarl_string_table_t *attributes)
{
  sarl_lattice_view_clear(view);

  view->p->diagram = diagram;
  view->p->st_objects = objects;
  view->p->st_attributes = attributes;

  sarl_lattice_view_recalc_bbox(view);
  sarl_lattice_view_recalc_transform(view);
  sarl_lattice_view_recalc_labels(view);
  sarl_lattice_view_recalc_font_size(view);
  sarl_lattice_view_rescale_labels(view);
  sarl_lattice_view_truncate_labels(view);
  sarl_lattice_view_center_labels(view);
  sarl_lattice_view_update(view);
};

void sarl_lattice_view_set_embedding(
  SarlLatticeView *view, sarl_diagram_t *diagram, 
  sarl_string_table_t* objects, sarl_string_table_t *attributes, 
  sarl_lattice_map_t *lattice_map)
{
  sarl_lattice_view_clear(view);

  view->p->diagram = diagram;
  view->p->st_objects = objects;
  view->p->st_attributes = attributes;
  view->p->lattice_map = lattice_map;

  sarl_lattice_view_recalc_bbox(view);
  sarl_lattice_view_recalc_transform(view);
  sarl_lattice_view_recalc_labels(view);
  sarl_lattice_view_recalc_font_size(view);
  sarl_lattice_view_rescale_labels(view);
  sarl_lattice_view_truncate_labels(view);
  sarl_lattice_view_center_labels(view);
  sarl_lattice_view_update(view);
};

void
sarl_lattice_view_recalc_transform(SarlLatticeView *view)
{
  GtkWidget *widget = GTK_WIDGET(view);

  /* calculate a transform to contain the items */
  double box_width = view->p->bbox.width;
  double box_height = view->p->bbox.height;

  double widget_width  = widget->allocation.width;
  double widget_height = widget->allocation.height;

  if ( widget_width <= 1 && widget_height <= 1 ) {
    // avoid the case then the allocation is too small
    widget_width = 500;
    widget_height = 500;
  }
  
  double sx = widget_width / box_width;
  double sy = widget_height / box_height;

  view->p->scale = 0.8 * (sx < sy ? sx : sy);
  view->p->offset.x = (int)(
    (widget_width - (view->p->bbox.width * view->p->scale))/(2*view->p->scale) - 
    view->p->bbox.left);
  view->p->offset.y = (int)(
    (widget_height - (view->p->bbox.height * view->p->scale))/(2*view->p->scale) - 
    view->p->bbox.top);
}

void sarl_lattice_view_recalc_bbox(SarlLatticeView *view)
{
  sarl_rect_t bbox;
  sarl_unsigned_int concept_index;
  sarl_rect_clear(&bbox);
  sarl_unsigned_int is_first = 1;
  SARL_LATTICE_FOR_EACH(concept_index, view->p->diagram->lattice);
    sarl_point_t* concept_pos = 
      sarl_diagram_get_concept_pos(view->p->diagram, concept_index);
    if ( is_first ) {
      bbox.left = concept_pos->x;
      bbox.top = concept_pos->y;
      bbox.width = 0;
      bbox.height = 0;
      is_first = 0;
    }
    else {
      sarl_rect_insert_point(&bbox, concept_pos);
    }
  SARL_LATTICE_END;
  bbox.top -= view->p->concept_radius * 4;
  bbox.height += view->p->concept_radius * 8;

  view->p->bbox = bbox;
  
};

void sarl_lattice_view_draw_concepts(SarlLatticeView *view, GdkDrawable *dest, GdkGC *gc)
{
  /* init for drawing */
  int scaled_line_width = (int)(view->p->line_width * view->p->scale);
  if ( scaled_line_width < 1 ) scaled_line_width = 1;
  
  gdk_gc_set_line_attributes(
    gc, scaled_line_width, GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_MITER);
  gdk_gc_set_rgb_fg_color(gc, &view->concept_color);

  /* draw the concepts */
  sarl_unsigned_int concept_index;
  SARL_LATTICE_FOR_EACH(concept_index, view->p->diagram->lattice);
    sarl_point_t* concept_pos = 
      sarl_diagram_get_concept_pos(view->p->diagram, concept_index);
    gdk_draw_arc(dest, gc, TRUE, 
      TR_X(concept_pos->x - view->p->concept_radius), 
      TR_Y(concept_pos->y - view->p->concept_radius), 
      (int)(view->p->concept_radius * 2 * view->p->scale), 
      (int)(view->p->concept_radius * 2 * view->p->scale), 
      0, 64*360
    );
  SARL_LATTICE_END;
};

void sarl_lattice_view_draw_lines(SarlLatticeView *view, GdkDrawable *dest, GdkGC *gc)
{
  /* init for drawing */
  int scaled_line_width = (int)(view->p->line_width * view->p->scale);
  if ( scaled_line_width < 1 ) scaled_line_width = 1;

  gdk_gc_set_line_attributes(
    gc, scaled_line_width, GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_MITER);

  /* draw the lines in the covering relation */
  sarl_unsigned_int parent_index, child_index;
  sarl_bit_set_t* children;
  SARL_POWER_MAP_FOR_EACH(
    parent_index, children, view->p->diagram->lattice->lower_covers
  );
    sarl_point_t* parent_pos = 
      sarl_diagram_get_concept_pos(view->p->diagram, parent_index);
    SARL_BIT_SET_FOR_EACH(child_index, children);
      sarl_point_t* child_pos = 
        sarl_diagram_get_concept_pos(view->p->diagram, child_index);
      if ( TR_Y(child_pos->y) > TR_Y(parent_pos->y) ) {
        gdk_gc_set_rgb_fg_color(gc, &view->line_good_color);
      }
      else {
        gdk_gc_set_rgb_fg_color(gc, &view->line_bad_color);
      }
      gdk_draw_line(
        dest,     gc, 
        TR_X(child_pos->x),  TR_Y(child_pos->y), 
        TR_X(parent_pos->x), TR_Y(parent_pos->y)
      );
    SARL_BIT_SET_END;
  SARL_POWER_MAP_END;
};

void sarl_lattice_view_recalc_font_size(SarlLatticeView *view)
{
  if ( view->p->fd == 0 ) {
    view->p->fd = pango_font_description_from_string("Sans 12");
  }
  pango_font_description_set_size(view->p->fd, 
    (int)(PANGO_SCALE * view->p->font_size * view->p->scale));
};

void sarl_lattice_view_rescale_labels(SarlLatticeView *view) 
{
  if ( view->p->labels ) {
    sarl_label_t *label = 0;
    SARL_ARRAY_FOR_EACH(sarl_label_t, label, view->p->labels);
      sarl_label_rescale(label, view->p->fd, view->p->font_size, view->p->scale);
    SARL_ARRAY_END;
  }
};

void sarl_lattice_view_draw_labels(
  SarlLatticeView *view, GdkDrawable *dest, GdkGC *gc)
{
  GtkWidget         *widget = GTK_WIDGET(view);
  sarl_label_t      *label = 0;
  sarl_bit_set_t    *cont = 0;
  sarl_power_map_t  *cont_map = view->p->diagram->lattice->concept_object_cont;
  sarl_point_t      *p = 0;
  sarl_unsigned_int label_index;

  int               do_draw = 1;

  int scaled_line_width = (int)(view->p->label_line_width * view->p->scale);
  if ( scaled_line_width < 1 ) scaled_line_width = 1;
  
  gdk_gc_set_line_attributes(
    gc, scaled_line_width, GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_MITER);

  SARL_ARRAY_FOR_EACH_INDEX(label_index, sarl_label_t, label, view->p->labels);
    cont = sarl_power_map_get(cont_map, label->concept);
    p = sarl_diagram_get_concept_pos(view->p->diagram, label->concept);
    switch (label->kind) {
    case SARL_LABEL_OBJECT_KIND:
      do_draw = view->p->show_objects;
      if (label->bbox.top + (label->bbox.height/2) > 0) {
        gdk_gc_set_rgb_fg_color(gc, &view->line_good_color);
      }
      else {
        gdk_gc_set_rgb_fg_color(gc, &view->line_bad_color);
      };
      break;
    case SARL_LABEL_ATTRIBUTE_KIND:
      do_draw = view->p->show_attributes;
      if (label->bbox.top + (label->bbox.height/2) < 0) {
        gdk_gc_set_rgb_fg_color(gc, &view->line_good_color);
      }
      else {
        gdk_gc_set_rgb_fg_color(gc, &view->line_bad_color);
      };
      break;
    }
    if ( do_draw ) {
      gdk_draw_line(
        dest, gc, 
        TR_X(p->x), TR_Y(p->y), 
        TR_X(p->x + label->bbox.left + (label->bbox.width/2)), 
        TR_Y(p->y + label->bbox.top + (label->bbox.height/2))
      );
    }
  SARL_ARRAY_END;

  SARL_ARRAY_FOR_EACH_INDEX(label_index, sarl_label_t, label, view->p->labels);
    cont = sarl_power_map_get(cont_map, label->concept);
    p = sarl_diagram_get_concept_pos(view->p->diagram, label->concept);
    switch (label->kind) {
    case SARL_LABEL_OBJECT_KIND:
      do_draw = view->p->show_objects;
      break;
    case SARL_LABEL_ATTRIBUTE_KIND:
      do_draw = view->p->show_attributes;
      break;
    }
    if ( do_draw ) {
      if ( label_index == view->hover_label ) {
        sarl_label_draw_background_with_scrollbar(
          view, dest, gc, label, p->x, p->y, &view->p->offset, view->p->scale
        );
      }
      else {
        sarl_label_draw_background(
          view, dest, gc, label, p->x, p->y, &view->p->offset, view->p->scale
        );
      };
      sarl_label_draw(
        widget, dest, gc, label, p->x, p->y, &view->p->offset, view->p->scale
      );
    }
  SARL_ARRAY_END;
};

static gboolean
sarl_lattice_view_expose_event_callback(
  GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
  SarlLatticeView *view = SARL_LATTICE_VIEW(widget);

  /* setup the GC */
  GdkDrawable *dest = GDK_DRAWABLE(widget->window);
  GdkGC* gc = gdk_gc_new(dest);
  gdk_gc_copy(gc, GET_FG_GC(widget));

  sarl_lattice_view_draw_lines(view, dest, gc);
  sarl_lattice_view_draw_concepts(view, dest, gc);
  sarl_lattice_view_draw_labels(view, dest, gc);

  g_object_unref(gc);

  return FALSE;
};

static gboolean
sarl_lattice_view_configure_event_callback(
  GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
  SarlLatticeView* view = SARL_LATTICE_VIEW(widget);
  sarl_lattice_view_recalc_transform(view);
  sarl_lattice_view_recalc_font_size(view);
  sarl_lattice_view_rescale_labels(view);

  return TRUE;
};

static gboolean
sarl_lattice_view_motion_event_callback(
  GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
  SarlLatticeView *view = SARL_LATTICE_VIEW(widget);

  int x = -1, y = -1, dx, dy;
  GdkModifierType state;

  guint gdk_button_mask = 
    GDK_BUTTON1_MASK | GDK_BUTTON2_MASK | GDK_BUTTON3_MASK;

  if ( ! (event->state & gdk_button_mask) ) {

    gdk_window_get_pointer(event->window, &x, &y, &state);
    
    dx = x - view->prev_pt.x;
    dy = y - view->prev_pt.y;

    sarl_unsigned_int prev_hover_label = view->hover_label;
    view->hover_label = sarl_lattice_view_label_at(view, ITR_X(x), ITR_Y(y));

    if ( view->hover_label != prev_hover_label ) {

      if ( prev_hover_label ) {
        // FIXME: selected labels need to respect changes in the interp
        sarl_label_set_active(
          (sarl_label_t *)sarl_array_get(view->p->labels, prev_hover_label), 
          0
        );
      }

      if ( view->hover_label ) {
        // FIXME: selected labels need to respect changes in the interp
        sarl_label_set_active(
          (sarl_label_t *)sarl_array_get(view->p->labels, view->hover_label), 1);
      }

      sarl_lattice_view_update(view);
    }
  }

  if ( (event->state & GDK_BUTTON1_MASK) && (view->dragged_concept) ) {
    gdk_window_get_pointer(event->window, &x, &y, &state);

    dx = x - view->prev_pt.x;
    dy = y - view->prev_pt.y;
  
    sarl_diagram_move_concept_by(
      view->p->diagram, view->dragged_concept, 
      (int)(dx / view->p->scale), (int)(dy / view->p->scale)
    );
    view->prev_pt.y = y;
    view->prev_pt.x = x;
    sarl_lattice_view_update(view);
  }
    
  if ( (event->state & GDK_BUTTON1_MASK) && (view->dragged_label) ) {
    gdk_window_get_pointer(event->window, &x, &y, &state);

    dx = x - view->prev_pt.x;
    dy = y - view->prev_pt.y;

    // FIXME: selected labels need to respect changes in the interp
    sarl_label_t* label = 
      (sarl_label_t*)sarl_array_get(view->p->labels, view->dragged_label);
    sarl_point_t* p = 
      sarl_diagram_get_concept_pos(view->p->diagram, label->concept);

    switch ( view->label_region ) {

    case SARL_LABEL_REGION_CONTENTS:
      sarl_label_move_by(
        label, (int)(dx / view->p->scale), (int)(dy / view->p->scale));
      break;
    case SARL_LABEL_REGION_RESIZE:
      sarl_label_resize_to(label, ITR_X(x) - p->x, ITR_Y(y) - p->y, view->p->font_size);
      break;
    default:
      break;
    }
    
    view->prev_pt.y = y;
    view->prev_pt.x = x;
    sarl_lattice_view_update(view);
  };

  return TRUE;
};

void sarl_lattice_view_update(SarlLatticeView *view)
{
  GtkWidget *widget = GTK_WIDGET(view);
  gtk_widget_queue_draw_area(widget, 0, 0, 
    widget->allocation.width, widget->allocation.height
  );
};

sarl_unsigned_int sarl_lattice_view_concept_at(
  SarlLatticeView *view,                                               
  sarl_signed_int x,
  sarl_signed_int y
)
{
  sarl_unsigned_int index;
  sarl_point_t* p;

  for(index=view->p->diagram->lattice->num_concepts; index>0; --index) {
    p = sarl_diagram_get_concept_pos(view->p->diagram, index);
    if ( 
        abs(p->x - x) < view->p->concept_radius && 
        abs(p->y - y) < view->p->concept_radius
    ) {
      return index;
    }
  }
  return 0;
};

/* \arg{x,y} coorindates in the diagram space  
   \return 0 if no label is at x,y otherwise the label number is returned.
*/


sarl_unsigned_int sarl_lattice_view_label_at(
  SarlLatticeView *view,                                               
  sarl_signed_int x,
  sarl_signed_int y
)
{
  sarl_label_t* label;
  sarl_point_t  p;
  sarl_point_t* cp = 0;
  sarl_unsigned_int index;

  // FIXME: selected labels need to respect changes in the interp
  for(index=view->p->labels->length; index>0; --index) {
    if (view->p->labels->table[index-1]) {
      label = (sarl_label_t *)view->p->labels->table[index-1];
      cp = sarl_diagram_get_concept_pos(view->p->diagram, label->concept);
      p.x = x - cp->x;
      p.y = y - cp->y;
      if ( sarl_rect_includes(&label->bbox, &p) ) {
        return index;
      }
    }
  }

  return 0;
};

void sarl_lattice_view_center_labels(SarlLatticeView *view)
{
  sarl_label_t* label;
  SARL_ARRAY_FOR_EACH(sarl_label_t, label, view->p->labels);
    if (label->kind == SARL_LABEL_ATTRIBUTE_KIND) {
      sarl_rect_center_top_left_on(&label->rect, 
        label->margin - (label->bbox.width/2), 
        - (view->p->concept_radius + label->bbox.height - label->margin));
      sarl_label_update_bbox(label, view->p->font_size);
    }
    else if (label->kind == SARL_LABEL_OBJECT_KIND) {
      sarl_rect_center_top_left_on(&label->rect, 
        label->margin - (label->bbox.width/2), 
        view->p->concept_radius + label->margin);
      sarl_label_update_bbox(label, view->p->font_size);
    }
  SARL_ARRAY_END;
};                                    

void sarl_lattice_view_truncate_labels(SarlLatticeView *view)
{
  sarl_label_t* label;
  SARL_ARRAY_FOR_EACH(sarl_label_t, label, view->p->labels);
    sarl_label_size_to_three_lines(label, view->p->scale, view->p->font_size);
  SARL_ARRAY_END;
};                                    
  
static void sarl_lattice_label_contents_hit(
 SarlLatticeView *view, GdkEventButton *event, 
 sarl_label_t * label, sarl_signed_int cx, sarl_signed_int cy
) 
{
	sarl_unsigned_int prev_selected_object, prev_selected_attribute;
  view->selected_layout_index = 
    sarl_label_layout_index_at(label, cx, cy, view->p->scale);
  fprintf(stderr, "Layout Index: %u\n", 
    (sarl_unsigned_int)view->selected_layout_index
  );
  if ( view->selected_layout_index ) {
    switch(label->kind) {
    case SARL_LABEL_OBJECT_KIND:
      prev_selected_object = view->selected_object;
      view->selected_object = 
        sarl_bit_set_nth(
          sarl_power_map_get(
            view->p->diagram->lattice->concept_object_cont,
            label->concept
          ),
          view->selected_layout_index
        );
      if ( view->selected_object ) {
        g_signal_emit(
          G_OBJECT(view), 
          sarl_lattice_view_signals[SARL_LATTICE_VIEW_SELECT_OBJECT_SIGNAL], 
          0, view->selected_object, event
        );
      };
      if ( prev_selected_object != view->selected_object ) {
        g_object_notify(G_OBJECT(view), "selected-object");
      }
      break;
    case SARL_LABEL_ATTRIBUTE_KIND:
      prev_selected_attribute = view->selected_attribute;
      view->selected_attribute = 
        sarl_bit_set_nth(
          sarl_power_map_get(
            view->p->diagram->lattice->concept_attribute_cont,
            label->concept
          ),
          view->selected_layout_index
        );
      if ( view->selected_attribute ) {
        g_signal_emit(
          G_OBJECT(view), 
          sarl_lattice_view_signals[SARL_LATTICE_VIEW_SELECT_ATTRIBUTE_SIGNAL], 
          0, view->selected_attribute, event
        );
      };
      if ( prev_selected_attribute != view->selected_attribute ) {
        g_object_notify(G_OBJECT(view), "selected-attribute");
      }
      break;
    }
  };
}

static gboolean
sarl_lattice_view_button_press_event_callback(
  GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  SarlLatticeView *view = SARL_LATTICE_VIEW(widget);
  sarl_unsigned_int prev_selected_concept;
  
  view->prev_pt.x    = (int)event->x;
  view->prev_pt.y    = (int)event->y;

  sarl_signed_int cx = ITR_X(event->x);
  sarl_signed_int cy = ITR_Y(event->y);

  // see if we hit a concept
  prev_selected_concept = view->selected_concept; 
  view->selected_concept = sarl_lattice_view_concept_at(view, cx, cy);
  view->dragged_concept = view->selected_concept;
  if ( view->selected_concept ) {
    g_signal_emit(
      G_OBJECT(view), 
      sarl_lattice_view_signals[SARL_LATTICE_VIEW_SELECT_CONCEPT_SIGNAL], 
      0, view->selected_concept, event
    );
  }
  if ( view->selected_concept != prev_selected_concept ) {
    g_object_notify(G_OBJECT(view), "selected-concept");
  }

  // see if we hit a label
  if ( view->selected_concept == 0 ) {
    view->selected_label  = 
      sarl_lattice_view_label_at(view, cx, cy);
    view->dragged_label = view->selected_label;

    // see find out which line was hit
    if ( view->selected_label != 0 ) {
      sarl_label_t* label = 
        (sarl_label_t *)sarl_array_get(view->p->labels, view->selected_label);

      // see what we hit in the label
      // FIXMENOW region_type = sarl_label_region_at(label, cx, cy);
      sarl_point_t* p = 
        sarl_diagram_get_concept_pos(view->p->diagram, label->concept);
      switch ( 
        view->label_region = sarl_label_region_at(label, cx - p->x, cy - p->y) 
      ) {
      case SARL_LABEL_REGION_CONTENTS:
        fprintf(stderr, "Contents.\n");
        sarl_lattice_label_contents_hit(view, event, label, cx - p->x, cy - p->y);
        break;
      case SARL_LABEL_REGION_UP_ARROW:
        sarl_label_scroll_up(label);
        sarl_lattice_view_update(view);
        fprintf(stderr, "Up.\n");
        break;
      case SARL_LABEL_REGION_DOWN_ARROW:
        sarl_label_scroll_down(label);
        sarl_lattice_view_update(view);
        fprintf(stderr, "Down.\n");
        break;
      case SARL_LABEL_REGION_RESIZE:
        fprintf(stderr, "Resize.\n");
        break;
      case SARL_LABEL_REGION_OUTSIDE:
        fprintf(stderr, "Outside.\n");
        break;
      case SARL_LABEL_REGION_SCROLL:
        fprintf(stderr, "Scroll.\n");
        break;
      }
    }
  }

  return TRUE;
};

static gboolean
sarl_lattice_view_button_release_event_callback(
  GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  SarlLatticeView *view = SARL_LATTICE_VIEW(widget);
  
  sarl_lattice_view_recalc_bbox(view);
  sarl_lattice_view_recalc_transform(view);
  sarl_lattice_view_recalc_font_size(view);
  sarl_lattice_view_rescale_labels(view);

  view->dragged_concept = 0;
  view->dragged_label = 0;
  view->selected_layout_index = 0;
  view->selected_object = 0;

  sarl_lattice_view_update(view);

  return TRUE;
};

static void sarl_lattice_view_size_request(
    GtkWidget *widget, GtkRequisition *req
)
{
  req->width =  400;	// Default value
  req->height = 400;	// Default value
}

void sarl_lattice_view_dump_to(SarlLatticeView *view, FILE *file)
{
  sarl_label_t *label;

  fprintf(file, "Dumping SarlLatticeView:\n"); 
  SARL_ARRAY_FOR_EACH(sarl_label_t, label, view->p->labels);
  fprintf(file, " label=%p, layouts=%p\n", label, label->layouts);
  SARL_ARRAY_END;
};

void sarl_lattice_view_set_font_size(SarlLatticeView *view, gint size)
{
  view->p->font_size = size;
  // g_object_notify(G_OBJECT(view), "font-size");

  sarl_lattice_view_recalc_font_size(view);
  sarl_lattice_view_rescale_labels(view);
  sarl_lattice_view_update(view);
}

void sarl_lattice_view_set_show_objects(
  SarlLatticeView *view, gboolean is_shown)
{
  view->p->show_objects = is_shown;
  fprintf(stderr, "Show objects val=%d\n", (is_shown ? 1 : 0));
  g_object_notify(G_OBJECT(view), "show-objects");
  sarl_lattice_view_update(view);
}

void sarl_lattice_view_set_show_attributes(
  SarlLatticeView *view, gboolean is_shown)
{
  view->p->show_attributes = is_shown;
  g_object_notify(G_OBJECT(view), "show-attributes");
  sarl_lattice_view_update(view);
}

gint sarl_lattice_view_get_font_size(SarlLatticeView *view)
{
  return view->p->font_size;
};

static void
sarl_lattice_view_class_init (SarlLatticeViewClass *klass)
{
  GObjectClass   *gobject_class  = G_OBJECT_CLASS(klass);
  GtkObjectClass *object_class   = GTK_OBJECT_CLASS(klass);
  GtkWidgetClass *widget_class   = GTK_WIDGET_CLASS(klass);

  //
  // Methods inherrited from GObject.
  //

  // widget_class->realize = sarl_lattice_view_realize;
  widget_class->size_request = sarl_lattice_view_size_request;
  object_class->destroy = sarl_lattice_view_destroy;
  
  gobject_class->set_property = sarl_lattice_view_set_property;
  gobject_class->get_property = sarl_lattice_view_get_property;

  //
  // Properties.
  //

  #define SARL_INSTALL_INT_PROPERY(p, pl, pt, pd, iv) \
    g_object_class_install_property( \
      gobject_class, p, \
      g_param_spec_uint( \
        pl, P_(pt), P_(pd), 0, UINT_MAX, iv, \
        (GParamFlags)G_PARAM_READWRITE \
      ) \
    );

  #define SARL_INSTALL_BOOL_PROPERY(p, pl, pt, pd, iv) \
    g_object_class_install_property( \
      gobject_class, p, \
      g_param_spec_boolean( \
        pl, P_(pt), P_(pd), iv, \
        (GParamFlags)G_PARAM_READWRITE \
      ) \
    );

  #define SARL_INSTALL_INT_PROPERY_RO(p, pl, pt, pd, iv) \
    g_object_class_install_property( \
      gobject_class, p, \
      g_param_spec_uint( \
        pl,P_(pt), P_(pd), 0, UINT_MAX, iv, \
        (GParamFlags)G_PARAM_READABLE \
      ) \
    );

  SARL_INSTALL_INT_PROPERY_RO(
    PROP_SLV_SELECTED_CONCEPT, "selected-concept", 
    "selected concept", "The concept which is currently selected.",
    0
  );

  SARL_INSTALL_INT_PROPERY_RO(
    PROP_SLV_SELECTED_ATTRIBUTE, "selected-attribute", 
    "selected attribute", "The attribute which is currently selected.",
    0
  );

  SARL_INSTALL_INT_PROPERY_RO(
    PROP_SLV_SELECTED_OBJECT, "selected-object", 
    "selected object", "The object which is currently selected.",
    0
  );

  SARL_INSTALL_INT_PROPERY(
    PROP_SLV_CONCEPT_LINE_WIDTH, "concept-line-width", 
    "concept line width", "Line width of concept-concept lines.",
    150
  );

  SARL_INSTALL_INT_PROPERY(
    PROP_SLV_CONCEPT_RADIUS, "concept-radius", 
    "concept radius", "Radius of concepts in the diagram.",
    150
  );

  SARL_INSTALL_INT_PROPERY(
    PROP_SLV_LABEL_LINE_WIDTH, "label-line-width", 
    "label line width", "Line width of concept-label lines.",
    150
  );

  SARL_INSTALL_INT_PROPERY(
    PROP_SLV_FONT_SIZE, "font-size", 
    "font size", "Size of the fonts used in labels.",
    150
  );

  SARL_INSTALL_BOOL_PROPERY(
    PROP_SLV_SHOW_OBJECTS, "show-objects", 
    "show objects", "Whether or not the object labels are displayed.",
    TRUE
  );

  SARL_INSTALL_BOOL_PROPERY(
    PROP_SLV_SHOW_ATTRIBUTES, "show-attributes", 
    "show attributes", "Whether or not the attribute labels are displayed.",
    TRUE
  );

  g_object_class_install_property(
    gobject_class,
    PROP_SLV_FONT_DESC,
    g_param_spec_boxed(
      "font-description", P_("Font Description"),
      P_("Font description as a PangoFontDescription struct"),
      PANGO_TYPE_FONT_DESCRIPTION,
      (GParamFlags)G_PARAM_READWRITE
    )
  );

  g_object_class_install_property(
    gobject_class,
    PROP_SLV_FONT_NAME,
    g_param_spec_string(
      "font-name",P_("Font Name"), 
     P_("name of the font used to render labels."),
      "Sans", (GParamFlags)G_PARAM_READWRITE
    )
  );

  //
  // Signals
  //

  #define SARL_REGISTER_SIMPLE_SIGNAL(se, sn, st, sc)  \
    sarl_lattice_view_signals[se] = \
      g_signal_new(sn, st, G_SIGNAL_RUN_LAST, 0, \
        0, 0, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0 \
      ); 

  #define SARL_REGISTER_GUINT_SIGNAL(se, sn, st, sc)  \
    sarl_lattice_view_signals[se] = \
      g_signal_new(sn, st, G_SIGNAL_RUN_LAST, 0, \
        0, 0, g_cclosure_marshal_VOID__UINT, G_TYPE_NONE, \
        1, G_TYPE_UINT \
      ); 

  #define SARL_REGISTER_2_GUINT_SIGNAL(se, sn, st, sc)  \
    sarl_lattice_view_signals[se] = \
      g_signal_new(sn, st, G_SIGNAL_RUN_LAST, 0, \
        0, 0, g_cclosure_user_marshal_VOID__UINT_BOXED, G_TYPE_NONE, \
        2, G_TYPE_UINT, GDK_TYPE_EVENT | G_SIGNAL_TYPE_STATIC_SCOPE \
      ); 

  SARL_REGISTER_2_GUINT_SIGNAL(
    SARL_LATTICE_VIEW_SELECT_CONCEPT_SIGNAL, "select-concept",
    SARL_LATTICE_VIEW_TYPE, SarlLatticeViewClass
  );
  SARL_REGISTER_2_GUINT_SIGNAL(
    SARL_LATTICE_VIEW_SELECT_OBJECT_SIGNAL, "select-object",
    SARL_LATTICE_VIEW_TYPE, SarlLatticeViewClass
  );
  SARL_REGISTER_2_GUINT_SIGNAL(
    SARL_LATTICE_VIEW_SELECT_ATTRIBUTE_SIGNAL, "select-attribute",
    SARL_LATTICE_VIEW_TYPE, SarlLatticeViewClass
  );
}

void sarl_lattice_view_save_image(SarlLatticeView *view, char *filename, char *format)
{
  gint depth, width, height;
  GdkDrawable *window = GTK_WIDGET(view)->window;
  depth = gdk_drawable_get_depth(window);
  gdk_drawable_get_size(window, &width, &height);

  GdkPixmap *pixmap = gdk_pixmap_new(0, width, height, depth);
  GdkDrawable *pixmap_drawable = GDK_DRAWABLE(pixmap);
  GdkGC* gc = gdk_gc_new(pixmap_drawable);
  gdk_gc_copy(gc, GET_FG_GC(GTK_WIDGET(view)));

  gdk_draw_rectangle(pixmap_drawable, GET_WT_GC(GTK_WIDGET(view)), 
    TRUE, 0, 0, width, height);

  sarl_lattice_view_draw_lines(view, pixmap_drawable, gc);
  sarl_lattice_view_draw_concepts(view, pixmap_drawable, gc);
  sarl_lattice_view_draw_labels(view, pixmap_drawable, gc);

  gdk_gc_unref(gc);

  GdkColormap *cmap = gdk_drawable_get_colormap(pixmap_drawable); 
  GdkPixbuf* pixbuf = gdk_pixbuf_get_from_drawable(
    0, pixmap_drawable, cmap, 0, 0, 0, 0, width, height
  );
  GError *error = 0;
  gdk_pixbuf_save(pixbuf, filename, format, &error, 0);
  
  gdk_colormap_unref(cmap);
  gdk_pixbuf_unref(pixbuf);
  gdk_pixmap_unref(pixmap);
  
};

void sarl_lattice_view_update_labels(SarlLatticeView *view)
{
  sarl_lattice_view_recalc_labels(view);
  sarl_lattice_view_recalc_font_size(view);
  sarl_lattice_view_rescale_labels(view);
  sarl_lattice_view_truncate_labels(view);
  sarl_lattice_view_center_labels(view);
  sarl_lattice_view_update(view);
};
