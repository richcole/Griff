require 'nav_space_viewer'

Gnome::Program.new(NavSpaceViewerApp::NAME, NavSpaceViewerApp::VERSION)
app = NavSpaceViewerApp.new()
app.show_all()

Gtk::main
