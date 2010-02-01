#!/usr/bin/ruby -w
 
# Copyright 2004, Richard Cole
#
# This file is part of Griff.
# 
# Griff is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 2 of the License, or (at your
# option) any later version.
# 
# Griff is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
# 
# You should have received a copy of the GNU General Public License
# along with Griff; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


require 'Qt'
include Qt

require 'formal_context'
require 'formal_context_formats'
require 'concept_lattice'
require 'diagram'

require 'nav_space'
require 'nav_controller'
require 'model'
require 'concept_view'
require 'line_view'
require 'label_view'
require 'transform'
require 'lattice_canvas_view'
require 'lattice_canvas'

class MyMainWindow < MainWindow

  slots 'fileOpen()', 'fileSave()', 'primeCanvas()', 'print()', 'printerSetup()'

  attr_reader :canvas_view, :printer

  def initialize(name)
    @printer = nil

    super(nil, name)
    setCaption("CASS Browser")
    setup_menu_bar
    setup_canvas
    setup_staus_bar
  end

  def setup_canvas
    @canvas = LatticeCanvas.new
    @canvas_view = LatticeCanvasView.new(@canvas, self, "Canvas View", 0)

    @canvas_view.setCanvas(@canvas)
    setCentralWidget(@canvas_view)
    @canvas_view.show
    
    begin
      @canvas.load_cxt_file("b2.cxt")
    rescue SystemCallError
    end
  end

  def setup_menu_bar
    # Create menu bar
    @mbar = menuBar
    
    # Create and populate file menu
    @menu = PopupMenu.new()
    @menu.insertItem("Open", self, SLOT('fileOpen()'))
    @menu.insertItem("Prime", self, SLOT('primeCanvas()'))
    @menu.insertSeparator()
    @menu.insertItem("Save Context", self, SLOT('fileSave()'))
    @menu.insertSeparator()
    @menu.insertItem("Print", self, SLOT('print()'))
    @menu.insertItem("Print Setup", self, SLOT('printerSetup()'))
    @menu.insertSeparator()
    @menu.insertItem("Exit", $qApp, SLOT('quit()'))
    
    # Add file menu to menu bar
    @mbar.insertItem("&File", @menu)
  end

  def setup_staus_bar
    @sbar = statusBar
  end

  def fileOpen()
    filename = FileDialog.getOpenFileName(nil, "Cxt (*.cxt)", 
       self, "file open", "Load CXT File")
    if ! filename.nil?
      @canvas.load_cxt_file(filename)
      statusBar.message("Loaded #{filename}")
    else
      statusBar().message("File Open Cancelled")
    end
  end

  def fileSave()
    filename = FileDialog.getSaveFileName(nil, "Cxt (*.cxt)", 
       self, "file save", "Save CXT File")
    if ! filename.nil?
      @canvas.write_cxt_file(filename)
      statusBar.message("Saved #{filename}")
    else
      statusBar().message("File Save Cancelled")
    end
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
      canvas_view.print(@printer)
    end
  end
  
  def status_message message
    statusBar.message(message)
  end
  
  def print_setup()
    init_printer
  end
  

  def primeCanvas
    puts "PrimeCanvas"
    @canvas.nav_controller.space.prime
    @canvas.nav_controller.primeCanvas
  end

end

app = Application.new(ARGV)

main_window = MyMainWindow.new("Canvas")

app.mainWidget = main_window
main_window.show()
app.exec()
