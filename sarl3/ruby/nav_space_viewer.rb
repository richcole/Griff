require 'gnome2'
require 'sarl'
require 'sarlrbgtk'
require 'cxt_file_format'
require 'nav_controller'

include Gnome
include SarlGtk
include Sarl

class NavSpaceViewerApp < App

  AUTHORS = [ "Richard Cole" ]
  COPYRIGHT = "(c) Richard Cole 2005"
  DESCRIPTION = "Program to generate and display concept lattices."
  DOCUMENTORS = [ "Richard Cole" ]
  VERSION = "0.0.1"
  NAME = "NavSpaceViewer"
  
  def initialize
    super("SarlNavSpaceViewer", "Sarl Nav Viewer")
    signal_connect("delete_event") { close_cb }

    #
    # File Menu
    #

    file_menu = [
      UIInfo::menu_open_item(Proc.new { self.open_cb }, nil),
      [
        Gnome::App::UI_ITEM, "prime", "Prime", 
        proc {|item, arg| self.prime_cb}, 
        'toolbar/prime', Gnome::App::PIXMAP_STOCK, Gtk::Stock::OPEN, 
        0, 0, nil
      ],
      [
        Gnome::App::UI_ITEM, "save-context", "Save Context", 
        proc {|item, arg| self.save_cb}, 
        'toolbar/save_context', Gnome::App::PIXMAP_STOCK, Gtk::Stock::SAVE, 
        0, 0, nil
      ],
      [
        Gnome::App::UI_ITEM, "save-image", "Save Image", 
        proc {|item, arg| self.save_image_cb}, 
        'toolbar/save_image', Gnome::App::PIXMAP_STOCK, Gtk::Stock::SAVE, 
        0, 0, nil
      ],
      UIInfo::menu_close_item(Proc.new { self.close_cb }, nil),
      UIInfo::menu_quit_item(Proc.new { self.quit_cb }, nil)
    ]

    space_menu = [
      [
        Gnome::App::UI_ITEM, "Inherits Space", "Inherits-in", 
        proc {|item, arg| self.choose_space("inherits-in") }, 
        'toolbar/inherits', Gnome::App::PIXMAP_STOCK, Gnome::Stock::ATTACH,
        0, 0, nil
      ],
      [
        Gnome::App::UI_ITEM, "Calls Space", "Calls-in", 
        proc {|item, arg| self.choose_space("calls-in") }, 
        'toolbar/inherits', Gnome::App::PIXMAP_STOCK, Gnome::Stock::ATTACH, 
        0, 0, nil
      ],
      [
        Gnome::App::UI_ITEM, "Prune Object Labels", "Prune Object Labels", 
        proc {|item, arg| self.prune_object_labels }, 
        'toolbar/inherits', Gnome::App::PIXMAP_STOCK, Gnome::Stock::ATTACH, 
        0, 0, nil
      ]
    ]

    help_menu = [
      UIInfo::menu_about_item(Proc.new { create_about }, nil),
    ]
     
    main_menu = [
      UIInfo::menu_file_tree(file_menu),
      UIInfo::subtree("_Space", space_menu),
      UIInfo::menu_help_tree(help_menu)
    ]

    @concept_menu = Gtk::Menu.new
    @concept_menu.append(item = Gtk::MenuItem.new("Unfold Contingent"))
    item.signal_connect("activate") {
      |item| 
      intent = @nav_controller.lattice.attribute_contingents[@view.selected_concept]
      extent = @nav_controller.lattice.object_contingents[@view.selected_concept]
      @nav_controller.zoom_to(intent, extent)
    }
    @concept_menu.append(item = Gtk::MenuItem.new("Focus on Intent"))
    item.signal_connect("activate") {
      |item| 
      intent = @nav_controller.lattice.intents[@view.selected_concept]
      @nav_controller.focus_on_attributes(intent)
    }
    @concept_menu.append(item = Gtk::MenuItem.new("Focus on Attr. Cont."))
    item.signal_connect("activate") {
      |item| 
      intent = @nav_controller.lattice.attribute_contingents[@view.selected_concept]
      @nav_controller.focus_on_attributes(intent)
    }
    @concept_menu.show_all
    @concept_menu.append(item = Gtk::MenuItem.new("Focus on Cont."))
    item.signal_connect("activate") {
      |item| 
      intent = @nav_controller.lattice.attribute_contingents[@view.selected_concept]
      extent = @nav_controller.lattice.object_contingents[@view.selected_concept]
      @nav_controller.focus_on_attributes(intent)
      @nav_controller.focus_on_objects(extent)
    }
    @concept_menu.show_all

    @object_menu = Gtk::Menu.new
    @object_menu.append(item = Gtk::MenuItem.new("Unfold"))
    item.signal_connect("activate") {
      |item| 
      puts "Unfold: #{@view.selected_object}"
      @nav_controller.unfold_object(@view.selected_object)
    }
    @object_menu.append(item = Gtk::MenuItem.new("Fold"))
    item.signal_connect("activate") {
      |item| 
      puts "Fold: #{@view.selected_object}"
      @nav_controller.fold_object(@view.selected_object)
    }
    @object_menu.append(item = Gtk::MenuItem.new("Focus On"))
    item.signal_connect("activate") {
      |item| 
      puts "Focus On Object: #{@view.selected_object}"
      @nav_controller.focus_on_object(@view.selected_object)
    }
    @object_menu.show_all

    @attribute_menu = Gtk::Menu.new()
    @attribute_menu.append(item = Gtk::MenuItem.new("Unfold"))
    item.signal_connect("activate") {
      |item| 
      puts "Unfold: #{@view.selected_attribute}"
      @nav_controller.unfold_attribute(@view.selected_attribute)
    }
    @attribute_menu.append(item = Gtk::MenuItem.new("Fold"))
    item.signal_connect("activate") {
      |item| 
      puts "Fold: #{@view.selected_attribute}"
      @nav_controller.fold_attribute(@view.selected_attribute)
    }
    @attribute_menu.append(item = Gtk::MenuItem.new("Focus On"))
    item.signal_connect("activate") {
      |item| 
      puts "Focus On Attribute: #{@view.selected_attribute}"
      @nav_controller.focus_on_attribute(@view.selected_attribute)
    }
    @attribute_menu.show_all

    create_menus(main_menu)

    callback = proc { |item, arg| @label.set_text(arg) }
    main_toolbar = [
      [
        Gnome::App::UI_ITEM, "back", "Back", 
        proc {|item, arg| self.back_cb}, 
        'toolbar/back', Gnome::App::PIXMAP_STOCK, Gtk::Stock::GO_BACK, 
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


    @toolbar = Gtk::Toolbar.new
    @toolbar.append("Prime") { self.prime_cb }
    @toolbar.append("Back")  { self.back_cb }

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

    @view.signal_connect("select-attribute") {
      puts "Attribute Selected: " + @view.selected_attribute.to_s
    }
    @view.signal_connect("select-object") {
      puts "Object Selected: " + @view.selected_object.to_s
    }
    @view.signal_connect("select-concept") {
      puts "Concept Selected: " + @view.selected_concept.to_s
    }
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
    puts "Type=#{@context.class}"
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
      |view, concept, event|
      if event.button == 3 then
        @concept_menu.popup(nil, nil, event.button, event.time)
      end
    }

    @view.signal_connect("select-attribute") {
      |view, attribute, event|
      if event.button == 3 then
        @attribute_menu.popup(nil, nil, event.button, event.time)
      end
    }

    @view.signal_connect("select-object") {
      |view, object, event|
      if event.button == 3 then
        @object_menu.popup(nil, nil, event.button, event.time)
      end
    }

    @nav_controller = NavController.new(@view)
    prime_cb
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
      @ic = interp_context_from_cxt(filename)
      @diagram = Diagram.layout(@ic.context)
      @view.set_diagram(@diagram, @ic.st_objects, @ic.st_attributes)
    end
  end

  def prune_object_labels
    st = @nav_controller.ic.st_objects
    for i in 1..st.length do
      if st[i] =~ /.*\.([a-z]+)\(/ then
        puts "Prune: '" + $1 + "'"
        st[i] = $1
      end
    end
    @view.recalc_labels
  end

  def back_cb
    @nav_controller.move_back
  end

  def save_cb
    filename = get_save_filename
    if filename != nil then
      @nav_controller.save_cxt(filename)
    end
  end

  def save_image_cb
    filename = get_save_filename
    if filename != nil then
      format = filename.gsub(/.*\.([a-z]+)$/,'\1')
      format = 'jpeg' if format == nil || format.length == 0
      puts "Filename: #{filename}, format: #{format}"
      @view.save_image(filename, format);
    end
  end

  def prime_cb
    begin
      @nav_controller.space.prime
      @nav_controller.primeView
    rescue NoMethodError => err
      puts "Unable to prime nav controller: class=" + err.to_s
    end
  end

  def choose_space(relation)
    begin
      @nav_controller.set_relation(relation)
      @nav_controller.space.prime
      @nav_controller.primeView
    rescue NoMethodError => err
      puts "Unable to set relation and prime nav controller: class=" + err.to_s
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

    if dialog.run == Gtk::Dialog::RESPONSE_ACCEPT
      result = dialog.filename
    end
    dialog.destroy

    return result
  end

  def get_save_filename
    result = nil
    dialog = Gtk::FileChooserDialog.new(
      "Save File", parent_window,
      Gtk::FileChooser::ACTION_SAVE, nil,
      [Gtk::Stock::CANCEL, Gtk::Dialog::RESPONSE_CANCEL],
      [Gtk::Stock::SAVE, Gtk::Dialog::RESPONSE_ACCEPT]
    )

    if dialog.run == Gtk::Dialog::RESPONSE_ACCEPT
      result = dialog.filename
    end
    dialog.destroy

    return result
  end

  def create_about
    about = About.new("SarlNavSpaceViewer", 
      VERSION, COPYRIGHT, DESCRIPTION, AUTHORS, DOCUMENTORS, nil
    )
    about.show
  end
end


