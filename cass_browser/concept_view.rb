 
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


require 'pens'
require 'canvas_events_handler'

include Qt

class ErrorDisposed
end

class ConceptView < CanvasEllipse

  include Model
  include CanvasEventsHandler

  attr_reader :concept

  def dispose
    @tr.unsubscribe(self)
    super
  end

  def initialize tr, x, y, canvas, concept, broker
    init_model
    super(2.0 * tr.s, 2.0 * tr.s, canvas)
    setZ(5)
    @tr = tr
    @concept = concept
    @broker = broker
    set_pos(x,y)
    updateSize
    show

    @tr.subscribe self
  end

  def tr x, y
    @tr.tr(x,y)
  end

  def itr x, y
    @tr.itr(x,y)
  end

  def set_pos(x, y)
    @pos = [x,y]
    p = tr(x,y)
    move(p[0], p[1])
    set_modified
  end

  def tr_pen_width
    return 0.1*@tr.s
  end

  def draw painter
    painter.setPen(Pen.new(black, tr_pen_width))
    painter.setBrush(gray_brush)

    r = shaveRect(boundingRect, tr_pen_width)
    painter.drawEllipse(r)
  end

  def mousePressEvent event
    case event.button & ( Qt::LeftButton | Qt::RightButton )
    when Qt::LeftButton then 
      @prev_pt = event.pos
      puts "Left Button Press"
    when Qt::RightButton then
      @prev_pt = nil
      canvas.nav_controller.concept_menu(event, @concept)
    end
  end

  def mouseReleaseEvent event
    @prev_pt = nil
  end

  def mouseMoveEvent event
    p = event.pos
    if @prev_pt then
      dx = p.x - @prev_pt.x
      dy = p.y - @prev_pt.y
      @broker.conceptMoveBy(@concept, dx, dy)
      @prev_pt = p
    end
  end

  def notify src
    p = tr(@pos[0],@pos[1])
    move(p[0], p[1])
    updateSize
    set_modified
  end

  def updateSize
    setSize(0.8 * @tr.s, 0.8 * @tr.s)
  end

  def hitable
    return true
  end

  def center
    return boundingRect().center()
  end

end

