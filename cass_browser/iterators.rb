 
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


def interface_method
  raise "Error: Interface method called"
end

class Ordered
  # return 0 for x == self, -1 for x < self,
  # 1 for x > self 
  def compare_with x
    interface_method
  end
  
  # return true if self is leq x
  def is_leq x
    (self.compare_with x) > 0
  end
  
  # return true if self is geq x
  def is_geq x 
    (self.compare_with x) < 0
  end
  
  # return true if self is equal to x
  def is_eq x
    (self.compare_with x) == 0
  end
end

class Iterator

  # return true if the iterator is at the
  # end of the sequence
  def is_at_end
    interface_method
  end
  
  # point to the first element of the list
  def to_start
    interface_method
  end
  
  # point to the last element of the list
  def to_end
    interface_method
  end
  
end

class ForwardIterator < Iterator

  def initialize
    interface_method
  end

  # advance to the next element
  def iterator
    interface_method 
  end

class OrderedIterator < Iterator

  # goto the next element after x or at_end
  # if there is no such element
  def next_gte x
    interface_method
  end
  
end

class ImmutableSet 

  def 
