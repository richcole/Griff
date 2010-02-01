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

class ['event] broker =
object (self)

  val mutable subs : ('event -> unit) list = []

  method subscribe x =
    subs <- List.append subs [x]

  method unsubscribe x =
    let is_not_match y = not (x=y) in
      subs <- List.filter is_not_match subs

  method fire_event (event: 'event) =
    let fire sub = sub event in
    List.iter fire subs

end ;;
    
