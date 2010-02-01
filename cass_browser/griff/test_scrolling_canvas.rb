require 'Qt'
require 'griff/scrolling_canvas_view'

include Qt

app = Qt::Application.new(ARGV)

class GriffDemoMainWindow < Qt::MainWindow

  def initialize(parent, name)
    super(parent, name)
    setup_menu
    setup_main_widget
  end

  def setup_main_widget
    @canvas_view = Griff::ScrollingCanvasView.new(self, "")
    setCentralWidget(@canvas_view)
  end

  def setup_menu
    @mbar = menuBar
    
    # Create and populate file menu
    @menu = PopupMenu.new()
    @menu.insertSeparator()
    @menu.insertItem("Exit", $qApp, SLOT('quit()'))
    
    # Add file menu to menu bar
    @mbar.insertItem("&File", @menu)
  end

  def status_message message
    statusBar.message(message)
  end
  
end

main_window = GriffDemoMainWindow.new(nil, "Canvas")

app.mainWidget = main_window
main_window.show()
app.exec()
