require 'griff/canvas_text'
require 'util'

module Griff

  class CanvasMultiText < CanvasRect

    attr_reader :text

    def initialize(
      canvas, text=[], top_left=Point.new(0, 0), z=0, 
      style=CanvasTextStyle.new()
    )
      @text = text

      @num_lines = text.size()
      @num_visible_lines = min_of(3, @num_lines)
      @first_line = 0

      s = calculate_text_size(text, style)
      r = Rect.new(top_left.x, top_left.y, s.width, s.height)
      
      super(canvas, r, z, style)

      @is_mouse_over = false
      @text_rect = rect
      @resize_rect = calculate_resize_rect(@text_rect)

      update_scroll_details

      @scroll_rect = calculate_scroll_rect(@text_rect)
      @scroll_widget_rect = calculate_scroll_widget_rect(@scroll_rect)
    end

    def update_scroll_details
      @scroll_min = 0
      @scroll_max = max_of(0, @num_lines - @num_visible_lines)
      @scroll_page_size = @num_visible_lines
      @scroll_position = @first_line
    end

    def scroll_to (value)
      @first_line = value.to_i
    end

    def calculate_text_plus_scroll_size(text, style)
      s = calculate_text_size(text, style)
      if @is_mouse_over then
        s.width += 10
      end
      return s
    end

    def check_for_overflow
      if @first_line > 0 && @first_line + @num_visible_lines > @num_lines then
        new_scroll_pos = @first_line + (@num_lines - (@first_line + @num_visible_lines))
        new_scroll_pos = max_of(0, new_scroll_pos)
        scroll_to(new_scroll_pos)
      end
    end

    def calculate_text_size(text, style)
      s = Qt::Size.new
      nh = (@num_visible_lines * style.height) + 
        style.margin + style.margin + 1
      for i in 1..text.size do
        r = style.boundingRect(text[i-1])
        nw = r.left + r.width + style.margin + style.margin + 3
        s.width  = max_of(nw, s.width)
      end
      s.height = max_of(nh, s.height)
      return s
    end

    def calculate_rect
      r = rect
      s = calculate_text_plus_scroll_size(@text, @style)
      return Rect.new(r.left, r.top, s.width, s.height)
    end

    def calculate_resize_rect(text_rect)
      @resize_rect = Rect.new(text_rect.right, text_rect.bottom - 10, 10, 10)
    end

    def calculate_text_rect
      r = rect
      s = calculate_text_size(@text, @style)
      return Rect.new(r.left, r.top, s.width, s.height)
    end

    def mouse_enter
      self.is_mouse_over = true
    end

    def mouse_leave
      self.is_mouse_over = false
    end

    def is_mouse_over=(is_over)
      @is_mouse_over = is_over
      recalculate_rect
    end

    def update_dependent_rects
      @resize_rect = calculate_resize_rect(@text_rect)
      @num_visible_lines = calculate_num_visible_lines
      update_scroll_details
      @scroll_rect = calculate_scroll_rect(@text_rect)
      @scroll_widget_rect = calculate_scroll_widget_rect(@scroll_rect)
    end

    def calculate_num_visible_lines
      return (@text_rect.height.div(@style.height))
    end

    def recalculate_rect
      r = @text_rect
      update_dependent_rects
      if @is_mouse_over then
        r = r.union(@resize_rect)
      end
      self.rect = r
    end

    def drag_start(p)
      if @scroll_widget_rect.include?(p) then
        @dragging = :scroll_widget_dragging
      end
      if @resize_rect.include?(p) then
        @dragging = :resize_dragging
      end 
      if @text_rect.include?(p) then
        @dragging = :visible_dragging
      end
   end

    def drag_stop(p)
      @dragging = :none
    end

    def drag(dx)
      case @dragging
        when :visible_dragging
          move_by(dx)
        when :resize_dragging 
          resize_by(dx)
        when :scroll_widget_dragging
          scroll_by(dx)
      end
    end

    def resize_by(dx)
      @text_rect = @text_rect.resize_by(dx)
      s = calculate_text_size(text, style)
      @text_rect = @text_rect.min_max(10, style.height, s.width + 10, s.height)
      recalculate_rect
      check_for_overflow
    end

    def move_by(dx)
      @text_rect = @text_rect.move_by(dx)
      recalculate_rect
    end

    def move_to(p)
      @text_rect = @text_rect.move_to(p)
      recalculate_rect
    end

    def text=(text)
      @text = text 
      recalculate_rect
    end

    def drawText(painter, is_postscript_printing)
      style.fashion(painter, is_postscript_printing)
      painter.save
      painter.setClipping(true)
      painter.setClipRect(
        @text_rect.left, @text_rect.top, @text_rect.width, @text_rect.height, 
        Qt::Painter::CoordPainter
      )
      end_line = (@first_line + @num_visible_lines - 1)
      start_line = @first_line
      curr_line_number = 1

      if start_line != 0 then
        painter.drawText(
          @rect.left + style.left, 
          @rect.top + style.baseline + ((curr_line_number-1) * style.height), 
          "#{start_line+1} more before ..."
        )
        start_line += 1
        curr_line_number += 1
      end

      if end_line != @num_lines-1 then
        end_line -= 1
      end

      for i in start_line..end_line do
        painter.drawText(
          @rect.left + style.left, 
          @rect.top + style.baseline + ((curr_line_number-1) * style.height), 
          text[i]
        )
        curr_line_number += 1
      end

      if end_line != @num_lines-1 then
        painter.drawText(
          @rect.left + style.left, 
          @rect.top + style.baseline + ((curr_line_number-1) * style.height), 
          "#{@num_lines - end_line - 1} more after ..."
        )
        curr_line_number += 1
      end

      painter.restore
    end

    def draw(painter, is_postscript_printing)
      super(painter, is_postscript_printing)
      drawText(painter, is_postscript_printing)
      if @is_mouse_over then 
        drawScrollbar(painter, is_postscript_printing)
        drawResizeRect(painter, is_postscript_printing)
      end
    end

    def calculate_scroll_rect(text_rect)
      return Rect.new(text_rect.right-1, text_rect.top, 10, text_rect.height - 10)
    end

    def drawScrollbar(painter, is_postscript_printing)
      r = @scroll_rect

      painter.drawRect(r.left, r.top, r.width+1, r.height)

      if r.height > 30 then
        c  = (r.left + r.right) .div 2
        bt = r.bottom - 10
        bb = r.top + 10

        drawTriangle(painter, r.left, bb, c, r.top, r.right, bb)
        drawTriangle(painter, r.left, bt, c, bt+10, r.right, bt)

        drawScrollControl(painter, r)
      end
    end

    def drawResizeRect(painter, is_postscript_printing)
      painter.drawRect(@scroll_rect.left, @scroll_rect.bottom-1, 11, 11);
    end

    def drawTriangle(painter, a, b, c, d, e, f)
      painter.drawLine(a,b,c,d)
      painter.drawLine(c,d,e,f)
      painter.drawLine(e,f,a,b)
    end

    def drawScrollControl(painter, r)
      r = calculate_scroll_widget_rect(r)
      painter.drawEllipse(r.left, r.top, r.width, r.height)
    end

    def calculate_uparrow_rect(r)
      if @num_visible_lines > 2 then
        return Rect.new(r.left, r.top, 10, 10)
      else
        return Rect.new(r.left, t.top, 0, 0)
      end
    end

    def calculate_downarrow_rect(r)
      if @num_visible_lines > 2 then
        return Rect.new(r.left, r.bottom-19, 10, 10)
      else
        return Rect.new(r.left, t.top, 0, 0)
      end
    end

    def calculate_scroll_widget_rect(r)
      @scroll_range = @scroll_max - @scroll_min
      @widget_space = r.height - 20
      if (@scroll_range + @scroll_page_size) > 0 then
        @widget_height = max_of(
          8, 
          (@scroll_page_size.to_f / (@scroll_page_size + @scroll_range)) * @widget_space
        )
        @remaining_space = @widget_space - @widget_height
        @widget_pos = (@widget_space * @scroll_position).div(@scroll_page_size + @scroll_range)
      else
        @widget_height = @widget_space
        @widget_pos = 0
        @remaining_space = 0
      end
      return Rect.new(r.left+1, r.top + 10 + @widget_pos, r.width-1, @widget_height)
    end

    def scroll_by(dx)
      if @remaining_space > 0 then
        scroll_amount = @scroll_range * (dx.y.to_f / @remaining_space)
        @scroll_position += scroll_amount
        @scroll_position = max_of(@scroll_position, @scroll_min)
        @scroll_position = min_of(@scroll_position, @scroll_max)
        scroll_to(@scroll_position)
      end
    end

  end

end
