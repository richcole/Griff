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


type pig_tree
type pig_lex

type index = int
type tuple = int array

external ml_create_tree : string -> pig_tree = "ml_pig_create_tree"
external ml_create_tree_new : string -> pig_tree = "ml_pig_create_tree_new"

external ml_create_lex : string -> pig_lex = "ml_pig_create_lex"
external ml_create_lex_new : string -> pig_lex = "ml_pig_create_lex_new"

external insert : pig_tree -> tuple -> unit = "ml_pig_insert"
external remove : pig_tree -> tuple -> unit = "ml_pig_remove"
external first_gte : pig_tree -> tuple -> tuple = "ml_pig_first_gte"
external succ   : pig_tree -> tuple -> tuple = "ml_pig_succ"

external index  : pig_lex -> string -> int = "ml_pig_lex_index"
external word   : pig_lex -> int -> string = "ml_pig_lex_word"

external index_count : pig_lex -> int = "ml_pig_lex_index_count"

external finalize_tree : pig_tree -> unit = "ml_do_pig_finalize"
external finalize_lex : pig_lex -> unit = "ml_do_pig_finalize"

let create_tree s =
  let p = ml_create_tree s in
    at_exit (fun () -> finalize_tree p);
    p

let create_tree_new s =
  let p = ml_create_tree_new s in
    at_exit (fun () -> finalize_tree p);
    p

let create_lex s =
  let p = ml_create_lex s in
    at_exit (fun () -> finalize_lex p);
    p

let create_lex_new s =
  let p = ml_create_lex_new s in
    at_exit (fun () -> finalize_lex p);
    p

let make_tuple a b c d =
  let r = Array.make 4 a in
    Array.set r 1 b;
    Array.set r 2 c;
    Array.set r 3 d;
    r
;;

Callback.register_exception "Not_found" Not_found;;




