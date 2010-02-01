require 'gnome2'
require 'sarl'
require 'sarlrbgtk'
require 'cxt_file_format'

include Gnome
include SarlGtk
include Sarl

include 'decorated_lattice_view'

class LatticeViewerApp < App

  AUTHORS = [ "Richard Cole" ]
  COPYRIGHT = "(c) Richard Cole 2005"
  DESCRIPTION = "Program to generate and display concept lattices."
  DOCUMENTORS = [ "Richard Cole" ]
  VERSION = "0.0.1"
  NAME = "LatticeViewer"
  
  def initialize
    super("SarlLatticeViewer", "Sarl Lattice Viewer")
    signal_connect("delete_event") { close_cb }

    #
    # File Menu
    #

    file_menu = [
      UIInfo::menu_open_item(Proc.new { self.open_cb }, nil),
      UIInfo::menu_close_item(Proc.new { self.close_cb }, nil),
      UIInfo::menu_quit_item(Proc.new { self.quit_cb }, nil)
    ]

    help_menu = [
      UIInfo::menu_about_item(Proc.new { create_about }, nil),
    ]
     
    main_menu = [
      UIInfo::menu_file_tree(file_menu),
      UIInfo::menu_help_tree(help_menu)
    ]

    create_menus(main_menu)

    callback = proc { |item, arg| @label.set_text(arg) }
    main_toolbar = [
      [
        Gnome::App::UI_ITEM, "open", "Open Formal Context", 
        proc {|item, arg| self.open_cb}, 
        'toolbar/open', Gnome::App::PIXMAP_STOCK, Gtk::Stock::OPEN, 
        0, 0, nil
      ],
      Gnome::UIInfo::separator,
    ]

    #
    # Main Widget
    #

    self.create_view

    #
    # Toolbar
    # 

    @toolbar = Gtk::Toolbar.new()

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
    set_toolbar(@toolbar)
    @toolbar.show_all
  end

  def create_appbar
    self.statusbar = Gnome::AppBar.new(false, true, Gnome::PREFERENCES_USER)
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

  def create_view
    @context = Context.create_b3()
    @diagram = Diagram.layout(@context)
    @st = StringTable.new
    @st.populate(1,3)

    @bin = Gtk::Frame.new
    @view = LatticeView.new(@diagram, @st, @st)
    @bin.add(@view)
    @bin.border_width = 5
    self.set_contents(@bin)
    @view.show

    @view.signal_connect("select-concept") {
      |view,arg|
      puts "Concept Selected!"
    }
    @view.signal_connect("notify::selected-concept") {
      |view,arg|
      puts "Notify: Concept Selected!"
    }
  end

  def close_cb
    self.destroy
    Gtk::main_quit
  end

  def quit_cb
    Gtk::main_quit
  end

  def open_cb
    filename = get_open_filename
    if filename != nil then
      begin
        @ic = interp_context_from_cxt(filename)
        @diagram = Diagram.layout(@ic.context)
        @view.set_diagram(@diagram, @ic.st_objects, @ic.st_attributes)
      rescue StandardError => e
        puts "Exception: " + e.to_s
      end
    end
  end

  def get_open_filename
    result = nil
    dialog = Gtk::FileChooserDialog.new(
      "Open File", parent_window,
      Gtk::FileChooser::ACTION_OPEN, nil,
      [Gtk::Stock::CANCEL, Gtk::Dialog::RESPONSE_CANCEL],
      [Gtk::Stock::OPEN, Gtk::Dialog::RESPONSE_ACCEPT]
    )
    filter = Gtk::FileFilter.new
    filter.add_pattern("*.cxt")
    dialog.filter = filter

    if dialog.run == Gtk::Dialog::RESPONSE_ACCEPT
      result = dialog.filename
    end
    dialog.destroy

    return result
  end

  def create_about
    about = About.new("SarlLatticeViewer", 
      VERSION, COPYRIGHT, DESCRIPTION, AUTHORS, DOCUMENTORS, nil
    )
    about.show
  end
end


