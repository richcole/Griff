#ifndef SARL_LABEL_H
#define SARL_LABEL_H

#include "sarl_types.h"
#include "sarl_exceptions.h"
#include "sarl_diagram.h"
#include "sarl_array.h"
#include "sarl_rect.h"

#include <pango/pango.h>
#include <gtk/gtkwidget.h>
#include <gdk/gdkgc.h>

#include <gc/gc.h>

typedef enum _SarlLabelKind {
  SARL_LABEL_ATTRIBUTE_KIND  = 0,
  SARL_LABEL_OBJECT_KIND     = 1
} SarlLabelKind;

struct _sarl_label_t {
  SarlLabelKind        kind;          /* e.g. SARL_LABEL_ATTRIBUTE_KIND */
  sarl_rect_t          rect;          /* unscaled coords of the label   */
  sarl_rect_t          bbox;          /* unscaled coords of the label   */
  sarl_unsigned_int    concept;       /* lattice concept number         */
  sarl_array_t*        layouts;       /* PangoLayout                    */
  sarl_unsigned_int    first_line;    /* first line displayed           */
  sarl_unsigned_int    num_lines;     /* number of lines displayed      */
  sarl_unsigned_int    width;         /* width, if set                  */
  sarl_unsigned_int    is_width_set;  /* true if the width has been set */
  sarl_unsigned_int    height;        /* width, if set                  */
  sarl_unsigned_int    is_height_set; /* true if the width has been set */
  sarl_unsigned_int    is_active;     /* is the label active            */
  sarl_unsigned_int    margin;        /* margin around the text         */

  PangoLayout*         before;  /* used in a scrolled label */
  PangoLayout*         after;   /* used in a scrolled label */
};

typedef _sarl_label_t sarl_label_t;

inline char* sarl_label_name() {
  static char *name = "sarl_label_t";
  return name;
};

inline sarl_label_t* sarl_label_new(
  sarl_unsigned_int concept, SarlLabelKind kind, 
  sarl_signed_int x, sarl_signed_int y
)
{
  sarl_label_t *label = (sarl_label_t*)GC_MALLOC(sizeof(sarl_label_t));
  if ( label ) {
    // GC_REGISTER_FINALIZER_NO_ORDER(
    //   label, trace_finalization, sarl_label_name(), 0, 0
    // );
    label->kind = kind;
    label->rect.left = x;
    label->rect.top = y;
    label->concept = concept;
    label->rect.width = label->rect.height = 0;
    label->layouts = sarl_array_default_new();
    label->first_line = 0;
    label->num_lines = 0;
    label->width = 0;
    label->is_width_set = 0;
    label->is_active = 0;
    label->margin = 0;
  }
  else {
    sarl_out_of_memory_error;
  }
  return label;
}

void sarl_label_dispose(sarl_label_t *label);

/* \arg(left, top): unscaled position of the label */

void sarl_label_draw(
  GtkWidget *widget, GdkDrawable *dest,
  GdkGC *gc,
  sarl_label_t *label,
  sarl_signed_int left, sarl_signed_int top, 
  sarl_point_t* offset, double scale
);

void sarl_label_rescale(
  sarl_label_t* label, PangoFontDescription *fd, 
  sarl_unsigned_int font_size, double scale
);

/* \arg(x, y): unscaled position of the label 
 * \return(layout_index): index of the layout that was hit
 */

sarl_unsigned_int sarl_label_layout_index_at(
  sarl_label_t* label, sarl_signed_int x, sarl_signed_int y, double scale
);

typedef struct _SarlLatticeView SarlLatticeView;

void sarl_label_draw_background_with_scrollbar(
  SarlLatticeView *view, GdkDrawable *dest, GdkGC *gc,
  sarl_label_t *label, sarl_signed_int x, sarl_signed_int y,
  sarl_point_t* offset, double scale
);

void sarl_label_draw_background(
  SarlLatticeView *view, GdkDrawable *dest, GdkGC *gc,
  sarl_label_t *label, sarl_signed_int x, sarl_signed_int y,
  sarl_point_t* offset, double scale
);

inline void sarl_label_move_by(
  sarl_label_t *label, sarl_signed_int x, sarl_signed_int y
) 
{
  label->rect.left += x;
  label->rect.top += y;
  label->bbox.left += x;
  label->bbox.top += y;
};

void sarl_label_resize_by(
  sarl_label_t *label, sarl_signed_int x, sarl_signed_int y,
  sarl_unsigned_int font_size
);

void sarl_label_resize_to(
  sarl_label_t *label, sarl_signed_int x, sarl_signed_int y,
  sarl_unsigned_int font_size
);

inline void sarl_label_set_active(
  sarl_label_t *label, sarl_unsigned_int is_active)
{
  if ( label == 0 ) return;
  if ( label->is_active != is_active ) {
    label->is_active = is_active;
    if ( is_active ) {
      label->bbox.width += 2*label->margin;
    }
    else {
      label->bbox.width -= 2*label->margin;
    }
  }
};

void sarl_label_update_bbox(sarl_label_t *label, sarl_unsigned_int font_size);

typedef enum _SarlLabelRegionType {
  SARL_LABEL_REGION_OUTSIDE,  /* outside the label */
  SARL_LABEL_REGION_CONTENTS,
  SARL_LABEL_REGION_SCROLL,
  SARL_LABEL_REGION_UP_ARROW,
  SARL_LABEL_REGION_DOWN_ARROW,
  SARL_LABEL_REGION_RESIZE
} SarlLabelRegionType;

SarlLabelRegionType sarl_label_region_at(sarl_label_t *label, sarl_signed_int x, sarl_signed_int y);

void sarl_label_set_before(
  sarl_label_t *label, sarl_unsigned_int before
);

void sarl_label_set_after(
  sarl_label_t *label, sarl_unsigned_int after
);

void sarl_label_scroll_up(sarl_label_t *label);
void sarl_label_scroll_down(sarl_label_t *label);

void sarl_label_size_to_three_lines(sarl_label_t *label, double scale, sarl_unsigned_int font_size);

#endif
