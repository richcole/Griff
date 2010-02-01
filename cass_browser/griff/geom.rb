require 'griff/misc'

module Griff 

  class Rect

    attr_accessor :left, :top, :width, :height
    
    def initialize (left, top, width, height)
      @left = left
      @top = top
      @width = width
      @height = height
    end

    def to_s
      return "[Rect: left=#{@left}, top=#{@top}, width=#{@width}, height=#{@height}]"
    end

    def bottom
      return @top + @height
    end

    def right
      return @left + @width
    end

    def bottom=(new_bottom)
      if new_bottom < @top then
        @top=new_bottom
      end
      @height = new_bottom - @top
    end

    def right=(new_right)
      if new_right < @left then
        @left = new_right
      end
      @width = new_right - @left
    end

    def move_to(p)
      return Rect.new(p.x, p.y, @width, @height)
    end

    def resize_by(dx)
      return Rect.new(@left, @top, @width + dx.x, @height + dx.y)
    end

    def copy()
      return Rect.new(@left, @top, @width, @height)
    end

    def shave(margin)
      return copy.shave!(margin)
    end

    def expand(margin)
      return copy.expand!(margin)
    end

    def move_by(dv)
      return copy.move_by!(dv)
    end

    def center
      return Point.new(@left + (@width.div(2)), @top + (@height.div(2)))
    end

    def center_on(p)
      return copy.center_on!(p)
    end

    def union(r)
      return copy.union!(r)
    end

    def include?(p)
      if right < p.x then
        return false
      end
      if left > p.x then
        return false
      end
      if bottom < p.y then
        return false
      end
      if top > p.y then
        return false
      end
      return true
    end

    def overlap?(r)
      if right < r.left then
        return false
      end
      if left > r.right then
        return false
      end
      if bottom < r.top then
        return false
      end
      if top > r.bottom then
        return false
      end
      return true
    end

    def includePoint(p)
      new_left = min_of(p.x, @left)
      new_top = min_of(p.y, @top)
      new_right = max_of(right, p.x)
      new_bottom = max_of(bottom, p.y)
      return Rect.new(new_left, new_top, new_right - new_left, new_bottom - new_top)
    end

    def min_max(min_width, min_height, max_width, max_height)
      new_width = max_of(min_width, width)
      new_width = min_of(max_width, new_width)
      new_height = max_of(min_height, height)
      new_height = min_of(max_height, new_height)

      return Rect.new(left, top, new_width, new_height)
    end

  protected

    def expand!(margin)
      doublemargin = margin*2
      @left -= margin
      @width += doublemargin
      @top -= margin
      @height += doublemargin
      return self
    end

    def shave!(margin)
      doublemargin = margin*2
      if doublemargin < width && doublemargin < height then
        @width -= doublemargin
        @height -= doublemargin
        @left += margin
        @top += margin
      else
        if doublemargin > width then
          if doublemargin > height then
            @left = @left + @width.div(2)
            @top = @top + @height.div(2)
            @width = 0
            @height = 0
          else
            @left = @left + @width.div(2)
            @width = 0
            @top += margin
            @height -= doublemargin
          end
        else
          @top = @top + @height.div(2)
          @height = 0
          @left += margin
          @width -= doublemargin
        end
      end
      return self
    end

    def move_by!(dv)
      @left += dv.x
      @top += dv.y
      return self
    end

    def union!(other_rect)
      return self if other_rect == nil  
      l = min_of(left, other_rect.left)
      r = max_of(right, other_rect.right)
      t = min_of(top, other_rect.top)
      b = max_of(bottom, other_rect.bottom)
      @left = l
      @top = t
      @width = r - l
      @height = b - t
      return self
    end

    def intersection!(other_rect)
      return self if other_rect == nil  
      l = max_of(left, other_rect.left)
      r = min_of(right, other_rect.right)
      t = max_of(top, other_rect.top)
      b = min_of(bottom, other_rect.bottom)
      @left = l
      @top = t
      @width = r - l
      @height = b - t
      return self
    end

    def center_on!(p)
      @left = p.x - width.div(2);
      @top  = p.y - height.div(2);
      return self
    end

  end

  class Point
    attr_accessor :x, :y

    def initialize(x, y)
      @x = x
      @y = y
    end

    def +(p)
      return Point.new(@x + p.x, @y + p.y)
    end

    def -(p)
      return Point.new(@x - p.x, @y - p.y)
    end

    def hash
      return @x.hash ^ @y.hash
    end

    def eql?(p)
      return @x.eql?(p.x) && @y.eql?(p.y)
    end

  end

end
