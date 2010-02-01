 
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

require 'countable'

def make_copy(w)
  return w.clone
rescue TypeError => err
  return w
end

 
class Lexicon

  @@constructor_count = 0

  def incr_constructor_count
    @@constructor_count += 1
  end

  def indexes
    return LexiconIndexes.new(self)
  end

  def words
    return LexiconWords.new(self)
  end

  def initialize
    self.incr_constructor_count
    @count = 0
    @word_hash = Hash.new { |h,k| self.alloc_new(k) } 
    @index_hash = Hash.new
  end

  def alloc_new w
    @count = @count + 1
    nw = make_copy(w)
    @word_hash[nw] = @count
    @index_hash[@count] = nw
    return @count
  end

  def count 
    return @count
  end

  def index w
    return @word_hash[w]
  end
  
  def word i
    if i > @count then
      raise "The index i=#{i} > #{@count} is invalid"
    else
      return @index_hash[i]
    end
  end

  def to_s
    s = "{\n"
    for x in @index_hash.keys do
      t = @index_hash[x].to_s
      s << ( "  " + x.to_s + " -> " + t + "\n" )
    end
    s << "}"
  end

  def set_word(word, index)
    if index < 1 then
      raise "The index i=#{i} < 1 is invalid"
    end
    @word_hash[word] = index
    @index_hash[index] = word
    if @count < index then
      @count = index
    end
  end

end

class LexiconIndexes
  def initialize lex
    @lex = lex
  end
  def each
    for x in 1..@lex.count do
      yield(x)
    end
  end
  include Countable
  include Enumerable
  def count
    return @lex.count
  end
end

class LexiconWords
  def initialize lex
    @lex = lex
  end
  def each
    for x in 1..@lex.count do
      yield(@lex.word(x))
    end
  end
  include Countable
  include Enumerable
  def count
    return @lex.count
  end
end

module Enumerable
  def join(*args)
    self.to_a.join(*args)
  end
end

