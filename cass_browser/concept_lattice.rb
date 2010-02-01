 
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


require 'concept'
require 'default_set'
require 'power_map'
require 'extent_set'
require 'rj_util'

class ConceptLattice

  attr_reader :upper_covers, :lower_covers, :upset, :downset
  attr_reader :index_lex, :topo, :index_lex, :concept_lex

  def bottom
    return @bottom.first
  end

  def top
    return @top.first
  end

  def calculate_topo
    extent_count = Hash.new
    @topo = Array.new
    for x in self do
      extent_count[x] = concept(x).extent.count
      topo.push(x)
    end
    @topo = @topo.sort_by { |k| extent_count[k] }
    @topo.reverse!
  end

  def extent
    return ExtentArray.new(self)
  end

  def intent
    return IntentArray.new(self)
  end

  def make_empty
    @concept_lex = Lexicon.new
    @index_lex   = Lexicon.new

    @top          = make_default_set(@index_lex)
    @bottom       = make_default_set(@index_lex)
    @upset        = make_default_power_map(@index_lex)
    @downset      = make_default_power_map(@index_lex)
    @upper_covers = make_default_power_map(@index_lex)
    @lower_covers = make_default_power_map(@index_lex)
    @attribute_concept = make_default_power_map(@index_lex)
    @object_concept = make_default_power_map(@index_lex)
  end

  def initialize k=nil
    make_empty
    if k != nil then
      compute_from_context(k)
    end
  end

  def compute_from_context k
    extent_set = extent_set_of_context(k)
    for extent in extent_set do
      concept = Concept.new(extent, k.intent(extent))
      @concept_lex.index(concept)
    end
 
#     time_stmt("up_down_sets") { compute_up_down_sets }
#     time_stmt("upper_covers") { compute_upper_covers }
#     time_stmt("lower_covers") { compute_lower_covers }
#     time_stmt("top_bottom")   { compute_top_bottom }
#     time_stmt("contingents")  { compute_contingents }
#     time_stmt("calculate_topo") { calculate_topo }

    compute_up_down_sets
    compute_upper_covers
    compute_lower_covers
    compute_top_bottom
    compute_contingents
    calculate_topo
 end

  def compute_up_down_sets
    for x in self do
      extent_x = concept(x).extent
      for y in @concept_lex.indexes do
        extent_y = concept(y).extent
        if extent_x.subseteq?(extent_y) then
          @upset[x].add(y)
          @downset[y].add(x)
        end
      end
    end
  end

  def compute_upper_covers
    for x in self do
      uc = @upset[x].clone()
      uc.remove(x)
      for y in @upset[x] do
        if ! y.eql?(x) then
          for z in @upset[y] do
            if ! z.eql?(y) then
              uc.remove(z)
            end
          end
        end
      end
      @upper_covers[x] = uc
    end
  end

  def compute_lower_covers
    for x in self do
      for y in @upper_covers[x] do
        @lower_covers[y].add(x)
      end
    end
  end

  def compute_top_bottom
    for x in self do
      if @upper_covers[x].count == 0 then
        @top.add(x)
      else 
        if @lower_covers[x].count == 0 then
          @bottom.add(x)
        end
      end
    end
  end

  def compute_contingents
    for x in self do
      ni = concept(x).intent.clone
      ne = concept(x).extent.clone
      for y in @upper_covers[x] do
        ni.minus_equals(concept(y).intent)
      end
      for y in @lower_covers[x] do
        ne.minus_equals(concept(y).extent)
      end
      concept(x).attribute_contingent = ni
      concept(x).object_contingent = ne

      for m in ni
        @attribute_concept[m] = x
      end

      for g in ne
        @object_concept[g] = x
      end
    end
  end

  def concepts
    return @concept_lex.indexes
  end

  def concept x
    return @concept_lex.word(x)
  end

  def count
    return @concept_lex.count
  end

  def to_s
    s = "{\n"
    for c in self do
      s << "  Concept #{c}: #{concept(c)}\n"
      s << "    attr. cont.:   #{concept(c).attribute_contingent}\n"
      s << "    object. cont.: #{concept(c).object_contingent}\n"
      s << "    upset:         #{@upset[c]}\n"
      s << "    downset:       #{@downset[c]}\n"
      s << "    upper_cover:   #{@upper_covers[c]}\n"
      s << "    lower_cover:   #{@lower_covers[c]}\n"
    end
    s << "}"
  end    

  def each
    for x in concepts do
      yield(x)
    end
  end
  
end

class ExtentArray
  def initialize(lattice)
    @lattice = lattice
  end
  
  def [](x)
    return @lattice.concept(x).extent
  end
end

class IntentArray
  def initialize(lattice)
    @lattice = lattice
  end
  
  def [](x)
    return @lattice.concept(x).intent
  end
end
      
