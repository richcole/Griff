require 'Qt'
require 'griff/geom'
require 'griff/observer'
require 'griff/canvas_controller'
require 'griff/layer'
require 'profiler'

module Qt
  class Point
    def to_s
      "(" + x.to_s + "," + y.to_s + ")"
    end
  end
end

module Griff

  class Canvas

    include Observable

    attr_reader :layers
    attr_accessor :controller

    def initialize
      @layers = Hash.new { |h,k| h[k] = Layer.new }
      @controller = CanvasController.new(self)
    end

    def remove_all_items
      @layers = Hash.new { |h,k| h[k] = Layer.new }
    end

    def add(canvas_item)
      @layers[canvas_item.z].add(canvas_item)
    end

    def draw(painter, is_postscript_printing)
      time_block("Canvas Paint") {
      Profiler__::start_profile
      for x in layers.keys.sort do
        @layers[x].draw(painter, is_postscript_printing)
      end
      Profiler__::stop_profile
      }
    end

    def calculate_bounding_rect
      result = nil
      for x in @layers.keys do
        if result then
          result = result.union(@layers[x].bounding_rect)
        else
          result = @layers[x].bounding_rect
        end
      end

      if result == nil then
        result = Rect.new(0, 0, 10, 10)
      end
      @bounding_rect = result
    end

    def bounding_rect
      if @bounding_rect == nil then
        calculate_bounding_rect
      end
      return @bounding_rect.copy()
    end

    def update_z(canvas_item, old_z, new_z)
      @layers[old_z].remove(canvas_item)
      @layers[new_z].add(canvas_item)
      changed(true)
    end

    def bounding_rect=(bounding_rect)
      @bounding_rect = bounding_rect
    end

    def first_collision(pos)
      for x in @layers.keys.sort.reverse do
        item = @layers[x].first_collision(pos)
        if item then
          return item 
        end
      end
      return nil
    end

    def mousePressEvent(event)
      controller.mousePressEvent(event)
    end

    def mouseReleaseEvent(event)
      controller.mouseReleaseEvent(event)
    end

    def mouseMoveEvent(event)
      controller.mouseMoveEvent(event)
    end

  end

end
