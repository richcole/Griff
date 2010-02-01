require 'griff/canvas_multitext'


module Griff

  class CanvasConceptLabel < CanvasMultiText
    
    def initialize(
      canvas, canvas_concept, text=[], top_left=Point.new(0, 0), z=0, 
      style=CanvasTextStyle.new()
    )
      @offset = top_left
      top_left += canvas_concept.center
      super(canvas, text, top_left, z, style)
      @canvas_concept = canvas_concept
    end

    def move_by(dx)
      @offset = Point.new(@offset.x + dx.x, @offset.y + dx.y)
      super(dx)
    end

    def draw(painter, is_postscript_printing)
      super(painter, is_postscript_printing)
    end

    def update_position
      left = @offset.x + @canvas_concept.center.x
      top = @offset.y + @canvas_concept.center.y
      move_to(Point.new(left, top))
    end

  end

end      
    
    
