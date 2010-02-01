 
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


require 'cairo'

class SplineWindow < Cairo::Xlib::Window
  def initialize(dpy)
    @width = @height = 400
    super(dpy, @width, @height, false)
  end

  def randx
    10 + rand(@width-20)
  end
  def randy
    10 + rand(@height-20)
  end

  def draw
    @cairo.set_rgb_color(1, 1, 1)
    @cairo.line_width = 2 + ((rand**2.0) * 50)
    @cairo.stroke {
      @cairo.move_to(randx, randy)
      @cairo.curve_to(randx, randy, randx, randy, randx, randy)
    }
  end

  def keypress(key)
    case key.chr
    when 'q'
      @dpy.quit
    end
  end
end

dpy = Cairo::Xlib::Display.new
sw = SplineWindow.new(dpy)
dpy.mainloop
