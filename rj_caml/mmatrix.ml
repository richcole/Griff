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
open Array ;;
open File  ;;

class matrix =
object (self)

  val mutable m     = 0
  val mutable n     = 0
  val mutable array = Array.make_matrix 0 0 0.0

  method m = m
  method n = n

  method resize new_m new_n =
    let new_array = 
      Array.make_matrix new_m new_n 0.0 in
    let copy_to src dest =
      Array.blit src 0 dest 0 (min n new_n) 
    in
      for i = 1 to (min m new_m) do
	copy_to (get array (i-1)) (get new_array (i-1))
      done;
      array <- new_array;
      m <- new_m;
      n <- new_n

  method incr i j v =
    self#set i j (v +. (self#get i j))

  method set i j v =
    set (get array (i-1)) (j-1) v

  method get i j =
    get (get array (i-1)) (j-1)

  method to_file ch =
    output_int ch m;
    output_int ch n;
    for i = 1 to m do
      for j = 1 to n do
	output_float ch (self#get i j)
      done
    done

  method from_file ch =
    m <- input_int ch;
    n <- input_int ch;
    array <- make_matrix m n 0.0;
    for i = 1 to m do
      for j = 1 to n do
	self#set i j (input_float ch)
      done
    done

  method print ch =
    let pr array =
      Array.iter (fun x -> Printf.fprintf ch "%e " x) array;
      Printf.fprintf ch "\n";
    in
      Printf.fprintf ch "Matrix (%d,%d)\n" m n;
      Array.iter pr array;

  method array = array

  method iter (f: ((int * int) * float) -> unit) =
    for i=1 to m do
      for j=1 to n do
	f ((i,j),self#get i j);
      done
    done

end ;;	    
	
let compare a b =
  compare (a#array) (b#array)

	
      
