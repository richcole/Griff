 
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


require 'bit_set'
require 'default_set'

class ExtentSet < BSet

  def initializer(lex=Lexicon.new)
    super(lex)
  end

  def add_extent(gs)
    new_extents = make_default_set(@lex)
    new_extents.add(gs)
    for x in self do
      nx = x.clone
      nx.and_equals(gs)
      new_extents.add(nx)
    end
    self.or_equals(new_extents)
  end

  def add_extents_from_context(k)
    self.add_extent(k.objects)
    for m in k.attributes do
      self.add_extent(k.attribute_extent(m))
    end
  end

end

def extent_set_of_context(k)
  e = ExtentSet.new(Lexicon.new)
  e.add_extents_from_context(k)

  return e
end
      
