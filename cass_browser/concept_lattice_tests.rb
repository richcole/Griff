 
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
require 'formal_context'
require 'formal_context_formats'
require 'formal_context_examples'
require 'concept_lattice'
require 'set'

class TC_ConceptLattice < Test::Unit::TestCase

  def test_B2
    k = make_B2()
    l = ConceptLattice.new(k)
    
    assert_equal(4, l.concepts().count())

    uc_counts = Hash.new { |h,x| h[x] = 0 }
    lc_counts = Hash.new { |h,x| h[x] = 0 }
    
    for c in l.concepts() do
      uc_counts[l.upper_covers[c].count()] += 1
      lc_counts[l.lower_covers[c].count()] += 1
    end

    puts "Lattice=#{l}"

    assert_equal(1, uc_counts[0])
    assert_equal(2, uc_counts[1])
    assert_equal(1, uc_counts[2])
    
    assert_equal(1, lc_counts[0])
    assert_equal(2, lc_counts[1])
    assert_equal(1, lc_counts[2])
  end

  def test_B3
    k = make_B3()
    l = ConceptLattice.new(k)
    
    assert_equal(8, l.concepts().count())

    uc_counts = Hash.new { |h,x| h[x] = 0 }
    lc_counts = Hash.new { |h,x| h[x] = 0 }
    
    for c in l.concepts() do
      uc_counts[l.upper_covers[c].count()] += 1
      lc_counts[l.lower_covers[c].count()] += 1
    end

    puts "Lattice=#{l}"

    assert_equal(1, uc_counts[0])
    assert_equal(3, uc_counts[1])
    assert_equal(3, uc_counts[2])
    assert_equal(1, uc_counts[3])
    
    assert_equal(1, lc_counts[0])
    assert_equal(3, lc_counts[1])
    assert_equal(3, lc_counts[2])
    assert_equal(1, lc_counts[3])
  end

  def test_performance
    file = File.open("b7.cxt")
    interp_context = load_formal_context_from_cxt_file(file)
    file.close

    time_stmt "Time to Compute Lattice" { 
      lattice = ConceptLattice.new(interp_context.context)
    }
  end

    

end 
