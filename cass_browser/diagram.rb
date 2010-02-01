 
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


require 'mathn'
require 'linalg'
require 'set'
require 'util'

include Linalg

class AttrRep
  attr_reader :rep_matrix

  def initialize lattice, attributes
    @lattice = lattice
    @attributes = attributes
    calculate_rep
  end

  def calculate_rep

    @rep_matrix = DMatrix.new(@lattice.count, @attributes.count, 0)
    for x in @lattice do
      for m in @lattice.concept(x).intent do
        @rep_matrix[x-1,@attributes.lex.index(m)-1] = 1
      end
    end
  end

  def rep(concept)
    return @rep_matrix.row(concept-1)
  end

  def each
    for y in @l.concept(@lattice.top).intent do
      yeild(y)
    end
  end

  alias [] rep
end

class MeetIrrOrder

  attr_reader :upset, :upper_covers

  def initialize meet_irr, lattice
    @upset = make_default_power_map(lattice.index_lex)
    @upper_covers = make_default_power_map(lattice.index_lex)

    for c in meet_irr do
      @upset[c] = lattice.upset[c].and(meet_irr)
    end

    for c in meet_irr do
      extra = make_default_set(lattice.index_lex)
      extra.add(c)
      for d in @upset[c] do
        if c != d then
          for e in @upset[d]
            if e != d then
              extra.add(e)
            end
          end
        end
      end
      @upper_covers[c] = @upset[c].minus(extra)
    end

  end

  def each
    for c in @upper_covers.keys do
      yield(c)
    end
  end

end      

class MeetIrrRep

  attr_reader :rep_matrix, :attr_order, :irreducibles

  def initialize l
    @lattice = l
    calculate_irreducibles
    calculate_chains
    calculate_contingent_rep
    @chain_rep_matrix = @rep_matrix * @chains_matrix
    @chain_cont_rep_matrix = @cont_matrix * @chains_matrix
  end

  def calculate_irreducibles
    @irreducibles = make_default_set(Lexicon.new)
    for x in @lattice do
      if @lattice.upper_covers[x].count == 1 then
        @irreducibles.add(x)
      end
    end
    @attr_order = MeetIrrOrder.new(@irreducibles, @lattice)
    
    @rep_matrix = DMatrix.new(@lattice.count, max_of(1, @irreducibles.count), 0)
    for x in @lattice do
      for y in @lattice.upset[x] do
        if @irreducibles.include?(y) then
          @rep_matrix[x-1,@irreducibles.lex.index(y)-1] = 1
        end
      end
      if @irreducibles.include?(x) then
        @rep_matrix[x-1,@irreducibles.lex.index(x)-1] = 1
      end
    end
  end

  def calculate_chains
    @num_chains = 0
    @chains = Hash.new
    @taken_irr = Set.new

    for irr in @irreducibles do
      look_for_chain(irr)
    end

    @chains_matrix = DMatrix.new(max_of(1,@irreducibles.count), max_of(1,@num_chains), 0)
    for irr, chain in @chains do
      @chains_matrix[@irreducibles.lex.index(irr)-1,chain-1] = 1
    end
  end

  def look_for_chain(irr)
    return @chains[irr] if @chains.keys.include?(irr)

    candidates = []
    for irr_parent in @attr_order.upper_covers[irr] do
      if @lattice.upper_covers[irr].include?(irr_parent) && ! @taken_irr.include?(irr_parent) then
        look_for_chain(irr_parent)
        candidates.push(irr_parent)
      end
    end

    if candidates.length > 0 then
      irr_parent = candidates.sort[0]
      @chains[irr] = @chains[irr_parent]
      @taken_irr.add(irr_parent)
    else
      @num_chains += 1
      result = @num_chains
      @chains[irr] = result
    end

    return result
  end

  def calculate_contingent_rep
    @cont_matrix = DMatrix.new(@lattice.count, max_of(1, @irreducibles.count), 0)
    for concept in @lattice do
      concept_rep = @rep_matrix.row(concept-1)
      for parent_concept in @lattice.upper_covers[concept] do
        parent_rep = @rep_matrix.row(parent_concept-1)
        diff_rep = concept_rep - parent_rep
        for i in 1..diff_rep.num_columns do
          if diff_rep[0,i-1] > 0 then
            @cont_matrix[concept-1,i-1] = 1
          end
        end
      end
    end
  end
      
  def rep(concept)
    return @chain_rep_matrix.row(concept-1)
  end

  def cont_rep(concept)
    return @chain_cont_rep_matrix.row(concept-1)
  end

  def count
    return @chain_rep_matrix.num_columns
  end

  alias [] rep
end

class AdditiveDiagram

  attr_reader :vect_matrix
  attr :unit

  def initialize rep, lattice
    @unit = 50
    @rep = rep
    @lattice = lattice
    alloc_vectors
  end

  def alloc_vectors

    num_rows = @rep.count
    @vect_matrix = DMatrix.new(num_rows, 2)
    @i    = -1

    for i in 1..num_rows do
      v = next_vector
      @vect_matrix[i-1,0] = v[0]
      @vect_matrix[i-1,1] = v[1]
    end
  end

  def next_vector
    @i += 1
    if @i == 0 then
      r = [0, @unit]
    else
      if @i % 2 == 1 then sign = 1 else sign = -1 end
      j = ((@i+1).div 2)
      r = [@unit * sign * j * Math.sqrt(j), @unit * j + (Math.sqrt(j) .div 4)]
    end
    return r
  end
    
  def pos(concept)
    rep = @rep[concept] 
    result = rep * @vect_matrix
    return [result[0,0], result[0,1]]
  end

  def move_by(concept, dx, dy)
    dx = dx.to_f
    dy = dy.to_f
    cont_rep = @rep.cont_rep(concept)
    total = 0.0
    for i in 1..cont_rep.num_columns do
      total += cont_rep[0,i-1]
    end
    if total == 0.0 then
      total = 1.0
    end

    for i in 1..cont_rep.num_columns do
      @vect_matrix[i-1, 0] += dx * (cont_rep[0, i-1] / total)
      @vect_matrix[i-1, 1] += dy * (cont_rep[0, i-1] / total)
    end
  end

  alias [] pos

end

