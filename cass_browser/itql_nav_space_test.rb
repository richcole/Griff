 
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
require 'itql_nav_space'
require 'default_set'

def test_simple
  space = ItqlNavSpace.new
  result = space.context
  context = result['context']
  mi = result['attribute_interp']
  gi = result['object_interp']
  write_formal_context_to_cxt_file(context, gi, mi, STDOUT)
end

test_simple
