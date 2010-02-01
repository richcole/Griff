 
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

require 'itql_nav_space'

class NavController 

  attr_reader :space, :view, :history, :ic, :diagram, :lattice, :context

  def fold_object(object)
    @space.fold_object(@ic.st_objects.get(object))
    primeView
  end

  def unfold_object(object)
    @space.unfold_object(@ic.st_objects.get(object))
    primeView
  end

  def fold_attribute(attribute)
    @space.fold_attribute(@ic.st_attributes.get(attribute))
    primeView
  end

  def unfold_attribute(attribute)
    @space.unfold_attribute(@ic.st_attributes.get(attribute))
    primeView
  end

  def focus_on_intent(intent)
    @space.unfold_attribute(@concept.intent)
    primeView
  end

  def focus_on_attributes(attributes)
    ms = []
    attributes.each { |m| 
      ms.push(@ic.st_attributes.get(m)) 
    }
    @space.focus_on_attributes(ms)
    primeView
  end

  def focus_on_objects(objects)
    ms = []
    objects.each { |g| 
      gs.push(@ic.st_objects.get(g)) 
    }
    @space.focus_on_objects(ms)
    primeView
  end

  def focus_on_attribute(attribute)
    @space.focus_on_attribute(@ic.st_attributes.get(attribute))
    primeView
  end

  def focus_on_object(object)
    @space.focus_on_object(@ic.st_objects.get(object))
    primeView
  end

  def primeView
    puts "History Push."
    if (@diagram != nil) && (@ic != nil) then
      puts "History Push."
      @history.push [@ic, @diagram]
    end
    @ic = @space.context
    @lattice = Lattice.new(@ic.context)
    @diagram = Diagram.layout(@lattice)
    @view.set_diagram(@diagram, @ic.st_objects, @ic.st_attributes)
  end

  def save_cxt(filename)
    interp_context_to_cxt(@ic, filename)
  end

  def move_back
    puts "History size = #{@history.size}"
    if @history.size > 0 then
      idc = @history.pop
      @ic = idc[0]
      @diagram = idc[1]
      @view.set_diagram(@diagram, @ic.st_objects, @ic.st_attributes)
    end
  end

  def initialize(view)
    puts "Init!"
    @view = view
    @space = ItqlNavSpace.new
    @history = []
  end

  def set_relation(relation)
    @space.set_relation(relation)
  end

end

