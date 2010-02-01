#ifndef SARL_LATTICE_VIEW_H
#define SARL_LATTICE_VIEW_H

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtkdrawingarea.h>

#include "sarl_diagram.h"
#include "sarl_rect.h"
#include "sarl_array.h"
#include "sarl_label.h"

G_BEGIN_DECLS

#define SARL_LATTICE_VIEW_TYPE \
   (sarl_lattice_view_get_type ())

#define SARL_LATTICE_VIEW(obj)  \
   (G_TYPE_CHECK_INSTANCE_CAST( \
      (obj), SARL_LATTICE_VIEW_TYPE, SarlLatticeView) \
   )
#define SARL_LATTICE_VIEW_CLASS(klass) \
   (G_TYPE_CHECK_CLASS_CAST( \
      (klass), SARL_LATTICE_VIEW_TYPE, SarlLatticeViewClass) \
   )
#define IS_SARL_LATTICE_VIEW(obj) \
   (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SARL_LATTICE_VIEW_TYPE))

#define IS_SARL_LATTICE_VIEW_CLASS(klass) \
   (G_TYPE_CHECK_CLASS_TYPE ((klass), SARL_LATTICE_VIEW_TYPE))

typedef struct _SarlLatticeView       SarlLatticeView;
typedef struct _SarlLatticeViewClass  SarlLatticeViewClass;
typedef struct _SarlLatticeViewPriv   SarlLatticeViewPriv;

struct _SarlLatticeView
{
  GtkDrawingArea  parent;

  SarlLatticeViewPriv *p;

  sarl_point_t      prev_pt;               /* previous point when draging */

  sarl_unsigned_int selected_concept;      /* the currently selected concept */
  sarl_unsigned_int selected_label;        /* the currently selected concept */
  sarl_unsigned_int selected_layout_index; /* the currently selected concept */
  sarl_unsigned_int selected_object;       /* the currently selected concept */
  sarl_unsigned_int selected_attribute;    /* the currently selected concept */

  sarl_unsigned_int hover_label;           /* label under the mouse   */
  sarl_unsigned_int hover_concept;         /* concept under the mouse */

  sarl_unsigned_int dragged_concept;      /* the concept being dragged */
  sarl_unsigned_int dragged_label;        /* the concept being dragged */

  SarlLabelRegionType label_region;        /* the label region last hit */

  GdkColor          line_good_color;
  GdkColor          line_bad_color;
  GdkColor          concept_color;
  GdkColor          bg_color;

  /* diagram coords: units of the sarl_diagram_t class 
   * area_coords: units of the drawing area
   *
   * transform (diag -> area) is: 
   *    x -> (x + offset.x) * scale
   *    y -> (y + offset.y) * scale
   */
};

struct _SarlLatticeViewClass
{
  GtkDrawingAreaClass parent_class;

  
};

GType sarl_lattice_view_get_type(void);

GtkWidget* sarl_lattice_view_new(
  sarl_diagram_t *diagram, 
  sarl_string_table_t* st_objects, 
  sarl_string_table_t* st_attributes
);

void 
  sarl_lattice_view_set_diagram(
    SarlLatticeView *view, sarl_diagram_t *diagram, 
    sarl_string_table_t *object, sarl_string_table_t *attributes
  );

void sarl_lattice_view_clear(SarlLatticeView *view);
void sarl_lattice_view_update(SarlLatticeView *view);
void sarl_lattice_view_set_bbox(SarlLatticeView *view, gint x, gint y);
void sarl_lattice_view_dump_to(SarlLatticeView *view, FILE *file);
void sarl_lattice_view_set_font_size(SarlLatticeView *view, gint size);
gint sarl_lattice_view_get_font_size(SarlLatticeView *view);
void sarl_lattice_view_center_labels(SarlLatticeView *view);

void sarl_lattice_view_set_show_attributes(
  SarlLatticeView *view, gboolean is_shown
);

void sarl_lattice_view_set_show_objects(
  SarlLatticeView *view, gboolean is_shown
);

void sarl_lattice_view_truncate_labels(SarlLatticeView *view);
void sarl_lattice_view_save_image(SarlLatticeView *view, char *filename, char *format);

void sarl_lattice_view_update_labels(SarlLatticeView *view);

void sarl_lattice_view_set_embedding(
  SarlLatticeView *view, sarl_diagram_t *diagram, 
  sarl_string_table_t* objects, sarl_string_table_t *attributes, 
  sarl_lattice_map_t *lattice_map)
;

G_END_DECLS

#endif /* SARL_LATTICE_VIEW_H */
