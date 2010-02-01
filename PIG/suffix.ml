(* 
Copyright 2004, Richard Cole

This file is part of Griff.

Griff is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your
option) any later version.

Griff is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with Griff; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*)

class suffix stack i =
object(self)
  method stack = stack
  method i = i
  method compare s =
    let rec compare s i t j =
      if (i < s#length) && (j < t#length) then
	let cmp = compare (s#get i) (t#get j) in
	  if cmp < 0 then
	    return -1
	  else if cmp > 0 then
	    return 1
	  else
	    compare s (i+1) t (j+1)
      else if i = s#length && j = t#length then
	return 0
      else if i < s#length && j = t#length then
	return 1
      else
	return -1
end

let compare x y = x#compare y

	  
	  
