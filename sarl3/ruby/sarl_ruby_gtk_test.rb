require 'gtk2'
require 'sarl'
require 'sarlrbgtk'

include Gtk
include SarlGtk
include Sarl

Gtk.init

diagram = Diagram.create_b3()
st = StringTable.new
st.populate(1,3)

window = Window.new()
view = LatticeView.new(diagram, st, st)
window.add view 
window.show_all

window.signal_connect('destroy') { Gtk.main_quit }

Gtk.main
