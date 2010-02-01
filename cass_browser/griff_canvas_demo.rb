require 'Qt'
require 'griff/canvas_view'
require 'griff/canvas_view_tests'
require 'griff/canvas'

include Qt

app = Qt::Application.new(ARGV)

class GriffDemoMainWindow < Qt::MainWindow

  slots 'print()', 'printerSetup()'

  attr_reader :printer

  def initialize(parent, name)
    super(parent, name)
    setup_menu
    setup_main_widget

    @printer = nil
  end

  def setup_main_widget
    @canvas_view = Griff::CanvasView.new(self)
    setCentralWidget(@canvas_view)
  end

  def populate
    utils = Griff::TestUtils.new
    utils.test_populate(@canvas_view.canvas)
  end

  def setup_menu
    @mbar = menuBar
    
    # Create and populate file menu
    @menu = PopupMenu.new()
    @menu.insertSeparator()
    @menu.insertItem("Print", self, SLOT('print()'))
    @menu.insertItem("Print Setup", self, SLOT('printerSetup()'))
    @menu.insertSeparator()
    @menu.insertItem("Exit", $qApp, SLOT('quit()'))
    
    # Add file menu to menu bar
    @mbar.insertItem("&File", @menu)
  end

  def printerSetup
    if not printer then
      init_printer
    else
      if not printer.setup then
        status_message("Printer setup cancelled.")
      end
    end
  end  

  def init_printer
    @printer = Printer.new
    if not @printer.setup then
      @printer.dispose
      @printer = nil
      status_message("Printer setup cancelled.")
    end
    return @printer
  end
  
  def print()
    if not @printer then
      printer = init_printer
    end
    
    if @printer then
      status_message("Printing...")
      painter = Painter.new(@printer)
      metrics = PaintDeviceMetrics.new(@printer)
      transform = @canvas_view.calculate_transform(
        metrics.width, metrics.height
      )
      painter.setWorldMatrix(transform, false)
      @canvas_view.canvas.draw(painter, true)
      painter.end
      status_message("Finished Printing.")
    end
  end
  
  def status_message message
    statusBar.message(message)
  end
  
end

main_window = GriffDemoMainWindow.new(nil, "Canvas")
main_window.populate

app.mainWidget = main_window
main_window.show()
app.exec()
