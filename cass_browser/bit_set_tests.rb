 
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
require 'bit_set'
require 'lexicon'
require 'default_set'

class TC_BSet < Test::Unit::TestCase

  def test_simple
    lex = Lexicon.new
    s1  = BSet.new(lex)
    s2  = BSet.new(lex)
    s3  = BSet.new(lex)

    s1.add 1
    s1.add "abc"

    assert_equal(2, lex.count)

    i = 0
    for x in s1 do
      i = i + 1
    end
    assert_equal(i, 2)

    for x in s1 do
      s2.add(x)
    end

    assert_equal(1, lex.index(1))
    assert_equal(2, lex.index("abc"))

    assert_equal(2, lex.count)
    
    assert_equal(true, s1.include?(1))
    assert_equal(true, s1.include?("abc"))

    assert_equal(s1.hash, s2.hash)
    assert_equal(true, s1.eql?(s2))
    assert_equal(true, s2.eql?(s1))

    assert_not_equal(s1.hash, s3.hash)
    assert_equal(false, s1.eql?(s3))
    assert_equal(false, s2.eql?(s3))
    assert_equal(false, s3.eql?(s1))
    assert_equal(false, s3.eql?(s2))

  end

  def test_subseteq
    lex = Lexicon.new
    s1 = make_default_set(lex, ["g1", "g2", "g3"])
    s2 = make_default_set(lex, ["g2", "g3"])
    s3 = make_default_set(lex, ["g1"])
    s4 = make_default_set(lex, [])

    assert_equal(true, s1.subseteq?(s1))
    assert_equal(false, s1.subseteq?(s2))
    assert_equal(false, s1.subseteq?(s3))
    assert_equal(false, s1.subseteq?(s4))

    assert_equal(true, s2.subseteq?(s2))
    assert_equal(true, s2.subseteq?(s1))
    assert_equal(false, s2.subseteq?(s3))
    assert_equal(false, s2.subseteq?(s4))

    assert_equal(true, s3.subseteq?(s3))
    assert_equal(true, s3.subseteq?(s1))
    assert_equal(false, s3.subseteq?(s2))
    assert_equal(false, s3.subseteq?(s4))

    assert_equal(true, s4.subseteq?(s4))
    assert_equal(true, s4.subseteq?(s1))
    assert_equal(true, s4.subseteq?(s2))
    assert_equal(true, s4.subseteq?(s3))
  end

  def test_indirect
    lex = Lexicon.new
    s1  = make_default_set(lex)
    s2  = make_default_set(lex)
    s3  = make_default_set(lex)

    s1.add("g1")
    s1.add("g2")

    insert_into_from(s2, s1)
    s3.add(s1);
    s3.add(s2);

    assert_equal(1, s3.count)
  end

  def test_various_lex
    lex1 = Lexicon.new
    lex2 = Lexicon.new
    lex3 = Lexicon.new

    s1  = make_default_set(lex1)
    s2  = make_default_set(lex2)
    s3  = make_default_set(lex3)

    s1.add("g1")
    s1.add("g2")

    insert_into_from(s2, s1)
    insert_into_from(s3, s1)

    s3.add("g3")

    assert_equal(true, s1.eql?(s2))
    assert_equal(true, s1.eql?(s2))

    assert_equal(false, s1.eql?(s3))
    assert_equal(false, s3.eql?(s1))
  end

  def test_bit_set_set
    lex = Lexicon.new

    s1 = make_default_set(lex)
    s2 = make_default_set(lex)
    s3 = make_default_set(lex)
    s2.add(s1)    # s1 = {}, s2 = { {} }
    s1.add(s2)    # s1 = { { {} } }
    s3.add(s1)    # s3 = { { { {} } } }
    s3.add(s2)    # s3 = { { { {} } }, { {} } }

    assert_equal(1, s1.count)
    assert_equal(1, s2.count)
    for x in s1 do
      assert_equal(true, s2.eql?(x))
      assert_equal(true, x.eql?(s2))
    end

    for x in s2 do
      assert_equal(false, s1.eql?(x))
      assert_equal(false, x.eql?(s1))
    end
  end
    
end
