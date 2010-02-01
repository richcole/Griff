require 'griff/canvas_rect'

module Griff

  class CanvasEllipseStyle < CanvasRectStyle
    def initialize(pen_width)
      super(pen_width)
    end
  end

  class CanvasEllipse < CanvasRect

    def initialize(canvas, n_r=Rect.new, n_z=0, style=CanvasRectStyle.new)
      super(canvas, n_r, n_z, style)
    end

    def draw(painter, is_postscript_printing)
      style.fashion(painter, is_postscript_printing)
      painter.drawEllipse(rect.left, rect.top, rect.width, rect.height)
    end
      
  end

end
