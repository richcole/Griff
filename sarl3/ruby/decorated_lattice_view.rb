require 'gnome2'
require 'sarl'
require 'sarlrbgtk'
require 'cxt_file_format'

include SarlGtk
include Sarl

class DecoratedLatticeView < Gtk::VBox

  attr_reader :view

  def create_view(context = nil, st_objects = nil, st_attributes = nil)

    if context != nil && st_objects != nil && st_attributes != nil then
      @context = context 
      @diagram = Diagram.layout(@context)
      @st_objects = st_objects
      @st_attributes = st_attributes
    else
      @context = Context.create_b3()
      @diagram = Diagram.layout(@context)
      @st_objects = StringTable.new
      @st_attributes = @st_objects
      @st_objects.populate(1, 3)
    end

    @bin = Gtk::Frame.new
    @view = LatticeView.new(@diagram, @st_objects, @st_attributes)
    @bin.add(@view)
    @bin.border_width = 5
    pack_start(@bin, true, true)
  end

  def initialize(context = nil, st_objects = nil, st_attributes = nil)
    super(false)

    @toolbar = Gtk::Toolbar.new()

    pack_start(@toolbar, false, false)
    create_view(context, st_objects, st_attributes)

    create_toolbar_setter('Font Size','font-size') 
    create_toolbar_setter('Concept Size','concept-radius') 

    controls = Gtk::Frame.new("Controls");
    hbox = Gtk::HBox.new
    controls.add(hbox)
    button = Gtk::Button.new("Center Labels");
    button.signal_connect("clicked") { @view.center_labels }
    hbox.add(button)

    #
    # Showing attributes and labels
    #

    object_button = Gtk::CheckButton.new("Show Objects")
    object_button.signal_connect("clicked") {
      begin
        @view.set_property("show-objects", object_button.get_property("active"))
      rescue Exception => e
        puts "Exception! #{e.to_s}"
      end
    }
    object_button.active = @view.get_property("show-objects")
    hbox.add(object_button)

    attribute_button = Gtk::CheckButton.new("Show Attributes")
    attribute_button.signal_connect("clicked") {
      begin
        @view.set_property("show-attributes", attribute_button.get_property("active"))
      rescue Exception => e
        puts "Exception! #{e.to_s}"
      end
    }
    attribute_button.active = @view.get_property("show-attributes")
    hbox.add(attribute_button)

    @toolbar.add(controls)
    controls.show_all
    self.show_all
  end

  def create_toolbar_setter(label, property, &block)
    frame = Gtk::Frame.new(label)
    spin = Gtk::SpinButton.new(0, 10000, 1)

    if block then
      spin.signal_connect("notify::value") { 
        |spin, value|
        new_value = yield(spin.value, true)
        if @view.get_property(@view) != new_value then
          @view.set_property(property, new_value)
        end
      }
      @view.signal_connect('notify::' + property) { 
        |view, value|
        new_value = yield(@view.get_property(property), false)
        if spin.value != new_value then
          spin.value = new_value
        end
      }
      spin.value = yield(@view.get_property(property), false) 
    else
      spin.signal_connect("notify::value") { 
        |spin, value|
        new_value = spin.value
        if @view.get_property(property) != new_value then
          @view.set_property(property, spin.value)
        end
      }
      @view.signal_connect("notify::" + property) { 
        |view, value|
        new_value = @view.get_property(property)
        if spin.value != new_value then
          spin.value = new_value
        end
      }
      spin.value = @view.get_property(property)
    end

    @toolbar.append(frame, label)
    frame.add(spin)
    frame.show_all
  end

  def set_interp_context(ic)
    @ic = ic
    begin
      @diagram = Diagram.layout(@ic.context)
      @view.set_diagram(@diagram, @ic.st_objects, @ic.st_attributes)
    rescue StandardError => e
      puts "Exception: " + e.to_s
    end
  end 

  

end

  
