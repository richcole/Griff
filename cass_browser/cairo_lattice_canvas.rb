#!/usr/bin/env ruby

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

require 'gtk2'
require 'cairo'
require 'gtkcairo'
require 'formal_context_examples'
require 'diagram'
require 'concept_lattice'
require 'interps'
require 'formal_context_formats'

class LatticeWindow < Gtk::Window

  def create_menu vb
    menubar = Gtk::MenuBar.new
    vb.pack_start(menubar, false, false, 0)

    # File Menu
    file_item = Gtk::MenuItem.new( "File" )
    menubar.append( file_item )
    
    file_menu = Gtk::Menu.new
    file_item.set_submenu( file_menu )

    # File/Open
    file_open_item = Gtk::MenuItem.new( "Open" )
    file_menu.append( file_open_item )
    file_open_item.signal_connect( "activate" ) { self.file_open }
  end

  def file_open
    chooser = Gtk::FileSelection.new("Load cxt file.")
    chooser.run
    self.load_context_from_file(chooser.filename)
    chooser.close
  end

  def initialize
    super

    set_default_size(200, 200)

    vb = Gtk::VBox.new(FALSE, 5)
    vb.border_width = 10

    self.create_menu(vb)
    self.create_canvas(vb)

    self.load_context(make_B2)
    vb.show_all
  end

  def create_canvas vb
    @gc = Gtk::CairoWidget.new
    signal_connect('destroy') { Gtk.main_quit }
    @gc.signal_connect('paint') { paint }

    vb.pack_start(@gc, TRUE, TRUE, 0)
    add(vb)
  end

  def load_context context
    @context   = context
    @lattice   = ConceptLattice.new(@context)
    @rep = MeetIrrRep.new(@lattice)
    @pos = AdditiveDiagram.new(@rep)
    @interp = IdentInterp.new
  end

  def load_context_from_file filename
    File.open(filename) { |file|
      interp_context = load_formal_context_from_cxt_file(file)
      @context = interp_context.context 
      @title = interp_context.title
      @object_interp = interp_context.object_interp
      @attribute_interp = interp_context.attribute_interp
      @lattice   = ConceptLattice.new(@context)
      @rep = MeetIrrRep.new(@lattice)
      @pos = AdditiveDiagram.new(@rep)
    }
  end

  def paint_window_bg
    cr = @gc.cairo
    cr.save
    cr.set_rgb_color(1, 1, 1)
    cr.rectangle(0,0,@gc.allocation.width, @gc.allocation.height)
    cr.fill
    cr.restore
  end

  def paint_window_size
    str = "Size: #{@gc.allocation.height}x#{@gc.allocation.width}"
    cr = @gc.cairo
    cr.save
      cr.set_rgb_color(1, 0, 0)
      cr.move_to(100, 100)
      cr.scale_font(12)
      cr.show_text(str)
    cr.restore
  end

  def paint
    cr = @gc.cairo
    self.paint_window_bg
    self.paint_lines
    self.paint_concepts
    self.paint_contingents
    # self.paint_window_size
  end

  def transform p
    sx = 50
    sy = 50
    ox = 50
    oy = 50
    return [ox + (sx * p[0]), oy + (sy * p[1])]
  end

  def paint_concept c
    cr = @gc.cairo
    p = self.transform(@pos[c])
    radius = 10
    cr.set_rgb_color(0, 0, 0)
    cr.arc(p[0], p[1], radius, 0, 2*3.14159)
    cr.fill
  end

  def attribute_label c
    s = ""
    for m in @lattice.concept(c).attribute_contingent do
      s << @attribute_interp.attribute_name(m) << " "
    end
    return s
  end

  def paint_attribute_contingent c
    cr = @gc.cairo
    p = self.transform(@pos[c])
    o = [0, -10]
    radius = 10
    cr.save
    cr.scale_font(12)
    label = self.attribute_label(c)
    ext = cr.text_extents(label)
    w = ext["width"]
    h = ext["height"]
    x = ext["x_advance"]
    y = ext["y_advance"]
    cr.rectangle(p[0] + o[0], p[1] + o[1] - (h + y), w, h)
    cr.set_rgb_color(0.8, 0.7, 0.7)
    cr.fill
    cr.set_rgb_color(0, 0, 0)
    cr.move_to(p[0] + o[0], p[1] + o[1])
    cr.show_text(label)
    cr.restore
  end    

  def paint_line a, b
    cr = @gc.cairo
    pa = self.transform(@pos[a])
    pb = self.transform(@pos[b])
    cr.save
    cr.line_width = 5
    cr.set_rgb_color(0, 0, 0)
    cr.move_to(pa[0],pa[1])
    cr.line_to(pb[0],pb[1])
    cr.stroke
    cr.restore
  end

  def paint_concepts
    for x in @lattice do
      self.paint_concept(x)
    end
  end

  def paint_contingents
    for c in @lattice do
      if @lattice.concept(c).attribute_contingent.count != 0 then
        self.paint_attribute_contingent c
      end
    end
  end

  def paint_lines
    for x in @lattice do
      for y in @lattice.lower_covers[x] do
        paint_line(x,y)
      end
    end
  end

end

Gtk.init
win = LatticeWindow.new
win.show
Gtk.main
