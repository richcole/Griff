 
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


# This is a comment right?

require 'util.rb'

$black_pen   = Pen.new(Qt::black, 5)
$black_brush = Brush.new(Qt::black)
$white_brush = Brush.new(Qt::white)
$gray_brush = Brush.new(Qt::gray)

def black_pen
  return $black_pen
end

def black_brush
  return $black_brush
end

def gray_brush
  return $gray_brush
end

def white_brush
  return $white_brush
end

def shaveRect r, c
  return Rect.new(r.x+c, r.y+c, r.width-(2*c), r.height-(2*c))
end

def bulkRect r, c
  return Rect.new(r.x-c, r.y-c, r.width+(2*c), r.height+(2*c))
end

