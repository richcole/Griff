require 'griff/canvas_rect'

module Griff

  class CanvasTextStyle < CanvasRectStyle
    attr_accessor :pen, :font

    def initialize(pen_width=10, font_name="Comic", font_size=12)
      super(pen_width)
      @font = Qt::Font.new(font_name, font_size)
      @metrics = Qt::FontMetrics.new(@font)
    end

    def boundingRect(text)
      r = @metrics.boundingRect(text)
      return r
    end

    def fashion(painter, is_postscript_printing)
      super(painter, is_postscript_printing)
      painter.setFont(@font)
    end

    def baseline
      return @metrics.leading + @metrics.ascent
    end

    def left
      return margin + 1
    end

    def height
      return @metrics.height
    end

    def margin
      return @metrics.leading
    end

  end

  class CanvasText < CanvasRect

    attr_reader :text

    def initialize(
      canvas, text="", top_left=Point.new(0, 0), z=0, 
      style=CanvasTextStyle.new()
    )
      @text = text
      s = calculate_size(text, style)
      r = Rect.new(top_left.x, top_left.y, s.width, s.height)
      super(canvas, r, z, style)
      puts "style.margin=#{style.margin}"
    end

    def calculate_size(text, style)
      s = Qt::Size.new
      r = style.boundingRect(text)
      s.width  = r.left + r.width + style.margin + style.margin + 3
      s.height = style.height + style.margin + style.margin + 1
      return s
    end

    def text=(text)
      @text = text
      calculate_rect
      calculate_bounding_rect
    end

    def draw(painter, is_postscript_printing)
      super(painter, is_postscript_printing)
      style.fashion(painter, is_postscript_printing)
      painter.drawText(
        rect.left + style.left, 
        rect.top + style.baseline, 
        text
      )
     end

  end

end
