 
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
require 'formal_context_examples'
require 'concept_lattice'
require 'diagram'

class TC_Diagram < Test::Unit::TestCase

  def test_B2
    k = make_B2
    l = ConceptLattice.new(k)
    rep = MeetIrrRep.new(l)
    pos = AdditiveDiagram.new(rep)

    for x in l do
      p = pos[x]
      puts "Concept #{x}, pos: (#{p[0]},#{p[1]})"
    end
  end

end
