require 'Qt'
require 'griff/scrolling_canvas_view'
require 'griff/canvas_view'
require 'griff/canvas_view_tests'
require 'griff/canvas'
require 'profiler'

require 'griff/canvas_lattice_controller'

class CassBrowserMainWindow < Qt::MainWindow

  slots 'fileOpen()', 'print()', 'printerSetup()', 'doubleBuffering()', 'printProfile()'

  attr_reader :printer, :canvas_view

  def initialize(parent, name)
    super(parent, name)
    setup_main_widget
    setup_menu

    @printer = nil
  end

  def setup_main_widget
    @canvas_view = Griff::ScrollingCanvasView.new(self, "CanvasView")
    setCentralWidget(@canvas_view)
  end

  def setup_menu
    @mbar = menuBar
    
    # Create and populate file menu
    @menu = PopupMenu.new()
    @menu.insertItem("Open", self, SLOT('fileOpen()'))
    @menu.insertSeparator()
    @menu.insertItem("Print", self, SLOT('print()'))
    @menu.insertItem("Print Setup", self, SLOT('printerSetup()'))
    @menu.insertSeparator()
    @menu.insertItem("Print Profile", self, SLOT('printProfile()'))
    @menu.insertItem("Exit", $qApp, SLOT('quit()'))

    # Add file menu to menu bar
    @mbar.insertItem("&File", @menu)

    # Create the view menu
    @view_menu = PopupMenu.new
    @double_buffering_id = 
        @view_menu.insertItem(
          "Use Double Buffering", self, SLOT('doubleBuffering()')
        )
    @view_menu.setItemChecked(
        @double_buffering_id, @canvas_view.double_buffering
    )
    @view_menu.setCheckable(true)
    
    @mbar.insertItem("&View", @view_menu)
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

  def printProfile()
    Profiler__::print_profile($stdout)
  end
  
  def print()
    if not @printer then
      printer = init_printer
    end
    
    if @printer then
      status_message("Printing...")
      painter = Painter.new(@printer)
      metrics = PaintDeviceMetrics.new(@printer)
      transform = @canvas_view.calculate_transform(metrics.width, metrics.height)
      painter.setWorldMatrix(transform, false)
      @canvas_view.canvas.draw(painter)
      painter.end
      status_message("Finished Printing.")
    end
  end
  
  def fileOpen()
    filename = FileDialog.getOpenFileName(nil, "Cxt (*.cxt)", self, "file open", "Load CXT File")
    if ! filename.nil?
      load_file(filename)
    else
      statusBar().message("File Open Cancelled")
    end
  end

  def load_file(filename)
    controller = Griff::CanvasLatticeController.new(canvas_view, canvas_view.canvas)
    controller.load_cxt_file(filename)
    statusBar.message("Loaded #{filename}")
  end

  def status_message message
    statusBar.message(message)
  end

  def doubleBuffering
    new_status = !@view_menu.isItemChecked(@double_buffering_id)
    @view_menu.setItemChecked(@double_buffering_id, new_status)
    @canvas_view.double_buffering = new_status
  end
      
    
  
end





