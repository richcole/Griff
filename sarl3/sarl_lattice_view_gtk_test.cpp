#include <stdlib.h>
#include <gtk/gtk.h>
#include <gtk/gtkvbox.h>
#include "sarl_lattice_view_gtk.h"
#include "sarl_diagram_examples.h"
#include "sarl_string_table.h"

struct AppContext {
  GtkWidget* view;
  GtkWidget* file_sel;
  sarl_diagram_t *diagram;
  sarl_string_table_t *st;
  unsigned int count;
};

void menuitem_response(gchar *signalname)
{
  fprintf(stderr, "File Selected signal='%s'.\n", signalname);
}

void trace_notify_font_size(GtkWidget *widget, GValue *param)
{
  fprintf(stderr, "Notify widget font size=%p, param=%p.\n", widget, param);
}

void trace_concept_selected(
  GObject *gobject, guint concept, guint button, guint state, gpointer user)
{
  fprintf(stderr, "Concept selected: widget=%p, concept=%u, button=%u",
    gobject, concept, button);
  fprintf(stderr, ", state=");
  if ( state & GDK_BUTTON1_MASK ) fprintf(stderr, "Button 1 ");
  if ( state & GDK_BUTTON2_MASK ) fprintf(stderr, "Button 2 ");
  if ( state & GDK_BUTTON3_MASK ) fprintf(stderr, "Button 3 ");
  if ( state & GDK_SHIFT_MASK ) fprintf(stderr, "Shift ");
  if ( state & GDK_LOCK_MASK ) fprintf(stderr, "Lock ");
  if ( state & GDK_CONTROL_MASK ) fprintf(stderr, "Control ");
  if ( state & GDK_MOD1_MASK ) fprintf(stderr, "Mod 1 ");
  if ( state & GDK_MOD2_MASK ) fprintf(stderr, "Mod 2 ");
  fprintf(stderr, ", user=%p\n", user);
}

void trace_object_selected(GtkWidget *widget, GValue *param)
{
  fprintf(stderr, "Object selected: widget=%p, param=%p.\n", widget, param);
}

void trace_attribute_selected(GtkWidget *widget, GValue *param)
{
  fprintf(stderr, "Attribute selected: widget=%p, param=%p.\n", widget, param);
}

void trace_notify_selected_object(GtkWidget *widget, GValue *param)
{
  fprintf(stderr, "Notify::selected-object: widget=%p, param=%p.\n", widget, param);
}

void trace_notify_selected_attribute(GtkWidget *widget, GValue *param)
{
  fprintf(stderr, "Notify::selected-attribute: widget=%p, param=%p.\n", widget, param);
}

void trace_notify_selected_concept(GtkWidget *widget, GValue *param)
{
  fprintf(stderr, "Notify::selected-concept: widget=%p, param=%p.\n", widget, param);
}

void trace_notify(GtkWidget *widget, GValue *param)
{
  fprintf(stderr, "Notify widget=%p, param=%p.\n", widget, param);
}

void load_context(GtkWidget *widget, AppContext *context)
{
  gchar const* filename = 
    gtk_file_selection_get_filename(GTK_FILE_SELECTION(context->file_sel));

  if ( filename ) {
    fprintf(stderr, "Loading Diagram: %s\n", filename);
    fprintf(stderr, "  - instead loading a B3 context.\n", filename);
    
    context->diagram = sarl_diagram_create_b3();
    context->st = sarl_string_table_default_new();
    sarl_string_table_populate(context->st, 1, 3);

    sarl_lattice_view_set_diagram(
      SARL_LATTICE_VIEW(context->view), 
      context->diagram, context->st, context->st
    );
  }

  gtk_widget_destroy(context->file_sel);
};

gboolean callback_update_view(AppContext *context) {
  fprintf(stderr, "Update...%d\n", context->count);
  SarlLatticeView *view = SARL_LATTICE_VIEW(context->view);
  context->count++;
  sarl_lattice_view_update(view);
  sarl_lattice_view_dump_to(view, stderr);
  GC_gcollect();
  return FALSE;
};

void callback_file_open(AppContext *context)
{
  fprintf(stderr, "File Open.\n");
  context->file_sel = gtk_file_selection_new("Select Context to Load");
  
  g_signal_connect(
    G_OBJECT (GTK_FILE_SELECTION(context->file_sel)->ok_button), 
    "clicked", G_CALLBACK(load_context), (gpointer)context
  );

  g_signal_connect_swapped(
    G_OBJECT(GTK_FILE_SELECTION(context->file_sel)->cancel_button),
    "clicked", G_CALLBACK (gtk_widget_destroy), G_OBJECT (context->file_sel)
  );

  gtk_widget_show(context->file_sel);
}

GtkWidget* create_menus(AppContext *context)
{
  GtkWidget *file_menu = gtk_menu_new ();    /* Don't need to show menus */

  /* Create the menu items */
  GtkWidget *open_item = gtk_menu_item_new_with_label ("Open");
  GtkWidget *save_item = gtk_menu_item_new_with_label ("Save");
  GtkWidget *quit_item = gtk_menu_item_new_with_label ("Quit");
  
  /* Add them to the menu */
  gtk_menu_shell_append (GTK_MENU_SHELL (file_menu), open_item);
  gtk_menu_shell_append (GTK_MENU_SHELL (file_menu), save_item);
  gtk_menu_shell_append (GTK_MENU_SHELL (file_menu), quit_item);
  
  /* Attach the callback functions to the activate signal */
  g_signal_connect_swapped (G_OBJECT (open_item), "activate",
                            G_CALLBACK (callback_file_open),
                            (gpointer) context);

  g_signal_connect_swapped (G_OBJECT (save_item), "activate",
                            G_CALLBACK (menuitem_response),
                            (gpointer) "file.save");
  
  /* We can attach the Quit menu item to our exit function */
  g_signal_connect_swapped (G_OBJECT (quit_item), "activate",
                            G_CALLBACK (exit),
                            (gpointer) "file.quit");
  
  /* We do need to show menu items */
  gtk_widget_show (open_item);
  gtk_widget_show (save_item);
  gtk_widget_show (quit_item);

  GtkWidget *menu_bar = gtk_menu_bar_new ();
  gtk_widget_show (menu_bar);
  
  GtkWidget* file_item = gtk_menu_item_new_with_label ("File");
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);
  gtk_widget_show (file_item);

  gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_item);
  return menu_bar;
}

int main( int   argc,
          char *argv[] )
{
  GtkWidget *window;
  GtkWidget *vbox;

  AppContext context;
  context.count = 0;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  vbox = gtk_vbox_new(0, 0);
  // gtk_widget_set_usize(window,500,500);
  gtk_window_set_title(GTK_WINDOW(window) , "Sarl Lattice Context.View");
  gtk_container_set_border_width(GTK_CONTAINER(window),5);
  
  g_signal_connect (G_OBJECT (window), "destroy",
		    G_CALLBACK (exit), NULL);
  
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);

  context.diagram = sarl_diagram_create_b3();
  context.st = sarl_string_table_default_new();
  sarl_string_table_populate(context.st, 1, 3);

  context.view = sarl_lattice_view_new(
    context.diagram, context.st, context.st
  );

  g_signal_connect(
    G_OBJECT(context.view), "notify::font-size", 
    G_CALLBACK(trace_notify_font_size), 0
  );

  g_signal_connect(
    G_OBJECT(context.view), "notify::selected-object", 
    G_CALLBACK(trace_notify_selected_object), 
    (gpointer) "notify::selected-object"
  );

  g_signal_connect(
    G_OBJECT(context.view), "notify::selected-attribute", 
    G_CALLBACK(trace_notify_selected_attribute), 
    (gpointer) "notify::selected-attribute"
  );

  g_signal_connect(
    G_OBJECT(context.view), "notify::selected-concept", 
    G_CALLBACK(trace_notify_selected_concept), 
    (gpointer) "notify::selected-concept"
  );

  g_signal_connect(
    G_OBJECT(context.view), "select-concept", 
    G_CALLBACK(trace_concept_selected), (gpointer) 0x1
  );

  g_signal_connect(
    G_OBJECT(context.view), "select-object", 
    G_CALLBACK(trace_object_selected), 0
  );

  g_signal_connect(
    G_OBJECT(context.view), "select-attribute", 
    G_CALLBACK(trace_attribute_selected), 0
  );

  fprintf(stderr, "--------------\n");
  GValue *value = g_new0(GValue, 1);
  g_value_init(value, G_TYPE_INT);
  g_value_set_int(value, 160);
  g_object_set_property(G_OBJECT(context.view), "font_size", value);
  g_free(value);
  fprintf(stderr, "--------------\n");

  GtkWidget *menu = create_menus(&context);
  gtk_widget_show_all(menu);
  gtk_box_pack_start (GTK_BOX (vbox), menu, FALSE, FALSE, 2);
  gtk_box_pack_end   (GTK_BOX (vbox), context.view, TRUE, TRUE, 2);
  gtk_container_add (GTK_CONTAINER (window), vbox);

  gtk_widget_show_all(window);

  g_timeout_add(50,  (GtkFunction)callback_update_view, &context);

  GValue gvalue;
  memset(&gvalue, 0, sizeof(gvalue));
  g_value_init(&gvalue, G_TYPE_UINT);

  g_value_set_uint(&gvalue, 150);
  g_object_set_property(G_OBJECT(context.view), "font_size", &gvalue);

  gtk_main ();
  
  return 0;
}
