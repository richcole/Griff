#include "sarl_label.h"
#include "sarl_lattice_view_gtk.h"
#include "pango/pango-font.h"

#define TRL_X(px) ((int)((px + offset->x) * scale))
#define TRL_Y(py) ((int)((py + offset->y) * scale))
#define max_of(x,y) ((x)>=(y)?(x):(y))
#define min_of(x,y) ((x)<=(y)?(x):(y))

void sarl_label_draw(
  GtkWidget *widget,
  GdkDrawable *dest,
  GdkGC *gc,
  sarl_label_t *label,
  sarl_signed_int left, sarl_signed_int top, 
  sarl_point_t* offset, double scale
)
{
  PangoLayout *layout;

  // set the cliping region
  gint line_offset = 0;
  gint layout_width, layout_height;

  sarl_unsigned_int num_lines = sarl_array_count(label->layouts);

  GdkRectangle rect = { 
    TRL_X(left + label->margin + label->bbox.left), 
    TRL_Y(top + label->margin + label->bbox.top), 
    (int)((label->bbox.width - (label->is_active ? 3 : 2) * label->margin) 
      * scale), 
    (int)((label->bbox.height - label->margin) * scale)
  };
  gdk_gc_set_clip_rectangle(gc, &rect);
 
  // set the first line if required
  if ( label->first_line != 0 ) {
    sarl_label_set_before(label, label->first_line);
    layout = label->before;
    gdk_draw_layout(dest, gc, rect.x, line_offset + rect.y, layout);
    pango_layout_get_size(layout, &layout_width, &layout_height);
    line_offset += layout_height / PANGO_SCALE;
  }

  // draw the label content
  sarl_unsigned_int layout_index;
  layout_index = label->first_line+1; 
  layout = (PangoLayout *)sarl_array_get(label->layouts, layout_index);

  while( layout ) {
    gdk_draw_layout(dest, gc, rect.x, line_offset + rect.y, layout);
    pango_layout_get_size(layout, &layout_width, &layout_height);
    line_offset += layout_height / PANGO_SCALE;

    ++layout_index;
    layout = (PangoLayout *)sarl_array_get(label->layouts, layout_index);

    sarl_signed_int second_next_line_bottom = 
      line_offset + 2*(layout_height / PANGO_SCALE);
    
    if ( label->is_height_set && second_next_line_bottom > rect.height ) {
      // check to see if the second next line goes below the clipping region
      if ( layout_index < num_lines ) {
        // if the next line is the last don't bother
        break;
      }
    }
  }

  // set the last line if required
  if ( layout != 0 ) {
    sarl_label_set_after(label, (num_lines - layout_index) + 1);
    layout = label->after;
    gdk_draw_layout(dest, gc, rect.x, line_offset + rect.y, layout);
    pango_layout_get_size(layout, &layout_width, &layout_height);
    line_offset += layout_height / PANGO_SCALE;
  }

  // reset the clipping region
  rect.x = rect.y = 0;
  gdk_drawable_get_size(
    dest, &rect.width, &rect.height);
  gdk_gc_set_clip_rectangle(gc, &rect);
};


// ---------------------------------------------------------------------------
// Background with Scrollbar
//

void sarl_label_draw_background_with_scrollbar(
  SarlLatticeView *view, GdkDrawable *dest, GdkGC *gc,
  sarl_label_t *label, sarl_signed_int x, sarl_signed_int y,
  sarl_point_t* offset, double scale
)
{
  GtkWidget *widget = GTK_WIDGET(view);
  sarl_unsigned_int s_label_margin = ((int)(label->margin * 2 * scale))+1;
  sarl_unsigned_int r_left   = TRL_X(x + label->bbox.left)-1;
  sarl_unsigned_int r_top    = TRL_Y(y + label->bbox.top)-1;
  sarl_unsigned_int r_width  = (int)((label->bbox.width  - 2*label->margin) * scale)+2;
  sarl_unsigned_int r_height = (int)(label->bbox.height * scale)+2;

  gdk_draw_rectangle(
    dest, widget->style->white_gc, true, 
    r_left, r_top, r_width + s_label_margin, r_height
  );

  gdk_gc_set_rgb_fg_color(gc, &view->line_good_color);

  gtk_draw_shadow(
   widget->style, dest,
   (GtkStateType)widget->state, GTK_SHADOW_OUT, 
   r_left, r_top, r_width + s_label_margin, r_height
  );

  gtk_draw_shadow(
   widget->style, dest,
   (GtkStateType)widget->state, GTK_SHADOW_OUT, 
   r_left + r_width, r_top, s_label_margin, r_height
  );

  if ( r_height > 2*s_label_margin ) {

    gtk_paint_arrow(
      widget->style, 
      dest,
      (GtkStateType)widget->state, 
      GTK_SHADOW_OUT,
      0, /* area   */
      0, /* style  */
      0, /* detail */
      GTK_ARROW_UP,
      true,
      r_left + r_width + 1, 
      r_top+1, 
      s_label_margin - 2, s_label_margin/2
    );
    
    gtk_paint_arrow(
      widget->style, 
      dest,
      (GtkStateType)widget->state, 
      GTK_SHADOW_OUT,
      0, /* area   */
      0, /* style  */
      0, /* detail */
      GTK_ARROW_DOWN,
      true,
      r_left + r_width + 1, 
      (r_top + r_height) - ((3 * s_label_margin) / 2), 
      s_label_margin - 2, s_label_margin/2
    );
  };
    
  gtk_paint_resize_grip(
    widget->style, 
    dest,
    (GtkStateType)widget->state, 
    0, /* area   */
    0, /* style  */
    0, /* detail */
    GDK_WINDOW_EDGE_SOUTH_EAST, 
    r_left + r_width + 1, 
    r_top + r_height - s_label_margin - 1, 
    s_label_margin - 2, s_label_margin - 2
  );
};

// ---------------------------------------------------------------------------
// Background without Scrollbar
//

void sarl_label_draw_background(
  SarlLatticeView *view, GdkDrawable *dest, GdkGC *gc,
  sarl_label_t *label, sarl_signed_int x, sarl_signed_int y,
  sarl_point_t* offset, double scale
)
{
  GtkWidget *widget = GTK_WIDGET(view);
  sarl_signed_int r_left   = TRL_X(x + label->bbox.left)-1;
  sarl_signed_int r_top    = TRL_Y(y + label->bbox.top)-1;
  sarl_signed_int r_width  = (int)(label->bbox.width * scale)+2;
  sarl_signed_int r_height = (int)(label->bbox.height * scale)+2;

  gdk_draw_rectangle(
    dest, widget->style->white_gc, true, 
    r_left, r_top, r_width, r_height
  );

  gdk_gc_set_rgb_fg_color(gc, &view->line_good_color);
  gtk_draw_shadow(
   widget->style, dest,
   (GtkStateType)widget->state, GTK_SHADOW_OUT, 
   r_left, r_top, r_width, r_height
  );
};

void sarl_label_rescale(
  sarl_label_t* label, PangoFontDescription *fd,
  sarl_unsigned_int font_size, double scale
)
{
  PangoLayout *layout;
  gint layout_width, layout_height;
  sarl_unsigned_int old_width = label->rect.width;

  label->rect.width = 0;
  label->rect.height = 0;
  SARL_ARRAY_FOR_EACH(PangoLayout, layout, label->layouts);
    pango_layout_set_font_description(layout, fd);
    pango_layout_get_size(layout, &layout_width, &layout_height);
    label->rect.width = max_of(
      label->rect.width, (int)(layout_width / PANGO_SCALE));
    label->rect.height += layout_height / PANGO_SCALE;
  SARL_ARRAY_END;

  pango_layout_set_font_description(label->before, fd);
  pango_layout_set_font_description(label->after, fd);

  label->rect.height = ((int)(label->rect.height / scale)) + 1;
  label->rect.width  = ((int)(label->rect.width / scale)) + 1;

/*
 * This next bit doesn't look much good.
 *
  if ( old_width > 10 && label->rect.width > 10 ) {
    label->rect.left = (unsigned int)((
      ((float)label->rect.left) * label->rect.width) / old_width);
    label->rect.top = (unsigned int)((
      ((float)label->rect.top) * label->rect.width) / old_width);
  }
*/

  if ( label->is_width_set ) {
    label->width  = label->width * label->rect.width / old_width;
  }
  if ( label->is_height_set ) {
    label->height = label->height * label->rect.width / old_width;
  }

  sarl_label_update_bbox(label, font_size);

};

void sarl_label_update_bbox(sarl_label_t *label, sarl_unsigned_int font_size)
{
  label->margin = font_size / 2;
  label->bbox.left   = label->rect.left - label->margin; 
  label->bbox.top    = label->rect.top - label->margin; 

  if ( label->is_width_set ) {
    label->bbox.width  = sarl_min_of(
      label->rect.width + (2 * label->margin), label->width
    );
  }
  else {
    label->bbox.width  = label->rect.width + (2 * label->margin);
  }

  if ( label->is_active ) {
    label->bbox.width += 2 * label->margin;
  }

  if ( label->is_height_set ) {
    label->bbox.height  = sarl_min_of(
      label->rect.height + (2 * label->margin), label->height
    );
  }
  else {
    label->bbox.height = label->rect.height + (2 * label->margin); 
  }
};

sarl_unsigned_int sarl_label_layout_index_at(
  sarl_label_t* label, sarl_signed_int x, sarl_signed_int y, double scale
)
{
  x -= label->rect.left;
  y -= label->rect.top;

  x = (int)(x * scale);
  y = (int)(y * scale);
  
  PangoLayout *layout;
  gint line_offset = 0;
  gint layout_width, layout_height;
  sarl_unsigned_int layout_index;

  // set the first line if required
  if ( label->first_line != 0 ) {
    line_offset += layout_height / PANGO_SCALE;
    layout_index = label->first_line; 
  }
  else {
    layout_index = label->first_line+1; 
  }

  // draw the label content
  layout = (PangoLayout *)sarl_array_get(label->layouts, layout_index);

  while( layout ) {
    pango_layout_get_size(layout, &layout_width, &layout_height);
    if ( y >= line_offset && y <= line_offset + layout_height / PANGO_SCALE ) {
      return layout_index;
    }
    line_offset += layout_height / PANGO_SCALE;

    ++layout_index;
    layout = (PangoLayout *)sarl_array_get(label->layouts, layout_index);
  };    

  return 0;
};

void sarl_label_size_to_three_lines(sarl_label_t *label, double scale, sarl_unsigned_int font_size)
{
  gint layout_width, layout_height;
  fprintf(stderr, "Truncating: count=%u\n", sarl_array_count(label->layouts));
  if ( sarl_array_count(label->layouts) > 2 ) {
    label->is_height_set = 1;
    label->height = 0;
    label->is_width_set = 1;
    label->width = 0;
    sarl_unsigned_int layout_index = 1;
    PangoLayout *layout = 
      (PangoLayout *)sarl_array_get(label->layouts, layout_index);
    while( layout_index <= 3 && layout ) {
      pango_layout_get_size(layout, &layout_width, &layout_height);
      label->height += (int)((layout_height / PANGO_SCALE) / scale);
      label->width  = 
        sarl_max_of(
          label->width, 
          (sarl_unsigned_int)((layout_width / PANGO_SCALE) / scale)
        );
      ++layout_index;
      layout = (PangoLayout *)sarl_array_get(label->layouts, layout_index);
    }
    label->height += (int)((layout_height / PANGO_SCALE) / scale) / 2;
    label->width += (int)((layout_height / PANGO_SCALE) / scale) / 2;
    sarl_label_update_bbox(label, font_size);
  }
};

SarlLabelRegionType sarl_label_region_at(
  sarl_label_t *label, sarl_signed_int x, sarl_signed_int y
)
{
  sarl_point_t p = { x, y };
  if ( sarl_rect_includes(&label->bbox, &p) ) {
    x -= label->bbox.left;
    if ( label->is_width_set && x <= (sarl_signed_int)(label->width) 
    ) {
        return SARL_LABEL_REGION_CONTENTS;
    }
    else if ( ! label->is_width_set &&
      x <= (sarl_signed_int)(label->rect.width + label->margin*2) 
    ) {
        return SARL_LABEL_REGION_CONTENTS;
    }
    else {
      if ( label->is_active ) {
        y -= label->bbox.top;
        if ( y <= (sarl_signed_int)(label->margin) ) {
          return SARL_LABEL_REGION_UP_ARROW;
        }
        else if ( y < (sarl_signed_int)(label->bbox.height - (label->margin*3)) ) {
          return SARL_LABEL_REGION_SCROLL;
        }
        else if ( y < (sarl_signed_int)(label->bbox.height - (label->margin*2)) ) {
          return SARL_LABEL_REGION_DOWN_ARROW;
        }
        else if ( y < (sarl_signed_int)(label->bbox.height) ) {
          return SARL_LABEL_REGION_RESIZE;
        }
      }
    }
  }
  return SARL_LABEL_REGION_OUTSIDE;
};

void sarl_label_resize_by(
  sarl_label_t *label, sarl_signed_int dx, sarl_signed_int dy,
  sarl_unsigned_int font_size
)
{
  if ( ! label->is_width_set ) {
    label->is_width_set = 1;
    label->width = label->bbox.width;
  }
  if ( ! label->is_height_set ) {
    label->is_height_set = 1;
    label->height = label->bbox.height;
  }
  label->width  = sarl_in_range(
    ((sarl_signed_int)label->width) + dx, 
    ((sarl_signed_int)label->margin*2), 
    ((sarl_signed_int)label->rect.width) + 
      ((sarl_signed_int)label->margin)*2
  );
  label->height = sarl_in_range(
    ((sarl_signed_int)label->height + dy), 
    (sarl_signed_int)(label->margin*2), 
    ((sarl_signed_int)label->rect.height) + 
      ((sarl_signed_int)label->margin*2)
  );
  sarl_label_update_bbox(label, font_size);
}

void sarl_label_resize_to(
  sarl_label_t *label, sarl_signed_int x, sarl_signed_int y,
  sarl_unsigned_int font_size
)
{
  x -= label->bbox.left;
  x -= label->margin;
  y -= label->bbox.top;
  y += label->margin;

  if ( ! label->is_width_set ) {
    label->is_width_set = 1;
    label->width = label->bbox.width;
  }
  if ( ! label->is_height_set ) {
    label->is_height_set = 1;
    label->height = label->bbox.height;
  }
  label->width  = sarl_in_range(
    x, (sarl_signed_int)(label->margin*2), (sarl_signed_int)(label->rect.width + (label->margin*2))
  );
  label->height  = sarl_in_range(
    y, (sarl_signed_int)(label->margin*2), (sarl_signed_int)(label->rect.height + (label->margin*2))
  );
  sarl_label_update_bbox(label, font_size);
}

void sarl_label_set_before(
  sarl_label_t *label, sarl_unsigned_int before
) 
{
  char buf[1024];

  if ( before ) {
    sprintf(buf, "... %d before", before);
    pango_layout_set_text(label->before, buf, strlen(buf));
  }
};                            

void sarl_label_set_after(
  sarl_label_t *label, sarl_unsigned_int after
) 
{
  char buf[1024];

  if ( after ) {
    sprintf(buf, "... %d after", after);
    pango_layout_set_text(label->after, buf, strlen(buf));
  }
};                            

void sarl_label_dispose(sarl_label_t *label)
{
  PangoLayout *layout;

  SARL_ARRAY_FOR_EACH(PangoLayout, layout, label->layouts);
    g_object_unref(layout);
  SARL_ARRAY_END;
  sarl_array_clear(label->layouts);

  if (label->before) {
    g_object_unref(label->before);
    label->before = 0;
  };

  if (label->after) {
    g_object_unref(label->after);
    label->after = 0;
  };
};

void sarl_label_scroll_down(sarl_label_t *label)
{
  if ( label->first_line < sarl_array_count(label->layouts) ) {
    label->first_line++;
  }
}

void sarl_label_scroll_up(sarl_label_t *label)
{
  if ( label->first_line > 0 ) {
    label->first_line--;
  }
}
