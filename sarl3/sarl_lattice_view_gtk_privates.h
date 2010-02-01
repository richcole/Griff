#ifndef SARL_LATTICE_VIEW_GTK_PRIVATES_H
#define SARL_LATTICE_VIEW_GTK_PRIVATES_H

typedef struct _SarlLatticeView SarlLatticeView;

// ----------------------------------------------------------------------
// Privates
//

struct _SarlLatticeViewPriv {
  sarl_diagram_t       *diagram;         /* lattice diagram               */
  sarl_string_table_t  *st_objects;      /* objects string table          */
  sarl_string_table_t  *st_attributes;   /* attributes string table       */
  sarl_lattice_map_t   *lattice_map;     /* embedding if it exists        */

  sarl_array_t         *labels;          /* stores the labels for diagram */
  sarl_array_t         *interps;         /* stores the labels for diagram */

  sarl_rect_t     bbox;           /* bounding box for diagram   */
  double          scale;          /* scaling factor             */
  sarl_point_t    offset;         /* offset (in diagram coords) */
  int             line_width;     /* inter-concept line_width */
  int             label_line_width;  /* concept-label line_width */
  int             concept_radius; /* radius of concepts */
  int             font_size;      /* font size for labels */

  PangoFontDescription   *fd;     /* font descriptions */

  gboolean        show_attributes;
  gboolean        show_objects;
};


#endif
