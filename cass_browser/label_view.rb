 
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


require 'canvas_events_handler' 

class LabelView < CanvasRectangle

  include Model
  include CanvasEventsHandler
	
	attr_reader :is_mouse_over

  def initialize tr, canvas, concept_view, xs, interp, dx, dy, is_attr=true
    super(canvas)
    init_model

    @is_attr = is_attr
    @xs = xs
    @interp = interp

    @font = Font.new("Comic", self.font_size(tr.s))
    @font_metrics = FontMetrics.new(@font)
    @tr = tr
    @tr.subscribe(self)

    setZ(10)

    @is_mouse_over = false
    @concept_view = concept_view
    @concept_view.subscribe(self)
    @offset = [dx, dy]
    notify(@concept_view)

    change_size 1.0, 1.0
    show

    if is_attr then
      @line_view = LineView.new(tr, canvas, self, concept_view, 0.01)
    else
      @line_view = LineView.new(tr, canvas, concept_view, self, 0.01)
    end
  end

  def unit
    @font_metrics.height
  end

  def change_size width_percent, height_percent
		@width_percent = width_percent
		@height_percent = height_percent
    rescale
  end

  def rescale
		s = max_size
    setSize( @width_percent * s.width,  @height_percent * s.height )
  end

  def curr_size
    return size
  end

  def min_size
    height = (margin * 2) + tr_pen_width + unit
    width = (margin * 2) + tr_pen_width + 4 * unit
    return Size.new(width, height)
  end

  def max_size
    width = 0
    num_lines = 0
    for x in @xs do
      num_lines += 1
      curr_line_width = @font_metrics.boundingRect(@interp.short_name(x)).width
      if @interp.is_foldable then
        curr_line_width += @font_metrics.height
        width = max_of(width, curr_line_width)
      else
        width = max_of(width, curr_line_width)
      end
    end
    height = ((num_lines - 1 ) * @font_metrics.lineSpacing) +
      @font_metrics.height
    height += (margin * 2) + tr_pen_width
    width  += (margin * 2) + tr_pen_width
		
    if is_mouse_over then
      width = max_of(width, curr_line_width + 4 * unit / 3)
    end
    return Size.new(width.to_i, height.to_i)
  end

  def dispose
    @tr.unsubscribe(self)
    super
  end

  def font_size s
    return max_of(6, 0.3 * s)
  end

  def tr_pen_width
    return 0.02*@tr.s
  end

  def margin
    return unit/4
  end

  def draw painter
    painter.setPen(Pen.new(black, tr_pen_width))
    painter.setBrush(white_brush)
    painter.setFont(@font)

    r = shaveRect(boundingRect, tr_pen_width)
    painter.drawRect(r)
    line_num = 1
    pos_y = (r.top + @font_metrics.ascent + margin).to_i
    pos_x = (r.left + margin).to_i
		text_width = r.width - (margin*2) - unit
		text_height = @font_metrics.height
    for x in @xs do
      if @interp.is_foldable then
        if @interp.is_unfolded(x) then
          drawUnfoldedIcon(painter, pos_x, pos_y - (@font_metrics.ascent))
        else
          drawFoldedIcon(painter, pos_x, pos_y - (@font_metrics.ascent))
        end
        painter.drawText(pos_x + @font_metrics.height, pos_y, @interp.short_name(x))
      else
        painter.drawText(pos_x, pos_y, @interp.short_name(x))
      end
      pos_y += @font_metrics.lineSpacing
    end
    
    if is_mouse_over then
      h = @font_metrics.height
      pos_x = r.right - h
      pos_y = r.bottom - h
      drawResizeTab(painter, Rect.new(pos_x, pos_y, h, h))
    end
  end

  def drawFoldedIcon painter, pos_x, pos_y
    icon_unit = (@font_metrics.height / 4).to_i

    left = (pos_x + icon_unit).to_i 
    top = (pos_y + icon_unit).to_i
    w = (2 * icon_unit).to_i
    h = (2 * icon_unit).to_i
    bottom = top + h
    right = left + w
    painter.drawLine(left, top, right, top)
    painter.drawLine(right, top, right, bottom)
    painter.drawLine(left, bottom, right, bottom)
    painter.drawLine(left, top, left, bottom)
    painter.drawLine(left + icon_unit, top, left + icon_unit, bottom)
    painter.drawLine(left, top + icon_unit, right, top + icon_unit)
  end

  def drawUnfoldedIcon painter, pos_x, pos_y
    icon_unit = (@font_metrics.height / 4).to_i

    left = (pos_x + icon_unit).to_i 
    top = (pos_y + icon_unit).to_i
    w = (2 * icon_unit).to_i
    h = (2 * icon_unit).to_i
    bottom = top + h
    right = left + w
    painter.drawLine(left, top, right, top)
    painter.drawLine(right, top, right, bottom)
    painter.drawLine(left, bottom, right, bottom)
    painter.drawLine(left, top, left, bottom)
    painter.drawLine(left, top + icon_unit, right, top + icon_unit)
  end

  def drawResizeTab painter, r
    painter.drawLine(r.left, r.bottom, r.right, r.bottom)
    painter.drawLine(r.right, r.bottom, r.right, r.top)
    painter.drawLine(r.left, r.bottom, r.right, r.top)
  end

  def notify c
    if c == @concept_view then
      sx = @concept_view.x + (@tr.s * @offset[0])
      sy = @concept_view.y + (@tr.s * @offset[1])
      move(sx, sy)
      set_modified
    else
      @font.setPixelSize(self.font_size(@tr.s))
      @font_metrics = FontMetrics.new(@font)
      rescale
      set_modified
    end
  end

  def hitable
    return true
  end

  def mousePressEvent event
    case event.button
    when Qt::LeftButton then
      @prev_pt = event.pos
    when Qt::RightButton then
      y = event.pos.y - boundingRect.top
      index = (y / @font_metrics.lineSpacing).to_i
      x_array = []
      for x in @xs do
        x_array.push(@interp.full_name(x))
      end
      if index < x_array.length then
        if @is_attr then
          canvas.nav_controller.attribute_menu(event, x_array[index])
        else
          canvas.nav_controller.object_menu(event, x_array[index])
        end
      end
    end
  end

  def mouseReleaseEvent event
    case event.button
    when Qt::LeftButton then
      @prev_pt = nil
    end
  end

  def mouseMoveEvent event
    p = event.pos
    if @prev_pt then
      dx = (p.x - @prev_pt.x) / @tr.s
      dy = (p.y - @prev_pt.y) / @tr.s
      @offset[0] += dx
      @offset[1] += dy
      notify(@concept_view)
      @prev_pt = p
    end
  end

  def mouseEnter event
    @is_mouse_over = true
		rescale
		update
		canvas.update
  end

  def mouseLeave event
    @is_mouse_over = false
		rescale
		update
		canvas.update
  end

  def mouseOver event
  end

  def center
    return boundingRect().center
  end

end

