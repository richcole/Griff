 
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


require 'bitset'
require 'countable'
require 'util'

def word_size
  return 32
end

class BSet 

  attr_reader :lex
  attr_reader :bit_set

  def initialize(lex)
    @lex = lex
    @bit_set = BitSet.new(32)
    @bit_set.clear
  end

  def add x
    @bit_set.set(@lex.index(x),1)
  end

  def remove x
    @bit_set.set(@lex.index(x),0)
  end

  def each
    for index in 0..max_index do
      if @bit_set.get(index) == 1 then
        yield(@lex.word(index))
      end
    end
  end

  def indexes
    return BitSetIndexes.new(self)
  end

  def max_index
    return min_of(@bit_set.size-1,@lex.count)
  end

  def first
    for index in 0..max_index do
      if @bit_set.get(index) == 1 then
        return @lex.word(index)
      end
    end
    return nil
  end

  def hash
    result = 0
    for x in self do
      result ^= x.hash
    end
    return result
  end

  def count
    result = 0
    for x in self do
      result = result + 1
    end
    return result
  end

  alias size count

  def include? w
    return (@bit_set.get(@lex.index(w)) == 1)
  end

  def clone
    r = BSet.new(@lex)
    for x in self do r.add(x) end
    return r
  end

  def eql? s
    if ( s.is_a?(BSet) ) && (s.lex.equal?(@lex)) then
      return ((@bit_set <=> s.bit_set) == 0)
    else
      # code for an incompatible bitset
      for x in self do
        if ! s.include?(x) then
          return false
        end
      end
      for x in s do
        if ! self.include?(x) then
          return false
        end
      end
      return true
    end
  end

  def subseteq? s
    if ( s.is_a?(BSet) ) && (s.lex.equal?(@lex)) then
      return (@bit_set.subseteq(s.bit_set) == 1)
    else
      # code for an incompatible bitset
      for x in self do
        if ! s.include?(x) then
          return false
        end
      end
      return true
    end
  end

  def to_s
    s = "{"
    is_first = true
    for x in self do
      if is_first then
        is_first = false
        s << x.to_s 
      else
        s << ", #{x}"
      end
    end
    s << "}"
    return s 
  end

  def and_equals(s)
    if s.is_a?(BSet) && s.lex.equal?(@lex) then
      @bit_set = @bit_set & s.bit_set
    else
      for x in self do
        if ! s.include?(x) then 
          self.remove x
        end
      end
    end
  end

  def or_equals(s)
    if s.is_a?(BSet) && s.lex.equal?(@lex) then
      @bit_set = @bit_set | s.bit_set
    else
      for x in s do
        self.add x
      end
    end
  end

  def minus_equals(s)
    if s.is_a?(BSet) && s.lex.equal?(@lex) then
      @bit_set = @bit_set & (~ s.bit_set)
    else
      for x in s do
        self.remove x
      end
    end
  end

  def minus(s) 
    r = self.clone()
    r.minus_equals(s)
    return r
  end

  def and(s)
    r = self.clone()
    r.and_equals(s)
    return r
  end

  def or(s)
    r = self.clone()
    r.or_equals(s)
    return r
  end

  include Enumerable
end
            
               
def insert_into_from(s,t)
  for x in t do
    s.add(x)
  end
end

class BitSetIndexes
  def initialize(bitset)
    @bitset = bitset
  end

  def each
    n = @bitset.max_index
    for index in 0..n do
      if @bitset.bit_set.get(index) == 1 then
        yield(index)
      end
    end
  end

  include Countable
  include Enumerable
end
    
    
