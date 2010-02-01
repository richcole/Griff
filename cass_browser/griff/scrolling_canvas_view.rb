require 'griff/canvas_view_tests'
require 'griff/canvas'
require 'util'

module Griff

  class CanvasViewport < Qt::Widget
    attr_accessor :offset, :double_buffering

    def initialize(canvas, parent, name)
      super(parent, name)
      @canvas = canvas
      @offset = Point.new(0, 0)
      @double_buffering = true
      setWFlags(Qt::WNoAutoErase)
      setPaletteBackgroundColor(Qt::white)
      setMouseTracking(true)
    end

    def before_paint(&block)
      @before_paint_block = block
    end

    def paintEvent(event)
      time_block("PaintEvent") {
        @before_paint_block.call if @before_paint_block
        pixmap = Qt::Pixmap.new(size)
        painter = Qt::Painter.new(pixmap, self)
        painter.eraseRect(0, 0, width, height)
        painter.translate(- @offset.x, - @offset.y)
        @canvas.draw(painter, false)
        painter.end
        bitBlt(self, 0, 0, pixmap, Qt::CopyROP)
        pixmap.dispose
      }
    end

    def resizeEvent(event)
      time_block("ResizeEvent") {
        parent.update_scrollbars
      }
    end

    def translate(pos)
      return Qt::Point.new(pos.x + @offset.x, pos.y + @offset.y)
    end

    def mousePressEvent(event)
      time_block("MousePressEvent") {
        inv_pos = translate(event.pos)
        event = Qt::MouseEvent.new(
          Qt::Event::MouseButtonPress, inv_pos, event.button, event.state
        )
        @canvas.mousePressEvent(event)
        update
      }
    end

    def mouseReleaseEvent(event) 
      time_block("MouseReleaseEvent") {
        inv_pos = translate(event.pos)
        event = Qt::MouseEvent.new(
          Qt::Event::MouseButtonRelease, inv_pos, event.button, event.state
        )
        @canvas.mouseReleaseEvent(event)
        @canvas.calculate_bounding_rect
        parent.update_scrollbars
        update
      }
    end

    def mouseMoveEvent(event)
      time_block("MouseMoveEvent") {
        inv_pos = translate(event.pos)
        event = Qt::MouseEvent.new(Qt::Event::MouseMove, 
          inv_pos, event.button, event.state
        )
        @canvas.mouseMoveEvent(event)
        update
      }
    end

  end

  class ScrollingCanvasView < Qt::Grid

    slots 'hscrollValue( int )'
    slots 'vscrollValue( int )'

    attr_reader :canvas, :viewport
    
    def before_paint(&block)
      @viewport.before_paint(&block)
    end

    def calculate_transform
    end

    def double_buffering
      return viewport.double_buffering
    end

    def double_buffering=(is_buffered)
      return viewport.double_buffering = is_buffered
    end

    def initialize(parent, name)
      super(2, Qt::Horizontal, parent, name)
      setWFlags(Qt::WNoAutoErase)

      @canvas = Griff::Canvas.new
      populate
      @viewport = CanvasViewport.new(@canvas, self, "CanvasViewPort")
      @vscroll = Qt::ScrollBar.new(Qt::Vertical, self, "Vert Scrollbar")
      @hscroll = Qt::ScrollBar.new(Qt::Horizontal, self, "Horiz Scrollbar")
      @double_buffering = true
      Qt::Object.connect(
        @hscroll, SIGNAL('valueChanged( int )'), 
        self, SLOT('hscrollValue( int )')
      )
      Qt::Object.connect(
        @vscroll, SIGNAL('valueChanged( int )'), 
        self, SLOT('vscrollValue( int )')
      )
      update_scrollbars
    end

    def populate
      utils = Griff::TestUtils.new
      utils.test_populate(@canvas)
    end

    def update_scrollbar(scrollbar, canvas_start, canvas_end, widget_dim)
      canvas_width = canvas_end - canvas_start
      m_min = canvas_start - 0.1 * widget_dim
      m_max = canvas_end - widget_dim + (0.1 * widget_dim)
      m_max = max_of(m_min, m_max)
      scrollbar.setRange(m_min, m_max)
      scrollbar.setPageStep(widget_dim)
    end

    def update_scrollbars
      c_r = @canvas.bounding_rect
      w_r = @viewport.size
      update_scrollbar(@vscroll, c_r.top, c_r.bottom, w_r.height)
      update_scrollbar(@hscroll, c_r.left, c_r.right, w_r.width)
    end

    def hscrollValue(value)
      @viewport.offset.x = value
      @viewport.update
    end

    def vscrollValue(value)
      @viewport.offset.y = value
      @viewport.update
    end

  end

end
