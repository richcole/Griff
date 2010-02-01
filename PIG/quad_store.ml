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

open Pig
open Rb_set
open Printf
;;

let print_tuple ch x = 
  Printf.fprintf ch "%d, %d, %d, %d" x.(0) x.(1) x.(2) x.(3)
;;

(* ordering: context subject predicate object *)


exception Exhausted
exception LogicError of string

type quad_store = { 
  lex: pig_lex; 
  trees: (tuple,pig_tree) Rb_set.map;
}

let make_quad_store filename = 
  let trees = new map in
    trees#insert_mapping (make_tuple 0 1 2 3) 
      (create_tree (filename ^ "_1234.tree"));
    trees#insert_mapping (make_tuple 0 1 3 2) 
      (create_tree (filename ^ "_1243.tree"));
    trees#insert_mapping (make_tuple 0 2 1 3) 
      (create_tree (filename ^ "_1324.tree"));
    trees#insert_mapping (make_tuple 0 2 3 1) 
      (create_tree (filename ^ "_1342.tree"));
    trees#insert_mapping (make_tuple 0 3 1 2) 
      (create_tree (filename ^ "_1423.tree"));
    trees#insert_mapping (make_tuple 0 3 2 1) 
      (create_tree (filename ^ "_1432.tree"));
    {lex=create_lex (filename ^ ".lex"); trees=trees}
;;

let make_quad_store_new filename = 
  let trees = new map in
    trees#insert_mapping (make_tuple 0 1 2 3) 
      (create_tree_new (filename ^ "_1234.tree"));
    trees#insert_mapping (make_tuple 0 1 3 2) 
      (create_tree_new (filename ^ "_1243.tree"));
    trees#insert_mapping (make_tuple 0 2 1 3) 
      (create_tree_new (filename ^ "_1324.tree"));
    trees#insert_mapping (make_tuple 0 2 3 1) 
      (create_tree_new (filename ^ "_1342.tree"));
    trees#insert_mapping (make_tuple 0 3 1 2) 
      (create_tree_new (filename ^ "_1423.tree"));
    trees#insert_mapping (make_tuple 0 3 2 1) 
      (create_tree_new (filename ^ "_1432.tree"));
    {lex=create_lex_new (filename ^ ".lex"); trees=trees}
;;

let tuple_of_strings s1 s2 s3 s4 qs =
  Pig.make_tuple 
    (index qs.lex s1)
    (index qs.lex s2)
    (index qs.lex s3)
    (index qs.lex s4)
;;

let apply_perm perm x =
  let r = Array.make 4 x.(0) in
    Array.set r 0 x.(perm.(0));
    Array.set r 1 x.(perm.(1));
    Array.set r 2 x.(perm.(2));
    Array.set r 3 x.(perm.(3));
    r
;;

let word x qs = Pig.word qs.lex x
;;

let index x qs = Pig.index qs.lex x
;;

let print_tuple_words ch x qs = 
  Printf.fprintf ch "%s, %s, %s, %s" 
    (word x.(0) qs) (word x.(1) qs) (word x.(2) qs) (word x.(3) qs)


let print_perm_and_tuple p px =
  printf "%d %d %d %d: %d %d %d %d\n"
    p.(0) p.(1) p.(2) p.(3) px.(0) px.(1) px.(2) px.(3)

let insert x qs =
  (* 
     printf "^^ Quad_store.insert x="; print_tuple_words stdout x qs; 
     printf "\n"; 
     flush stdout;
  *)

  qs.trees#iter 
    (fun (p,tr) ->
       let px = apply_perm p x in
         (* print_perm_and_tuple p px; *)

         Pig.insert tr px
    )


let is_eq_tuple x y =
  List.fold_right
    (fun i r -> r && (x.(i) = y.(i)))
    [0; 1; 2; 3]
    true

let test_tuple x qs =
  let test_tuple_in_tree (p,tr) result = 
    let px = apply_perm p x in
    let tx = (Pig.first_gte tr px) in
      if is_eq_tuple tx px then
        result
      else
        (print_perm_and_tuple p px; result)
  in
    qs.trees#fold_left test_tuple_in_tree true

let remove x qs =
  (* 
     printf "^^ Quad_store.remove x="; print_tuple_words stdout x qs; 
     printf "\n";
     flush stdout;
  *)

  qs.trees#iter 
    (fun (p,tr) ->
       let px = apply_perm p x in
       Pig.remove tr px
    )
;;

let tuple_of_string x1 x2 x3 x4 qs =
  make_tuple
    (index x1 qs)
    (index x2 qs)
    (index x3 qs)
    (index x4 qs)
;;

let create_uid qs =
  let uid = Printf.sprintf "#%d" ((Pig.index_count qs.lex)+1) in
    Pig.word qs.lex (Pig.index qs.lex uid);
;;

class virtual result_set =
object(self)
  method virtual next: unit
  method virtual first_gte: int -> unit
  method virtual is_null: bool
  method virtual var: index
  method virtual reset: unit
end ;;

type func_tuple = (unit -> index) array ;;

class tuple_result (var_index: index) (tr: pig_tree) (t: func_tuple) =
object(self)
  inherit result_set

  val mutable curr : tuple option = None

  method ith i = (Array.get t i) ()

  method first_gte v =
    let cand = (make_tuple 0 0 0 0) in
      begin
	for i = 0 to var_index-1 do
	  Array.set cand i (self#ith i)
	done;
	Array.set cand var_index v;
	curr <- 
	try 
	  let first = Pig.first_gte tr cand in
(*
	    printf "## cand="; print_tuple stdout cand; 
	    printf ", first="; print_tuple stdout first; 
	    printf "\n"; flush stdout;
*)

	    
	    let rec cmp_n i = 
	      if i = 0 then
		(Array.get cand i) = (Array.get first i)
	      else
		((Array.get cand i) = (Array.get first i)) && cmp_n (i-1)
	    in
	      if cmp_n (var_index-1) then
		Some first
	      else 
		begin
		  (* printf "-- first doesn't match\n"; *)

		  None
		end
	with 
	    Not_found -> None;
      end;

  method next =
    match curr with
      | Some(c) ->
	  self#first_gte (c.(var_index)+1)
      | None -> ()

  method is_null =
    match curr with
	None -> true
      | Some(c) -> 
	  not ((self#ith (var_index-1)) = (Array.get c (var_index-1)))

  method var =
    if not self#is_null then
      match curr with
	| Some(c) -> Array.get c (var_index)
	| None -> 0
    else
      0

  method reset =
    self#first_gte 0

  initializer
    self#reset

end ;;

let max_value max rs =
  let rec max_value_rec max maxed not_maxed =
    match not_maxed with
	[] -> 
	  max
      | h::t -> 
	  h#first_gte max;
	  if h#is_null then
	    begin
	      raise Exhausted
	    end
	  else
	    begin
	      if max < h#var then
		max_value_rec h#var [] (maxed @ not_maxed)
	      else
		max_value_rec max (maxed @ [h]) t
	    end
  in
    try
      Some (max_value_rec max [] rs)
    with
      | Exhausted -> None
;;
	  
class and_results (rsl: tuple_result list) =
object(self)
  inherit result_set

  val mutable curr_max = 
    List.iter (fun rs -> rs#reset) rsl;
    max_value 0 rsl

  method reset =
    List.iter (fun rs -> rs#reset) rsl;
    curr_max <- max_value 0 rsl

  method next =
    match curr_max with
      | Some(cm) ->
	  curr_max <- max_value (cm + 1) rsl
      | None -> 
	  curr_max <- None

  method first_gte v =
    curr_max <- max_value v rsl

  method var = 
    match curr_max with
      | Some(m) -> m
      | None -> 0

  method is_null =
    match curr_max with
	Some(v) -> false;
      | None -> true

end ;;

let make_and_query_results rsl =
  new and_results rsl
;;

let make_result (perm: tuple) (var_index:int) qs (x: func_tuple) =
  match qs.trees#image perm with
    | Some(t) -> new tuple_result var_index t x
    | _ -> raise (LogicError "quad_store.ml: 210")
;;

type variable =
    Bound of (unit -> index)
  | Unbound
  | Var
;;

let make_func_tuple a b c d =
  let r = Array.make 4 a in
    Array.set r 1 b;
    Array.set r 2 c;
    Array.set r 3 d;
    r
;;

let query q qs =
  let z () = 0 in
  match q with
    | (Bound(a),Var,Bound(c),Bound(d)) ->
	make_result (make_tuple 0 2 3 1) 3 qs (make_func_tuple a c d z)
    | (Bound(a),Var,Bound(c),Unbound) ->
	make_result (make_tuple 0 2 1 3) 2 qs (make_func_tuple a c z z)
    | (Bound(a),Var,Unbound,Bound(d)) ->
	make_result (make_tuple 0 3 1 2) 2 qs (make_func_tuple a d z z)
    | (Bound(a),Var,Unbound,Unbound) ->
	make_result (make_tuple 0 1 2 3) 1 qs (make_func_tuple a z z z)

    | (Bound(a),Bound(b),Var,Bound(d)) ->
	make_result (make_tuple 0 1 3 2) 3 qs (make_func_tuple a b d z)
    | (Bound(a),Bound(b),Var,Unbound) ->
	make_result (make_tuple 0 1 2 3) 2 qs (make_func_tuple a b z z)
    | (Bound(a),Unbound,Var,Bound(d)) ->
	make_result (make_tuple 0 3 2 1) 2 qs (make_func_tuple a d z z)
    | (Bound(a),Unbound,Var,Unbound) ->
	make_result (make_tuple 0 2 3 1) 1 qs (make_func_tuple a z z z)

    | (Bound(a),Bound(b),Bound(c),Var) ->
	make_result (make_tuple 0 1 2 3) 3 qs (make_func_tuple a b c z)
    | (Bound(a),Bound(b),Unbound,Var) ->
	make_result (make_tuple 0 1 3 2) 2 qs (make_func_tuple a b z z)
    | (Bound(a),Unbound,Bound(c),Var) ->
	make_result (make_tuple 0 2 3 1) 2 qs (make_func_tuple a c z z)
    | (Bound(a),Unbound,Unbound,Var) ->
	make_result (make_tuple 0 3 2 1) 1 qs (make_func_tuple a z z z)

    | _ -> raise (LogicError "quad_store.ml: 235")
;;



  


