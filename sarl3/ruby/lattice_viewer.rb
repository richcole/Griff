require 'gnome2'
require 'sarl'
require 'sarlrbgtk'
require 'cxt_file_format'
require 'decorated_lattice_view'

include Gnome
include SarlGtk
include Sarl


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

    self.create_view
  end

  def create_appbar
    self.statusbar = Gnome::AppBar.new(false, true, Gnome::PREFERENCES_USER)
  end

  def create_view
    @lattice_view = DecoratedLatticeView.new
    set_contents(@lattice_view)
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
      @lattice_view.set_interp_context(interp_context_from_cxt(filename))
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


