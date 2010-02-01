
module Griff

  class CanvasItemDispatcher
    def drag(item, dx)
      item.drag(dx)
    end

    def drag_start(item, p)
#      r = item.rect
#      np = Point.new(p.x - r.left, p.y - r.top)
      item.drag_start(p)
    end

    def drag_stop(item, p)
#      r = item.rect
#      np = Point.new(p.x - r.left, p.y - r.top)
      item.drag_stop(p)
    end

    def mouse_enter(item)
      item.mouse_enter
    end

    def mouse_leave(item)
      item.mouse_leave
    end
  end

  class CanvasControllerBroker 
    include Observable

    def ob_call(method, *args)
      if @observers then
        for ob in @observers do
          ob.call(method, *args)
        end
      end
    end      

    def drag_start(item, p)
      ob_call(:drag_start, item, p)
    end

    def drag_stop(item, p)
      ob_call(:drag_stop, item, p)
    end

    def drag(item, dx)
      ob_call(:drag, item, dx)
    end

    def mouse_enter(item)
      ob_call(:mouse_enter, item)
    end

    def mouse_leave(item)
      ob_call(:mouse_leave, item)
    end
  end

  class CanvasController

    attr_reader :canvas, :dragging_item, :mouse_over_item
    attr_accessor :broker

    def initialize(canvas)
      @canvas = canvas
      @dragging_item = nil
      @dragging = false
      @prev_pt = nil
      @broker = CanvasItemDispatcher.new
    end

    def mousePressEvent(event)
      @dragging_item = canvas.first_collision(event.pos)
      if @dragging_item then
        @broker.drag_start(@dragging_item, event.pos)
        prev_pt = event.pos
      end
      @selected_item = @dragging_item
    end

    def mouseMoveEvent(event)
      if @dragging_item && ! @dragging then
        @dragging = true
        @prev_pt = event.pos
        @broker.drag_start(@dragging_item, event.pos)
      end
      if @dragging_item && @dragging then
        p = Point.new(event.pos.x - @prev_pt.x, event.pos.y - @prev_pt.y)
        @prev_pt = event.pos
        @broker.drag(dragging_item, p)
      end
      curr_item = canvas.first_collision(event.pos)
      if (! @dragging) && (curr_item != @mouse_over_item) then
        @broker.mouse_leave(@mouse_over_item) if @mouse_over_item
        @broker.mouse_enter(curr_item) if curr_item
        @mouse_over_item = curr_item
      end
    end

    def mouseReleaseEvent(event)
      if @dragging_item && @dragging then
        @broker.drag_stop(dragging_item, event.pos)
      end
      @dragging = false
      @dragging_item = false
      @prev_pt = nil
    end
  end

end
