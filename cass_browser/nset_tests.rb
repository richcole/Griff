 
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
require 'nset'

class TC_nset < Test::Unit::TestCase

  def test_rehash
    a = Hash.new
    a.rehash
  end

  def test_set_equality
  
    a = NSet.new
    b = NSet.new
    c = NSet.new
    d = NSet.new
    
    for x in [b, c] do
      x.add("a")
    end
    
    d.add("b")
    d.add("b")
    
    a.add(b)
    a.add(c)
    
    puts "a.size()=" + a.size().to_s + ", d.size()=" + d.size().to_s
    puts "b == c: " + (b==c).to_s
    assert_equal(a.size(), d.size())
    assert_equal(b, c)
    assert_equal(a.size(), 1)

    assert_equal(a.subseteq?(a),true)

    assert_equal(a.subset?(a),true)
  end

  def test_hash_mapping
    h = Hash.new
    h["m1"] = "m2"
    assert_equal(h["m1"], "m2")
  end
  
end
