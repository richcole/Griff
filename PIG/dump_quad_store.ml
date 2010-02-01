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

open Query_string 
open Printf
;;

let qs = Quad_store.make_quad_store Sys.argv.(1) in

let print_tuple t =
  for i = 0 to (Array.length t) - 1 do
    printf "%s, " (Quad_store.word t.(i) qs);
  done
in

let dump_tree tr =  
    try 
      let curr = ref (Pig.first_gte tr (Pig.make_tuple 0 0 0 0)) in
	while true do
	  printf "  ";
	  print_tuple !curr;
	  printf "\n";
	  curr := Pig.succ tr !curr
	done
    with
	Not_found -> printf "End of tree.\n"
in

    qs.Quad_store.trees#iter
      (fun (perm,tr) -> 
	 printf "Dumping tree %d %d %d %d:\n" 
         perm.(0) perm.(1) perm.(2) perm.(3);
	 dump_tree tr;
      )

		 
;;
