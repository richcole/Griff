 
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
require 'default_set'

class TC_FormalContext < Test::Unit::TestCase

  def test_B2
    k = FormalContext.new
    
    k.insert_pair("g1","m1")
    k.insert_pair("g2","m2")
    
    lex = Lexicon.new

    a0 = make_default_set(lex, [])
    a1 = make_default_set(lex, ["g1"])
    a2 = make_default_set(lex, ["g2"])
    a3 = make_default_set(lex, ["g1", "g2"])
    
    b0 = make_default_set(lex, [])
    b1 = make_default_set(lex, ["m1"])
    b2 = make_default_set(lex, ["m2"])
    b3 = make_default_set(lex, ["m1", "m2"])

    assert(k.object_intent("g1").eql?(b1))
    assert(k.object_intent("g2").eql?(b2))
    assert(k.attribute_extent("m1").eql?(a1))
    assert(k.attribute_extent("m2").eql?(a2))    
    assert(k.intent(a3).eql?(b0))
    assert(k.intent(a1).eql?(b1))
   end
end
