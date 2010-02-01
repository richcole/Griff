require 'sarl'
require 'sarlgtk'
require 'lexicon'
require 'interp_context'
require 'gtk2'
require 'decorated_lattice_view'

include Sarl
Gtk.init

class SubLatticeView 
  attr_reader :context, :diagram, :lattice_view, :attribute_lexicon, :ic
  
  def initialize 
    @ic = InterpContext.new(Context.new, StringTable.new, StringTable.new)
    @context = Sarl::Context.new

    @ic.st_attributes = Sarl::StringTable.new
    @ic.st_objects = Sarl::StringTable.new
    @diagram = Sarl::Diagram.layout(@context)

    @lattice_view = 
      DecoratedLatticeView.new(
        @ic.context, @ic.st_objects, @ic.st_attributes
      )

    @attribute_lexicon = Lexicon.new
    @object_lexicon = Lexicon.new
    @attributes = BitSet.new
  end

  def load_string_table_to_lexicon(st, lexicon)
    lexicon.clear
    for i in 1..st.count do
      lexicon.set_word(st[i], i)
    end
  end

  def load_context(filename)
    @attributes = BitSet.new
    @ic = interp_context_from_cxt(filename)
    @context = Context.new()
    load_string_table_to_lexicon(@ic.st_objects, @object_lexicon)
    load_string_table_to_lexicon(@ic.st_attributes, @attribute_lexicon)
  end

  def add_attribute(attribute)
    @attributes.set(attribute)
    @context = @ic.context.attribute_subcontext(@attributes)
    @diagram = Sarl::Diagram.layout(@context)
    @lattice_view.view.set_diagram(@diagram, @ic.st_objects, @ic.st_attributes)
  end

  def clear
    @attributes.clear_all()
    @context = Context.new
    @diagram = Sarl::Diagram.layout(@context)
    @lattice_view.view.set_diagram(@diagram, @ic.st_objects, @ic.st_attributes)
  end

  def remove_attribute(attribute)
    @attributes.clear(attribute)
    @context.remove_attribute(attribute)
    @diagram = Sarl::Diagram.layout(@context)
    @lattice_view.view.set_diagram(@diagram, @ic.st_objects, @ic.st_attributes)
  end

end

class SubLatticeViewer

  def initialize
    @hpane = Gtk::HPaned.new
    @vpane = Gtk::VPaned.new
    @swindow = Gtk::ScrolledWindow.new
    @window = Gtk::Window.new
    @treestore = Gtk::TreeStore.new(String, Integer, Integer, FalseClass)
    @tree = Gtk::TreeView.new(@treestore)

    @sublattice_view = SubLatticeView.new

    @tree.signal_connect("row-activated") do 
      |view, path, column|
      if iter = @tree.model.get_iter(path)
        iter[3] = ! iter[3]
        if iter[3] then 
          @sublattice_view.add_attribute(iter[1])
        else
          @sublattice_view.remove_attribute(iter[1])
        end
      end
    end

    @clearbutton = Gtk::Button.new("Clear")
    
    # connect signal
    @clearbutton.signal_connect("clicked") {
      @treestore.each do |model, path, iter|
        if iter[2] then
          iter[2] = false
        end
      end
      # clear lattice
      @sublattice_view.clear
    }
    
    @renderer = Gtk::CellRendererText.new
    @renderer.weight = Pango::FontDescription::WEIGHT_BOLD
    @column = Gtk::TreeViewColumn.new(
      "Keyword", @renderer, { :text => 0, :weight_set => 3})
    @tree.append_column(@column)

    @column = Gtk::TreeViewColumn.new(
      "Frequency", @renderer, { :text => 2, :weight_set => 3})
    @tree.append_column(@column)

    @window.add(@hpane)
    @hpane.add(@swindow)
    @hpane.add(@vpane)
    @vpane.add(@clearbutton)
    @vpane.add(@sublattice_view.lattice_view)
    @swindow.add(@tree)
    @window.set_default_size(300,300)
    @window.show_all
    @window.signal_connect("destroy") { Gtk.main_quit }

    @treestore.set_sort_column_id(2)
  end

  def load_context(filename)
    @sublattice_view.load_context(filename)
    lex = @sublattice_view.attribute_lexicon
    for m in lex.indexes do
      item = @treestore.append(nil)
      item[0] = lex.word(m)
      item[1] = m
      item[2] = @sublattice_view.ic.context.attribute_extent(m).count
      item[3] = false
    end
  end
end


slv = SubLatticeViewer.new
slv.load_context("test.cxt")
Gtk.main




  
