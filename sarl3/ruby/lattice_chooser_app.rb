require 'lattice_chooser'
require 'cxt_file_format'

Gtk.init

ic = interp_context_from_cxt('/home/rcole/Source/griff/flashview/contexts/b4-judgements.cxt')

metrics = []
diagrams = Diagram.layout_array(ic.context, metrics)

def max_of(x,y)
  return (x != nil && x > y) ? x : y
end

maxes = []
for m in metrics do
  for mi in m.range do
    maxes[mi] = max_of(maxes[mi], m[mi])
  end
end

sd = []
for i in diagrams.range do
  d = diagrams[i]
  m = metrics[i]
  score = 0
  for mi in m.range do
    score += 1 if m[mi] == maxes[mi]
  end
  sd.push [score, d, m]
end

diagrams = sd.sort { |x,y| - ( x[0] <=> y[0] ) }.map { |x| x[1] }
puts "Diagrams count=#{diagrams.size}"
lattice = Lattice.new(ic.context)  

window = Gtk::Window.new
chooser = LatticeChooser.new(ic, lattice, diagrams)
window.add(chooser)
window.show_all()
window.signal_connect("delete_event") {
  Gtk.main_quit
}

Gtk::main
