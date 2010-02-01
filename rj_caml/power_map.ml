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
open Rb_set;;

class ['a, 'b] power_map =
object (self) 

  val mutable tree = empty

  method insert (a: 'a) (b: 'b) =
    tree <- transform_mapping a (insert b) empty tree

  method iter (f: ('a * 'b) -> unit) =
    let g a b = f (a,b) in
      iter (fun (a,bs) -> iter (g a) bs) tree

  method remove a b =
    tree <- transform_mapping a (remove b) empty tree

  method image a =
    some_default empty (image a tree)

  method domain =
    fold_left (fun (a,b) s -> insert a s) empty tree

  method to_channel (ch: out_channel) =
    output_value ch tree

  method from_channel (ch: in_channel) =
    tree <- (input_value ch)

end ;;
