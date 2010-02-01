require 'griff/canvas_rect'

module Griff

  class CanvasLineStyle < CanvasRectStyle
    def initialize(pen_width)
      super(pen_width)
    end
  end

  class CanvasLine < CanvasItem

    attr_accessor :rect, :from, :to, :style

    def initialize(canvas, from, to, z, style)
      super(canvas, z, style)
      @from = from
      @to = to
      @style = style
    end

    def collidesWithPoint(point)
      return false
    end

    def collidesWithRect(rect)
      return false
    end

    def draw(painter, is_postscript_printing)
      cf = from.rect.center
      ct = to.rect.center
      style.fashion(painter, is_postscript_printing)
      painter.drawLine(cf.x, cf.y, ct.x, ct.y)
    end

  end
end      

      
      
