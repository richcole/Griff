require 'lattice_viewer'

Gnome::Program.new(LatticeViewerApp::NAME, LatticeViewerApp::VERSION)
app = LatticeViewerApp.new()
app.show_all()

Gtk::main
