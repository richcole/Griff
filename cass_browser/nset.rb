 
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


require 'set'

class NSet < Set

  def hash
    result = 0
    i = 0
    primes = [ 3, 5, 7, 13, 17, 23 ]
    for x in self do
      i = (i+1) % primes.size()
      result ^= (x.hash() << primes[i])
    end
    return result
  end

  def rehash
    @hash.rehash
  end

  def or_equals s =
    for x in s do
      self.add x
    end
  end

  alias count length
  
  def eql? s
    if self.size == s.size then
      for x in self do
        if not s.include? x then
          return false
        end
      end
      return true
    else
      return false
    end
  end

  def subseteq? s
    for x in self do
      if not (s.include?(x)) then
        return false
      end
    end
    return true
  end

  def to_s
    s = "{"
    for x in self do 
      s << "#{x}, " 
    end
    s << "}"
    return s 
  end


end    
