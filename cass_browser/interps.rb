 
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

require 'delegate'


module NonFoldable
  def is_foldable
    return false
  end
end

# interface Interp
#   def full_name(i) -- String
#   def short_name(i) -- String
#   def is_foldable -- String
# end

class BasicInterp

  def full_name i
    return i
  end

  def short_name i
    return full_name(i)
  end

  def is_foldable
    return false
  end

end


class ArrayInterp < BasicInterp
  
  def initialize names
    @names = names
  end

  def transform s
    s.gsub(/net\.sourceforge\.toscanaj/, "tj").
      gsub(/\(.*?\)/, "()")
  end

  def full_name i
    return @names[i]
  end

  def short_name i
    return transform(full_name(i))
  rescue
    return "No name for #{i}"
  end

  def is_foldable
    return false
  end
    
end

class NavSpaceArrayInterp < DelegateClass(ArrayInterp)
  
  def initialize interp, is_unfolded_block
    super(interp)
    @is_unfolded_block = is_unfolded_block
  end

  def is_foldable
    return true
  end

  def is_unfolded i
    return @is_unfolded_block.call(full_name(i))
  end

  def is_folded i
    return ! is_unfolded(i)
  end
end

class IdentInterp < BasicInterp
end

class ToStringInterp
  def full_name i
    return i.to_s
  end

  def short_name i
    return full_name(i)
  end

  def is_foldable
    return false
  end
end    

