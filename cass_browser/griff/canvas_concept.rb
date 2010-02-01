require 'griff/canvas_ellipse'

module Griff

  class CanvasConceptStyle < CanvasEllipseStyle
    attr_accessor :radius

    def initialize(pen_width=3, radius=20)
      super(pen_width)
      @radius = radius
    end
  end

  class CanvasConcept < CanvasEllipse
    attr_accessor :concept

    def initialize(canvas, concept, p, z, style)
      rect = Rect.new(p[0], p[1], 0, 0).expand(style.radius)
      super(canvas, rect, z, style)
      @concept = concept
    end

    def center_on(p)
      @rect = rect.center_on(Point.new(p[0], p[1]))
      calculate_bounding_rect
    end

  end
end
