require 'profiler'

module Griff

  class CanvasView < Qt::Widget
    attr_accessor :canvas, :double_buffering
    
    def initialize(*args)
#      args.unshift(2)
      super(*args)
      setWFlags(Qt::WNoAutoErase)
      setMouseTracking(true)
      setPaletteBackgroundColor(Qt::white)
      self.canvas = Canvas.new
      @double_buffering = true
      @transform = Qt::WMatrix.new
      @inv_transform = Qt::WMatrix.new
    end

    def canvas=(canvas)
      @canvas = canvas
      @canvas.add_observer(self)
    end

    def notify(ob)
      update
    end

    def draw(painter)
      painter.setWorldMatrix(@transform, false)
      canvas.draw(painter, false)
    end

    def before_paint(&block)
      @before_paint_block = block
    end

    def paintEvent(event)
      @before_paint_block.call if @before_paint_block
        if @double_buffering then
          pixmap = Qt::Pixmap.new(size)
          painter = Qt::Painter.new(pixmap, self)
          painter.eraseRect(0, 0, width, height)
          draw(painter)
          painter.end
          
          bitBlt(self, 0, 0, pixmap)
          pixmap.dispose
        else
          painter = Qt::Painter.new(self)
          painter.eraseRect(0, 0, width, height)
          draw(painter)
          painter.end
        end
    end

    def mousePressEvent(event)
      inv_pos = @inv_transform.map(event.pos())
      event = Qt::MouseEvent.new(
        Qt::Event::MouseButtonPress, inv_pos, event.button, event.state
      )
      canvas.mousePressEvent(event)
    end

    def mouseReleaseEvent(event)
      inv_pos = @inv_transform.map(event.pos())
      event = Qt::MouseEvent.new(
        Qt::Event::MouseButtonRelease, inv_pos, event.button, event.state
      )
      canvas.mouseReleaseEvent(event)
      canvas.bounding_rect = canvas.calculate_bounding_rect
      calculate_transform
    end

    def mouseMoveEvent(event)
      inv_pos = @inv_transform.map(event.pos)
      event = Qt::MouseEvent.new(
        Qt::Event::MouseMove, 
        inv_pos, event.button, event.state
      )
      canvas.mouseMoveEvent(event)
    end

    def resizeEvent(event)
      calculate_transform
    end

    def calculate_transform
      @transform = Qt::WMatrix.new
      ww = width.to_f
      wh = height.to_f
      c_rect = canvas.bounding_rect
      cl = c_rect.left.to_f
      ct = c_rect.top.to_f
      cw = c_rect.width.to_f
      ch = c_rect.height.to_f

      s = min_of(ww / cw, wh / ch) * 0.9
      dx = (0.5 * (ww - (cw * s))) - (cl * s);
      dy = (0.5 * (wh - (ch * s))) - (ct * s);

      @transform.translate(dx, dy)
      @transform.scale(s, s)
      @inv_transform = @transform.invert()
      return @transform
    end
  end

end
