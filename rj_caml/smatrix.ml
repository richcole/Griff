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
open Rb_set ;;
open File  ;;

class matrix =
object (self)

  val array : ((int * int), float) map = new map

  method resize m n =
    let 
      pred ((i,j),v) = (i > m) or (j > n) or (i < 1) or (j < 1) 
    in
      array#remove_if pred

  method iter (f: ((int * int) * float) -> unit) =
    array#iter f 

  method fold_left: 'a. (((int * int) * float) -> 'a -> 'a) -> 'a -> 'a
    = fun f z ->
      fold_left f z array#tree

  method set i j v =
    array#insert_mapping (i,j) v

  method get i j =
    match array#image (i,j) with
	Some(x) -> x
      | None -> 0.0

  method tree = array#tree
		  
  method incr i j v =
    self#set i j (v +. (self#get i j))

  method to_file ch =
    let pr ((i,j),x) = 
	output_float ch (float_of_int i);
	output_float ch (float_of_int j);
	output_float ch x;
    in
	output_int ch (array#count);
	output_int ch 3;
	array#iter pr

    method from_file ch =
      let 
	len  = input_int ch and
	cols = input_int ch
      in
	for row = 1 to len do
	  let 
	    i = int_of_float (input_float ch) and
	    j = int_of_float (input_float ch) and
	    x = input_float ch
	  in
	    self#set i j x
	done

    method print ch =
      let pr ((i,j),v) =
	Printf.fprintf ch "%d, %d, %e\n" i j v 
      in
	Printf.fprintf ch "Sparse Matrix (%d,3) \n" (array#count);
	array#iter pr;

    method array = array

end ;;	    
	
let compare a b =
  compare a#array b#array
