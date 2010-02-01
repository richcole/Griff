 
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


require 'test/unit'
require 'lexicon'
require 'value_object'

class DoTimes
  def initialize 
    @first = true
  end

  def first
    if @first then
      @first = false
      yield
    end
  end

  def not_first
    if @first then
      @first = false
    else
      yield
    end
  end
end      
    
class TestObj
  include ValueObject

  def initialize(str="")
    @str = str
  end

  def set_str(str)
    @str = str
  end

  def to_s
    s = "["
    times = DoTimes.new
    for x in self.instance_variables do
      times.not_first { s << ", " }
      s << "#{x.class}: #{x}: #{eval(x)}"
    end
    s << "]"
    return s
  end
  
end

class TC_Lexicon < Test::Unit::TestCase

  def test_simple
    lex = Lexicon.new
    i1 = lex.index(1)
    i2 = lex.index("abc")
    assert_equal(i1, lex.index(1))
    assert_equal(i2, lex.index("abc"))
    assert_equal("abc", lex.word(i2))
    assert_equal(1, lex.word(i1))
    assert_equal(2, lex.count)
  end

  def test_rewrite
    lex = Lexicon.new
    a = "abc"
    i1 = lex.index(a);
    a.upcase!;
    i2 = lex.index(a);
    assert_equal(1, lex.index("abc"))
    assert_equal(2, lex.index("ABC"))
  end

  def test_rewrite_obj
    lex = Lexicon.new
    g1 = TestObj.new("abc")
    i1 = lex.index(g1)
    g1.set_str("ABC")
    i2 = lex.index(g1)
    g2 = g1
    g3 = TestObj.new("abc")
    puts "g1: #{g1}"
    puts "g2: #{g2}"
    puts "g3: #{g3}"
    assert_equal(1, lex.index(g3))
    assert_equal(2, lex.index(g1))
    assert_equal(2, lex.index(g2))
  end

  def test_obj
    lex = Lexicon.new
    g1 = TestObj.new("abc")
    i1 = lex.index(g1.to_s)
    g2 = TestObj.new("ABC")
    i2 = lex.index(g2.to_s)
    g3 = TestObj.new("abc")
    i3 = lex.index(g3.to_s)
    assert_equal(g3.hash, g1.hash)
    assert_equal(true, g3.eql?(g1))
    assert_equal(true, g1.eql?(g3))
    assert_equal(1, i1)
    assert_equal(2, i2)
    assert_equal(1, i3)
  end

end
