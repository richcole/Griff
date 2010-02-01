 
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


require 'default_set'
require 'lexicon'
require 'power_map'

class FormalContext 

# public interface

  attr_reader :objects, :attributes

  def initialize
    @objects = make_default_set()
    @attributes = make_default_set()
    @intent_map = make_default_power_map(@attributes.lex)
    @extent_map = make_default_power_map(@objects.lex)
  end

  def replace_sets(objects, attributes)
    @objects = objects
    @attributes = attributes
    @intent_map = make_default_power_map(attributes.lex)
    @extent_map = make_default_power_map(objects.lex)
  end
    
  
  # insert a pair into the formal context
  def insert_pair g, m
    @objects.add(g)
    @attributes.add(m)
    @intent_map[g].add(m)
    @extent_map[m].add(g)
  end
  
  # add an object to the context
  def insert_object g
    @objects.add g
  end
  
  # add an attribute to the context
  def insert_attribute m
    @attributes.add m
  end
  
  # remove a pair from the formal context
  def remove_object g
    @objects.remove g
    @intent_map.delete g
    for m in @extent_map do
      extent_map[m].delete g
    end
  end
  
  # the result of this method is not to be edited
  def object_intent g
    @intent_map[g]
  end
  
  # the result of this method is not to be edited
  def attribute_extent m
    @extent_map[m]
  end
  
  # calculate the intent of xA, the result is new
  # and may be edited
  def intent xA
    result = @attributes.clone
    for g in xA do
      result.and_equals(self.object_intent(g))
    end
    result
  end
  
  # calculate the extent of xB, 
  def extent xB
    result = @objects.clone
    for m in xB do
      result.and_equals(self.attribute_extent(m))
    end
    result
  end
  
  def object_closure xA
    self.extent(self.intent(xA))
  end
  
  def attribute_closure xB
    self.extent(self.intent(xB))
  end

  def to_s
    s = "{ FormalContext:#{self.id}\n"
    s << "  objects: #{@objects}\n"
    s << "  attributes: #{@attributes}\n"
    s << "  intents:\n"
    for g in @objects do
      s << "    #{g} -> #{self.object_intent(g)}\n"
    end
    s << "  extents:\n"
    for m in @attributes do
      s << "    #{m} -> #{self.attribute_extent(m)}\n"
    end
    s << "}\n"
    return s
  end

end
    
