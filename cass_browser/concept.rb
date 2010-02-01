 
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
require 'value_object'

class Concept

  attr_reader :extent, :intent
  attr_reader :object_contingent, :attribute_contingent
  attr_writer :object_contingent, :attribute_contingent

  include ValueObject

  def initialize extent, intent
    @extent = extent.clone() 
    @intent = intent.clone()
  end

  def clone
    return Concept.new(@extent, @intent)
  end

  def hash
    return @extent.hash
  end

  def set_object_contingent (s)
    @object_contingent = s
  end

  def set_attribute_contingent (s)
    @attribute_contingent = s
  end

  def to_s
    s = "[Concept:#{self.id} (#{@extent}, #{@intent})]"
    return s
  end

end      
      

  
