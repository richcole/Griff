 
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


class LatticeCanvasView < CanvasView

  def initialize *args
    super(*args)
    viewport.setMouseTracking(true)
  end

  def transformEvent(e)
    p = self.inverseWorldMatrix.map(e.pos)
    return MouseEvent.new(e.type, p, e.globalPos, e.button, e.state)
  end

  def contentsMousePressEvent(event)
    event = transformEvent(event)
    canvas.mousePressEvent(event)
  end

  def contentsMouseReleaseEvent(event)
    event = transformEvent(event)
    canvas.mouseReleaseEvent(event)
  end

  def contentsMouseMoveEvent(event)
    event = transformEvent(event)
    canvas.mouseMoveEvent(event)
  end

  def viewportResizeEvent(e)
    if canvas then
      canvas.resize(e.size.width, e.size.height)
      canvas.reset_scale
      canvas.update
    end
  end
	
  def print printer
    painter = Painter.new
    painter.begin(printer)
    canvas.setDoubleBuffering(false);
    drawContents(painter, 0, 0, canvas.width, canvas.height)
    canvas.setDoubleBuffering(true);
    painter.end()
    printer.newPage
  end
		
end

