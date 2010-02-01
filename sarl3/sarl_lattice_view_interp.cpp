#include "sarl_lattice_view_interp.h"
#include "sarl_lattice_view_gtk.h"
#include "sarl_label.h"
#include "sarl_array.h"
#include "sarl_power_map.h"
#include "sarl_string_table.h"

#include "sarl_lattice_view_gtk_privates.h"
#include "sarl_lattice_view_macros.h"

void sarl_lattice_view_interp_dispose(
  sarl_lattice_view_interp_t *interp
)
{
  sarl_label_t* label = 0;
  SARL_ARRAY_FOR_EACH(sarl_label_t, label, interp->labels);
    sarl_label_dispose(label);
  SARL_ARRAY_END;
  sarl_array_clear(interp->labels);
};


void sarl_lattice_view_interp_recalc_labels_for(
  SarlLatticeView                       *view, 
  sarl_lattice_view_interp_t            *view_interp,
  sarl_contingent_interp_t              *cont_interp,
  SarlLabelKind                          kind,
  sarl_signed_int                        ox, 
  sarl_signed_int                        oy
)
{
  sarl_power_map_t   *cont_map = cont_interp->contingents;
  sarl_bit_set_t     *cont;

  sarl_label_t*       label;
  sarl_point_t*       p;
  sarl_unsigned_int   ob, concept;

  GtkWidget *widget = GTK_WIDGET(view);

  SARL_POWER_MAP_FOR_EACH(concept, cont, cont_map);
    if ( sarl_bit_set_count(cont) > 0 ) {
      p = sarl_diagram_get_concept_pos(view->p->diagram, concept);
      label = sarl_label_new(concept, kind, ox, oy);
      sarl_array_push(view_interp->labels, label);
      SARL_BIT_SET_FOR_EACH(ob, cont);
      sarl_array_push(label->layouts, 
        gtk_widget_create_pango_layout(widget, 
          sarl_string_table_get(cont_interp->names, ob)
      ));
      label->before = gtk_widget_create_pango_layout(widget, "... %d before");
      label->after = gtk_widget_create_pango_layout(widget, "... %d after");
      SARL_BIT_SET_END;
    }
  SARL_POWER_MAP_END;
};


void sarl_lattice_view_set_current_interp(
  SarlLatticeView *view, 
  sarl_unsigned_int interp_index
)
{
  sarl_lattice_view_interp_t *view_interp = 
    (sarl_lattice_view_interp_t *)
      sarl_array_get(view->p->interps, interp_index);

  view->p->labels = sarl_array_dup_ref(view_interp->labels);
};

void sarl_lattice_view_recalc_contingent_interp(
  SarlLatticeView *view, 
  sarl_lattice_view_interp_t *interp
)
{
  sarl_lattice_view_interp_dispose(interp);

  sarl_lattice_view_interp_recalc_labels_for(
    view, interp, interp->interp->attribute_interp, 
    SARL_LABEL_ATTRIBUTE_KIND, 0, view->p->concept_radius);
  sarl_lattice_view_interp_recalc_labels_for(
    view, interp, interp->interp->object_interp, 
    SARL_LABEL_OBJECT_KIND, 0, -view->p->concept_radius);
};


sarl_unsigned_int sarl_label_view_add_interp(
  SarlLatticeView *view,
  sarl_concept_interp_t *interp
)
{
  sarl_lattice_view_interp_t *view_interp = 
    sarl_lattice_view_interp_new(interp);
  sarl_unsigned_int view_interp_index = 
    sarl_array_push(view->p->interps, view_interp);
  sarl_lattice_view_recalc_contingent_interp(view, view_interp);

  return view_interp_index;
};

void sarl_label_view_remove_interp(
  SarlLatticeView *view,
  sarl_unsigned_int interp_index
)
{
  sarl_array_set(view->p->interps, interp_index, 0);
};


void sarl_lattice_view_interp_rescale_labels(
  SarlLatticeView *view) 
{
  sarl_label_t *label = 0;

  if ( view->p->labels ) {
    SARL_ARRAY_FOR_EACH(sarl_label_t, label, view->p->labels);
      sarl_label_rescale(
        label, view->p->fd, view->p->font_size, view->p->scale);
    SARL_ARRAY_END;
  }
};

void sarl_lattice_view_interp_draw_labels(
  SarlLatticeView *view, 
  GdkDrawable *dest, GdkGC *gc)
{
  GtkWidget         *widget = GTK_WIDGET(view);
  sarl_label_t      *label = 0;
  sarl_point_t      *p = 0;
  sarl_unsigned_int label_index;

  int               do_draw = 1;

  int scaled_line_width = (int)(view->p->label_line_width * view->p->scale);
  if ( scaled_line_width < 1 ) scaled_line_width = 1;
  
  gdk_gc_set_line_attributes(
    gc, scaled_line_width, GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_MITER);

  // draw the lines first

  SARL_ARRAY_FOR_EACH_INDEX(label_index, sarl_label_t, label, view->p->labels);
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

  // next draw the labels themselves
  SARL_ARRAY_FOR_EACH_INDEX(label_index, sarl_label_t, label, view->p->labels);
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

sarl_unsigned_int sarl_lattice_view_label_at(
  SarlLatticeView       *view,                                               
  sarl_signed_int       x,
  sarl_signed_int       y
)
{
  sarl_label_t* label;
  sarl_point_t  p;
  sarl_point_t* cp = 0;
  sarl_unsigned_int index;

  SARL_ARRAY_FOR_EACH_INDEX(index, sarl_label_t, label, view->p->labels);
    cp = sarl_diagram_get_concept_pos(view->p->diagram, label->concept);
    p.x = x - cp->x;
    p.y = y - cp->y;
    if ( sarl_rect_includes(&label->bbox, &p) ) {
      return index;
    }
  SARL_ARRAY_END;

  return 0;
};




