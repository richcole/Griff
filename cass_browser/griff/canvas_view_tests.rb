require 'griff/canvas_rect'
require 'griff/canvas_text'
require 'griff/canvas_ellipse'
require 'griff/canvas_line'
require 'griff/canvas_multitext'
require 'griff/geom'

module Griff

  class TestUtils
    def test_populate(canvas)
      rect_style = CanvasRectStyle.new(1)
      text_style = CanvasTextStyle.new(1, "Comic", 10)

      x1 = CanvasRect.new(
        canvas, Rect.new(10, 10, 10, 10), 1, rect_style.clone
      )
      x2 = CanvasRect.new(
        canvas, Rect.new(20, 30, 10, 10), 1, rect_style.clone
      )
      x3 = CanvasRect.new(
        canvas, Rect.new(30, 20, 10, 10), 1, rect_style.clone
      )
      x4 = CanvasText.new(
        canvas, "Hello Google", Point.new(20, 10), 1, 
        text_style.clone
      )
      x5 = CanvasEllipse.new(
        canvas, Rect.new(30, 80, 10, 10), 2, text_style.clone
      )
      x6 = CanvasLine.new(
        canvas, x1, x2, 3, rect_style.clone
      )
      x7 = CanvasMultiText.new(
        canvas, ["Hello", "World", "Google", "This", "is", "the", "world"], 
        Point.new(80, 30), 2, text_style.clone
      )
      x7 = CanvasMultiText.new(
        canvas, ["Small"], 
        Point.new(80, 30), 2, text_style.clone
      )
      x7 = CanvasMultiText.new(
        canvas, ["Small"], 
        Point.new(80, 30), 2, text_style.clone
      )
    end
  end

end
  
