 
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


require 'formal_context_formats'
require 'pig'
require 'util'

class NavSpace

  attr_reader :pig

  def initialize 
    @pig = Pig.new
    @h = "h1"
    @n = "n1"
    @graph = "_g calls-hp _m."

    init_caches
    prime
  end

  def context
    result = @pig.get_context("_g is-member #{@h}.", "_m is-member #{@n}.",
        "_g is-member #{@h}, _m is-member #{@n}, #{@graph}")
    result["object_interp"] = 
      NavSpaceArrayInterp.new(
        result["object_interp"], 
        lambda {|x| return self.is_object_unfolded(x)} 
    )
    result["attribute_interp"] = 
      NavSpaceArrayInterp.new(
        result["attribute_interp"], 
        lambda {|x| return self.is_attribute_unfolded(x)} 
    )
    return result
  end

  def retract_attributes
    graph = "_a is-member #{@n}."
    pig.retract(graph, graph)
  end

  def assert_attributes ms
    action = ""
    for m in ms do
      action << "assert #{quote("#{quote(m)} is-member #{@n}")}"
    end
    pig.execute_action(action)
  end

  def assert_objects ms
    action = ""
    for m in ms do
      action << "assert #{quote("#{quote(m)} is-member #{@h}")}"
    end
    pig.execute_action(action)
  end

  def retract_objects
    graph = "_a is-member #{@h}."
    pig.retract(graph, graph)
  end

  def retract_object_unfolds
    graph = "#{@h} has-unfolded _a."
    pig.retract(graph, graph)
  end

  def retract_attribute_unfolds
    graph = "#{@n} has-unfolded _a."
    pig.retract(graph, graph)
  end

  def prime
    retract_object_unfolds
    retract_attribute_unfolds
    retract_objects
    retract_attributes
    unfold_attribute("net.sourceforge.toscanaj")
    unfold_object("net.sourceforge.toscanaj")
  end

  def quote s
    return "\"" + s.gsub(/\\/, "\\\\").gsub(/"/,"\\\"") + "\""
  end

  def fold_attribute m
    action = <<-EOF
    for #{quote("_a in #{quote(m)}")} do
      retract "_a is-member #{@n}"
    done
    retract "#{quote(@n)} has-unfolded #{quote(m)}"
    EOF

    pig.execute_action(action)
    @unfolded_attributes_cache[m] = false
  end

  def fold_object g
    action = <<-EOF
      for #{quote("_a in #{quote(g)}")} do
        retract "_a is-member #{@h}"
      done
      retract "#{quote(@h)} has-unfolded #{quote(g)}"
    EOF

    pig.execute_action(action)
    @unfolded_objects_cache[m] = false
  end

  def unfold_attribute m
    action = <<-EOF
      for #{quote("_a in #{quote(m)}")} do
        assert #{quote("_a is-member #{@n}")}
      done
      assert #{quote("#{quote(@n)} has-unfolded #{quote(m)}")}
    EOF

    pig.execute_action(action)
    @unfolded_attributes_cache[m] = true
  end

  def unfold_object g
    action = <<-EOF
      for #{quote("_a in #{quote(g)}")} do
        assert #{quote("_a is-member #{@h}")}
      done
      assert #{quote("#{quote(@h)} has-unfolded #{quote(g)}")}
    EOF

    pig.execute_action(action)
    @unfolded_objects_cache[g] = true
  end

  def focus_on_attribute m
    retract_attributes
    ms = [m]
    assert_attributes(ms)
  end

  def focus_on_object g
    retract_objects
    gs = [g]
    assert_objects(gs)
  end

  def init_caches
    @unfolded_objects_cache = Hash.new { 
      |h,k| 
      h[k] = self.calc_is_object_unfolded(k) 
    }
    @unfolded_attributes_cache = Hash.new { 
      |h,k| 
      h[k] = self.calc_is_attribute_unfolded(k) 
    }
  end

  def is_object_unfolded i
    return @unfolded_objects_cache[i]
  end

  def is_attribute_unfolded i
    return @unfolded_attributes_cache[i]
  end

  def calc_is_object_unfolded i
    graph = "#{quote(@h)} has-unfolded #{quote(i)}."
    result = (count_in(pig.query(graph)) == 2)
    return result
  end
	
  def calc_is_attribute_unfolded i
    graph = "#{quote(@n)} has-unfolded #{quote(i)}."
    result = (count_in(pig.query(graph)) == 2)
    return result
  end

end

    
