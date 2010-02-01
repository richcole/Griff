#ifndef SARL_LATTICE_VIEW_MACROS_H
#define SARL_LATTICE_VIEW_MACROS_H

// ----------------------------------------------------------------------
// Macros
//

#define GET_FG_GC(widget) widget->style->fg_gc[widget->state]
#define GET_WT_GC(widget) widget->style->white_gc

#define TR_X(px) ((int)((px + view->p->offset.x) * view->p->scale))
#define TR_Y(py) ((int)((py + view->p->offset.y) * view->p->scale))

#define ITR_X(px) ((int)((px / view->p->scale) - view->p->offset.x))
#define ITR_Y(py) ((int)((py / view->p->scale) - view->p->offset.y))

#define gdk_color_set(c, p, r, g, b) \
  (c).pixel = (p); \
  (c).red   = (r); \
  (c).green = (g); \
  (c).blue  = (b)

#endif
