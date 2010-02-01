require 'set'

module Griff
  class Layer
    attr_reader :canvas_items

    def initialize
      @canvas_items = []
    end

    def add item
      @canvas_items.push(item)
    end

    def draw(painter, is_postscript_printing)
      for item in @canvas_items do
        item.draw(painter, is_postscript_printing)
      end
    end

    def first_collision(point)
      for item in @canvas_items.reverse do
        if item.collidesWithPoint(point) then
          return item 
        end
      end
      return nil
    end

    def bounding_rect
      result = nil
      for item in @canvas_items do
        if result then
          result = result.union(item.bounding_rect)
        else
          result = item.bounding_rect
        end
      end
      return result
    end

  end

end
