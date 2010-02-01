 
# Copyright 2004, Richard Cole
#
# This file is part of Griff.
# 
# Griff is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 2 of the License, or (at your
# option) any later version.
# 
# Griff is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
# 
# You should have received a copy of the GNU General Public License
# along with Griff; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


class LatticeCanvas < Canvas

  attr_reader :nav_controller, :context, :lattice, :rep, :pos

  def initialize(*args)
    super(500, 500)
    setBackgroundColor(white)
    @nav_controller = NavController.new(self)
    internal_clear
  end
	
  def internal_clear
    clear_canvas_of_items
    
    @context = nil
    @lattice = nil
    @rep = nil
    @pos = nil

    @concept_view = nil

    @selected_item = nil
    @mouse_over_item = nil

    @tr = Transform.new(100, 200, 100)
  end

  def load_context k, object_interp=IdentInterp.new, attribute_interp=IdentInterp.new
    internal_clear
	
    @context   = k
    @object_interp = object_interp
    @attribute_interp = attribute_interp
    @lattice   = ConceptLattice.new(@context)
    @rep = MeetIrrRep.new(@lattice)
    @pos = AdditiveDiagram.new(@rep, @lattice)
    
    calculate_items
    calculate_lines
    calculate_attribute_labels
    calculate_object_labels
    reset_scale
    update
  end

  def load_cxt_file filename
    internal_clear
	
    file   = File.open(filename)
    result = load_formal_context_from_cxt_file(file)
    @context = result.context
    @title = result.title
    @object_interp = result.object_interp
    @attribute_interp = result.attribute_interp
    load_context(@context, @object_interp, @attribute_interp)
  end

  def write_cxt_file filename
    File.open(filename, "w") { |file| 
      write_formal_context_to_cxt_file(@context, @object_interp, @attribute_interp, file)
    }
  end

  def clear_canvas_of_items
    allItems.each { |x| x.dispose }
    allItems.clear()
  end

  def calculate_items
    # create an item for each concept
    @concept_view = {}
    for c in @lattice do
      p = @pos[c]
      circle = ConceptView.new(@tr, p[0], p[1], self, c, self)
      @concept_view[c] = circle
    end
  end

  def calculate_lines
    # create a line view for each concept
    for c in @lattice do
      for d in @lattice.upper_covers[c] do
        line = LineView.new(@tr, self, @concept_view[d], @concept_view[c])
      end
    end
  end

  def calculate_attribute_labels
    for c in @lattice do
      ms = @lattice.concept(c).attribute_contingent
      if ms.count > 0 then
        text = LabelView.new(@tr, self, @concept_view[c], ms, @attribute_interp, 0.3, -0.3, true)
      end
    end
  end

  def calculate_object_labels
    for c in @lattice do
      gs = @lattice.concept(c).object_contingent
      if gs.count > 0 then
        text = LabelView.new(@tr, self, @concept_view[c], gs, @object_interp, 0.3, 0.3, false)
      end
    end
  end

  def on_drag
    # reset_scale
    update
  end

  def on_drop
    reset_scale
    update
  end
	
  def reset_scale
    reset_scale_to(width, height)
  end

  def reset_scale_to width, height
    if @lattice then
      tleft = 0
      ttop = 0
      tbottom = 0
      tright = 0
      for x in @lattice do
        p = @pos[x]
        tleft = min_of(tleft, p[0])
        tright = max_of(tright, p[0])
        ttop = min_of(ttop, p[1])
        tbottom = max_of(tbottom, p[1])
      end
      for x in allItems do
        r = x.boundingRect
        tl = @tr.itr(r.left, r.top)
        br = @tr.itr(r.right, r.bottom)
        tleft = min_of(tleft, tl[0])
        tright = max_of(tright, br[0])
        ttop = min_of(ttop, tl[1])
        tbottom = max_of(tbottom, br[1])
      end
      twidth = tright - tleft
      theight = tbottom - ttop
      s = min_of(width / twidth, height / theight) * (4 / 5)
      ox = ((width - (twidth * s))/2) - (tleft * s)
      oy = ((height - (theight * s))/2) - (ttop * s)
      @tr.set_tr(s.to_f, ox.to_f, oy.to_f)
    end
  end

  def conceptMoveBy c, x, y
    @pos.move_by(c, x / @tr.s, y / @tr.s)
    for ci in @lattice do
      p = @pos[ci]
      @concept_view[ci].set_pos(p[0], p[1])
    end
  end

  def get_hit_item p
    for x in self.collisions(p) do
      if x.hitable then
        return x
      end
    end
    return nil
  end

  def mousePressEvent event
    @selected_item = get_hit_item(event.pos)
    if @selected_item then
      @selected_item.mousePressEvent(event)
    end
  end

  def mouseReleaseEvent event
    if @selected_item then
      @selected_item.mouseReleaseEvent(event)
      @selected_item = nil
      on_drop
    end
  end

  def mouseMoveEvent event
    if @selected_item then
      @selected_item.mouseMoveEvent(event)
      on_drag
    else
      hi = get_hit_item(event.pos)
      if @mouse_over_item then
        if hi != @mouse_over_item then
          @mouse_over_item.mouseLeave(event)
          @mouse_over_item = nil
        else
          @mouse_over_item.mouseOver(event)
        end
      else
        @mouse_over_item = hi
        if @mouse_over_item then
          @mouse_over_item.mouseEnter(event)
        end
      end
    end
  end

end

