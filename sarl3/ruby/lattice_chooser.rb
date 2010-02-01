require 'gnome2'
require 'sarl'
require 'sarlrbgtk'

include SarlGtk
include Sarl

class LatticeChooser < Gtk::Table
  
  def initialize(ic, lattice, diagrams, nr=2, nc=3)
    super(nr, nc, true)
    puts "init parent"
    self.set_diagrams(ic, lattice, diagrams)
  end

  def set_diagrams(ic, lattice, diagrams)
    count = 0
    for d in diagrams do
      break if count >= n_rows * n_columns
      bin = Gtk::Frame.new
      view = LatticeView.new(d, ic.st_objects, ic.st_attributes)
      bin.add(view)
      self.attach_defaults(bin, 
                  count % n_columns, (count % n_columns) + 1, 
                  count.div(n_columns), count.div(n_columns) + 1)
      puts "Attaching view, d=#{d}"
      count += 1
    end
  end

end
