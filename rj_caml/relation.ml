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
open Power_map;;

class ['a,'b] relation =
object
  val im_map : ('a,'b) power_map = new power_map
  val co_map : ('b,'a) power_map = new power_map

  method im_map = im_map
  method co_map = co_map

  method insert x y =
    im_map#insert x y;
    co_map#insert y x

  method image x =
    im_map#image x

  method coimage y =
    co_map#image y

  method is_member x y =
    Rb_set.is_member y (im_map#image x)

  method iter f = im_map#iter f

end;;

type int_relation = (int,int) relation
