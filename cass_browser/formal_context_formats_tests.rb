 
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
require 'formal_context_formats'
require 'default_set'
require 'concept_lattice'

class TC_FormalContextFormats < Test::Unit::TestCase

  def test_cxt_format_B2

    file = File.open("b2.cxt", "r")
    result = load_formal_context_from_cxt_file(file)
    file.close

    k = result["context"]
    interp = result["interp"]
    title = result["title"]

    assert_equal("Test Context B2", title)
  end

  def test_cxt_format_B3

    file = File.open("b3.cxt", "r")
    result = load_formal_context_from_cxt_file(file)
    file.close

    k = result["context"]
    interp = result["interp"]
    title = result["title"]

    lex = Lexicon.new
    i1 = make_default_set(lex)
    i2 = make_default_set(lex)
    i3 = make_default_set(lex)

    e1 = make_default_set(lex)
    e2 = make_default_set(lex)
    e3 = make_default_set(lex)

    i1.or_equals [ 1 ]
    i2.or_equals [ 2 ]
    i3.or_equals [ 3 ]

    e1.or_equals [ 2, 3 ]
    e2.or_equals [ 1, 3 ]
    e3.or_equals [ 1, 2 ]

    puts "k=#{k}"

    puts "e1 = #{e1}"
    puts "i1 = #{i1}"
    puts "i1' = #{k.extent(i1)}"

    assert_equal("Test Context B3", title)

    assert(e1.eql?(k.extent(i1)))
    assert(e2.eql?(k.extent(i2)))
    assert(e3.eql?(k.extent(i3)))

    assert(e1.eql?(k.intent(i1)))
    assert(e2.eql?(k.intent(i2)))
    assert(e3.eql?(k.intent(i3)))

    assert(i1.eql?(k.intent(e1)))
    assert(i2.eql?(k.intent(e2)))
    assert(i3.eql?(k.intent(e3)))

    assert(i1.eql?(k.extent(e1)))
    assert(i2.eql?(k.extent(e2)))
    assert(i3.eql?(k.extent(e3)))
  end

  def test_cxt_format_B3_lattice

    file = File.open("b3.cxt", "r")
    result = load_formal_context_from_cxt_file(file)
    file.close

    k = result["context"]
    interp = result["interp"]
    title = result["title"]

    lex = Lexicon.new
    l = ConceptLattice.new(k)
    
    assert_equal(8, l.count)
  end
end
