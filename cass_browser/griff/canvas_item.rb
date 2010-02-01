require 'griff/observer'

module Griff

  class CanvasItem 

    attr_accessor :canvas, :z, :style, :bounding_rect

    def initialize(canvas, z, style)
      @canvas = canvas
      @z = z
      @style = style
      @bounding_rect = nil
      canvas.add(self)
    end

    def z=(new_z)
      old_z = @z
      @z = new_z
      canvas.update_z(self, old_z, new_z) if canvas
    end

    def collidesWithRect(other_rect)
      return false if @bounding_rect == nil
      return @bounding_rect.overlap?(other_rect)
    end

    def collidesWithPoint(point)
      return false if @bounding_rect == nil
      return @bounding_rect.include?(point)
    end

    def center
      return @bounding_rect.center if @bounding_rect
      return Point(0,0)
    end

    def drag_start p
    end

    def drag_stop p
    end

    def drag(dv)
      move_by(dv)
    end

    def mouse_enter
    end

    def mouse_leave
    end

  end

end
