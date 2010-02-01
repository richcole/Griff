 
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


require 'canvas_events_handler.rb'

class LineView < CanvasLine

  include CanvasEventsHandler

  attr_reader :pen_width, :pen_color
  attr_writer :pen_width, :pen_color

  def initialize tr, canvas, from_item, to_item, pen_width=0.1
    super(canvas)
    @tr = tr
    @pen_width = pen_width
    @pen_color = red

    tr.subscribe(self)
    setPen(black_pen)
    setBrush(black_brush)

    @from_item = from_item
    @to_item = to_item
    @from_item.subscribe(self)
    @to_item.subscribe(self)

    setPoints(@from_item.x, @from_item.y, @to_item.x, @to_item.y)
    show
  end

  def tr_pen_width
    return max_of(1, (pen_width * @tr.s).to_i)
  end

  def notify src
    if src == @from_item || src == @to_item then
      p1 = @from_item.center
      p2 = @to_item.center
      setPoints(p1.x, p1.y, p2.x, p2.y)
      if p2.y < p1.y then
        self.pen_color = red
      else
        self.pen_color = black
      end
    end
    setPen(Pen.new(self.pen_color, tr_pen_width))
  end

  def dispose
    @tr.unsubscribe(self)
    super
  end

  def hitable
    return false
  end

end

