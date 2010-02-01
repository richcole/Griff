 
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


module Model

  def init_model
    @subscribers = []
  end

  def set_modified
    if @subscribers then
      for x in @subscribers do
        x.notify(self)
      end
    end
  end

  def subscribe o
    if @subscribers then
      @subscribers.push(o)
    else
      @subscribers = [o]
    end
  end

  def unsubscribe o
    @subscribers.delete(o)
  end

end

