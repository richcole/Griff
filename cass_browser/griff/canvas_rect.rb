require 'griff/canvas_item'

module Griff

  class CanvasRectStyle
    attr_accessor :pen, :brush

    def initialize(pen_width=10)
      @pen = Qt::Pen.new(Qt::black, pen_width)
      @tr_pen = Qt::Pen.new(Qt::black, pen_width)
      @brush = Qt::Brush.new(Qt::white)
    end

    def fashion(painter, is_postscript_printing)
      transform = painter.worldMatrix()

      if is_postscript_printing then
        painter.setPen(@pen)
        painter.setBrush(@brush)
      else
        @tr_pen.width = @pen.width.to_f * transform.m11
        painter.setPen(@tr_pen)
        painter.setBrush(@brush)
      end
    end

    def ratio(painter, width)
      transform = painter.worldMatrix()
      return (@pen.width.to_f * transform.m11) / width.to_f
    end
  end

  class CanvasRect < CanvasItem

    attr_reader :rect, :style

    def initialize(canvas, rect=Rect.new, z=0, style=CanvasRectStyle.new)
      super(canvas, z, style)

      @style = style
      @rect = rect

      calculate_bounding_rect
    end

    def move_by(dv)
      self.rect = @rect.move_by(dv)
    end

    def calculate_bounding_rect
      @bounding_rect = @rect.expand(style.pen.width.div(2))
    end

    def rect=(new_rect)
      @rect = new_rect
      calculate_bounding_rect
    end

    def draw(painter, is_postscript_printing)
      style.fashion(painter, is_postscript_printing)
      painter.drawRect(rect.left, rect.top, rect.width, rect.height)
    end

    def mousePressEvent(event)
      style.brush.setColor(Qt::blue)
      update
    end

  end

end
