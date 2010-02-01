 
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
require 'formal_context_examples'
require 'extent_set'
require 'formal_context_formats'

class TC_Extent_set < Test::Unit::TestCase

  def test_extent_set_B2
    extent_set = extent_set_of_context(make_B2)
    assert_equal(4, extent_set.count())

    lex = Lexicon.new
    e1 = make_default_set lex
    e2 = make_default_set lex
    e3 = make_default_set lex
    e4 = make_default_set lex

    e1.or_equals [ "g1" ]
    e2.or_equals [ "g2" ]
    e3.or_equals [ "g1", "g2" ]

    assert(extent_set.include?(e1))
    assert(extent_set.include?(e2))
    assert(extent_set.include?(e3))
    assert(extent_set.include?(e4))

    assert_equal(4, extent_set.count)
  end

  def test_extent_set_B3
    extent_set = extent_set_of_context(make_B3)
    assert_equal(8, extent_set.count())

    lex = Lexicon.new
    e1 = make_default_set lex
    e2 = make_default_set lex
    e3 = make_default_set lex
    e4 = make_default_set lex
    e5 = make_default_set lex
    e6 = make_default_set lex
    e7 = make_default_set lex
    e8 = make_default_set lex

    e1.or_equals [ "g1" ]
    e2.or_equals [ "g2" ]
    e3.or_equals [ "g3" ]
    e4.or_equals [ "g1", "g2" ]
    e5.or_equals [ "g2", "g3" ]
    e6.or_equals [ "g1", "g3" ]
    e7.or_equals [ "g2", "g3", "g1" ]
    e8.or_equals [ ]

    assert(extent_set.include?(e1))
    assert(extent_set.include?(e2))
    assert(extent_set.include?(e3))
    assert(extent_set.include?(e4))
    assert(extent_set.include?(e5))
    assert(extent_set.include?(e6))
    assert(extent_set.include?(e7))
    assert(extent_set.include?(e8))

    assert_equal(8, extent_set.count)
  end

  def test_extent_set_load_B3

    file = File.open("b3.cxt", "r")
    result = load_formal_context_from_cxt_file(file)
    file.close

    k = result["context"]
    interp = result["interp"]

    extent_set = extent_set_of_context(k)

    lex = Lexicon.new
    e1 = make_default_set lex
    e2 = make_default_set lex
    e3 = make_default_set lex
    e4 = make_default_set lex
    e5 = make_default_set lex
    e6 = make_default_set lex
    e7 = make_default_set lex
    e8 = make_default_set lex

    e1.or_equals [ 1 ]
    e2.or_equals [ 2 ]
    e3.or_equals [ 3 ]
    e4.or_equals [ 1, 2 ]
    e5.or_equals [ 2, 3 ]
    e6.or_equals [ 1, 3 ]
    e7.or_equals [ 2, 3, 1 ]

    assert(extent_set.include?(e1))
    assert(extent_set.include?(e2))
    assert(extent_set.include?(e3))
    assert(extent_set.include?(e4))
    assert(extent_set.include?(e5))
    assert(extent_set.include?(e6))
    assert(extent_set.include?(e7))
    assert(extent_set.include?(e8))

    assert_equal(8, extent_set.count)

    puts "List of extents"
    for e in extent_set do
      s = ""
      for x in e do
        s << interp.object_name(x) + " ";
      end
      puts s
    end
  end

end
