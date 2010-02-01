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
open Printf ;;

let qs = make_quad_store_new "test" ;;

let x1 = tuple_of_string "std_context" "cat" "on" "mat" qs in
let x2 = tuple_of_string "std_context" "cat" "is-a" "animal" qs in
let x3 = tuple_of_string "std_context" "mat" "is-a" "thing" qs in
let x4 = tuple_of_string "std_context" "cat" "is-a" "thing" qs in
let x5 = tuple_of_string "std_context" "cat" "has-name" "claw" qs in

insert x1 qs;
insert x2 qs;
insert x3 qs;
insert x4 qs;
insert x5 qs;

let tr = (Rb_set.some (qs.trees#image (Pig.make_tuple 0 1 2 3))) in
let first = (Pig.first_gte tr x1) in

let rec print_all v =
  try
    Printf.printf "%s: %s %s %s.\n" 
      (word v.(0) qs) (word v.(1) qs) (word v.(2) qs) (word v.(3) qs);
    print_all (Pig.succ tr v);
  with
    | Not_found -> ()
in
  print_all first;

let f i () = x3.(i) in
let rs = query (Bound (f 0), Var, Bound (f 2), Bound (f 3)) qs in

  Printf.printf "query= %s: _a %s %s\n" 
    (word (f 0 ()) qs) (word (f 2 ()) qs) (word (f 3 ()) qs);
  while not rs#is_null do
    Printf.printf "var=%s\n" (word rs#var qs);
    rs#next
  done;

;;

let t = Rb_set.some (qs.trees#image (Pig.make_tuple 0 2 1 3))  in
let v = Pig.first_gte t (Pig.make_tuple 4 6 0 0) in
  printf "first_gte=";
  print_tuple stdout v;
  printf "\n";






    
