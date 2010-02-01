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

open Quad_store ;;
open Quad_graph ;;
open Printf ;;

let qs = make_quad_store_new "test" 

let x1 = tuple_of_string "std_context" "cat" "on" "mat" qs 
let x2 = tuple_of_string "std_context" "cat" "is-a" "animal" qs 
let x3 = tuple_of_string "std_context" "mat" "is-a" "thing" qs 
let x4 = tuple_of_string "std_context" "cat" "is-a" "thing" qs 
let x5 = tuple_of_string "std_context" "cat" "has-name" "claw" qs 

let _ =
  Quad_store.insert x1 qs;
  Quad_store.insert x2 qs;
  Quad_store.insert x3 qs;
  Quad_store.insert x4 qs;
  Quad_store.insert x5 qs;

  printf "std_context=%d, cat=%d, on=%d, mat=%d, is-a=%d\n"
    (index "std_context" qs) (index "cat" qs) (index "on" qs) (index "mat" qs)
    (index "is-a" qs);
  
  printf "animal=%d, thing=%d, has-name=%d, claw=%d\n"
    (index "animal" qs) (index "thing" qs) 
    (index "has-name" qs) (index "claw" qs)

let print_query_structure s qs =
  Rb_set.iter 
    (fun (v, rs) -> 
       if not rs#is_null  then
	 Printf.printf "%s -> %s\n" v (word rs#var qs)
       else
	 Printf.printf "%s -> Null\n" v 
    ) s

let _ =
  printf "std_context: _a is-a animal\n"

let e = array_from_quad 
	   (
	     (Const (index "std_context" qs)), 
	     (Var "_a"), 
	     (Const (index "is-a" qs)),
	     (Const (index "animal" qs))
	   )
;;

let g = Quad_graph.insert e Quad_graph.empty in
  print_query_structure (create_query_structure g qs) qs


let _ =
  printf "std_context: _a is-a _b\n"

let e = array_from_quad 
	   (
	     (Const (index "std_context" qs)), 
	     (Var "_a"), 
	     (Const (index "is-a" qs)),
	     (Var "_b")
	   )

let t = Rb_set.some (qs.trees#image (Pig.make_tuple 0 2 1 3))
let v = Pig.first_gte t (Pig.make_tuple 4 6 0 0) 

let _ =
  printf "first_gte=";
  print_tuple stdout v;
  printf "\n";
