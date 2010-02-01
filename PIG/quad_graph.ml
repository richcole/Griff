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

open Array;;
open Printf;;

type vertex =
  | Const of int
  | Var of string

type edge = vertex array

type quad_graph = {
  vs: vertex Rb_set.tree; es: edge Rb_set.tree
}

let empty = { vs=Rb_set.empty; es=Rb_set.empty }

let make_edge (a,b,c,d) =
  let r = Array.make 4 a in
    Array.set r 1 b;
    Array.set r 2 c;
    Array.set r 3 d;
    r

let print_graph g qs =
  let vertex_string = function
      Const(i) -> (Quad_store.word i qs) 
    | Var(v) -> v
  in
  let print_edge e = 
    printf "%s %s %s %s, "  
      (vertex_string e.(0)) (vertex_string e.(1)) 
      (vertex_string e.(2)) (vertex_string e.(3))
  in
    Rb_set.iter print_edge g.es

let insert e g = 
  {
    vs=List.fold_right 
	(fun i z -> Rb_set.insert e.(i) z)
	(Usual.list_range 0 3) 
	g.vs;
    es=Rb_set.insert e g.es
  }
;;

let query_from_edge v e vm =
  let f z () = z in
  let fvm z () = (Rb_set.some (Rb_set.image z vm))#var in
  let r = Array.make 4 (Quad_store.Unbound) in
    for i = 0 to 3 do
      match e.(i) with
	| u when u = v -> Array.set r i Quad_store.Var 
	| Const(c) -> Array.set r i (Quad_store.Bound (f c))
	| Var(v) ->
	    begin
	      match Rb_set.image v vm with
		| Some(x) -> Array.set r i (Quad_store.Bound (fvm v))
		| None -> Array.set r i (Quad_store.Unbound)
	    end
    done;
    r

let edge_contains_v v e =
  let rec edge_contains_v_rec v e i =
    if i < 4 then
      if e.(i) = v then
	true
      else
	edge_contains_v_rec v e (i+1)
    else
      false
  in
    edge_contains_v_rec v e 0
;;
      

let query_list v g vm =
  Rb_set.fold_left 
    (fun e z -> 
       if edge_contains_v v e then
	 (query_from_edge v e vm) :: z
       else
	 z
    )
    []
    g.es
;;

let quad_from_array a =
  (a.(0), a.(1), a.(2), a.(3))
;;

let array_from_quad (a,b,c,d) =
  let r = Array.make 4 a in
    Array.set r 1 b;
    Array.set r 2 c;
    Array.set r 3 d;
    r
;;

let compose f g x = f (g x)
;;

let create_query_structure g qs =
  Rb_set.fold_right
    (fun vm v -> 
       match v with
	 | Const(c) -> vm
	 | Var(v) ->
(*
             printf "Create variable: v=%s, vm=<%s>\n" v 
               (Usual.symbol_seperated ", " 
                  (fun (i,rs) -> i) 
                  vm);
*)

	     Rb_set.insert_mapping v 
	       (Quad_store.make_and_query_results 
		  (List.map 
		     (fun q -> Quad_store.query (quad_from_array q) qs) 
		     (query_list (Var v) g vm)
		  )
	       )
	       vm
    )
    Rb_set.empty
    g.vs
;;

exception NotConst

let is_constant_satisfied g qs =

  (* constraint is that all complete tuples (i.e. tuples not containing
     any variables) in the query actually exist in the data
  *)


  let const_of v =
    match v with Const(a) -> a | _ -> raise NotConst
  in

  let cand_of e =
    Pig.make_tuple 
      (const_of e.(0))
      (const_of e.(1))
      (const_of e.(2))
      (const_of e.(3))
  in

  Rb_set.fold_left
    (fun e z ->
       try
	 let tr = Rb_set.some 
		    (qs.Quad_store.trees#image (Pig.make_tuple 0 1 2 3)) in
	 let cand = cand_of e in 
	   try 
	     z && (cand = (Pig.first_gte tr cand))
	   with
	     | Not_found -> false
       with
	 | NotConst -> z
    )
    true
    g.es

let rec new_solve 
  qs i_option 
  (phi: (string, Quad_store.and_results) Rb_set.map_tree) 
  x 
= 
  match i_option with
    | Some(i,rs) -> 
(*	let x_word = if x = 0 then "Null" else (Quad_store.word x qs) in
        printf "Solve: i=%s - first_gte %s\n" i x_word; flush stdout;   *)

        rs#first_gte x;
        if rs#is_null then
	  (
(*            printf "  rs#is_null\n"; flush stdout;   *)

            false
          )
        else
          (
(*           printf "  rs#var=%s\n" (Quad_store.word rs#var qs); flush stdout;  *)

            let nx = rs#var+1 in
              if new_solve qs (Rb_set.map_succ i phi) phi 0 then
                true
              else
                new_solve qs i_option phi nx
          )
    | None ->
(*        printf "  Solved.\n"; flush stdout;   *)

        true

let rec new_solve_next 
  qs i_option 
  (phi: (string, Quad_store.and_results) Rb_set.map_tree) 
=
  match i_option with
    | None -> 
(*	printf "Solve Next: Success\n"; flush stdout;   *)

        true
    | Some(i,rs) ->
(*	printf "Solve Next: i=%s, is_null=%b\n" i rs#is_null; flush stdout;   *)

        if rs#is_null then
          false
        else
          let nx = rs#var+1 in
            if new_solve_next qs (Rb_set.map_succ i phi) phi then
              true
            else
              new_solve qs i_option phi nx

let print_query_structure s qs =
  Rb_set.iter 
    (fun (v, rs) -> 
       if not rs#is_null  then
	 Printf.printf "%s -> %s\n" v (Quad_store.word rs#var qs) 
       else
	 Printf.printf "%s -> Null\n" v  
    ) s

let vars_of_vertex_set vs = 
  Rb_set.fold_left 
    (fun x z -> match x with Var(x) -> Rb_set.insert x z | _ -> z)
    Rb_set.empty
    vs

class result_set g qs =
object(self)
  val phi    = create_query_structure g qs
  val vars   = vars_of_vertex_set g.vs
  val is_sat = is_constant_satisfied g qs
  val mutable is_at_end = true

  method next =
(*    printf "ResultSetNext: \n"; flush stdout;  *)
 
    (match Rb_set.last phi with
      | Some(i,rs) -> rs#next
      | None -> ());
    is_at_end <- not (new_solve_next qs (Rb_set.first phi) phi)

  method var x =
    Quad_store.word (self#var_index x) qs

  method var_index x =
    match Rb_set.image x phi with
	Some(rs) -> 
	  if rs#is_null then
	    raise Not_found
	  else
	    rs#var
      | None -> raise Not_found

  method vars = vars
  method phi = phi
  method is_satisfied = is_sat

  method is_null =
    (not is_sat) || is_at_end
      
  initializer
    is_at_end <- not (new_solve qs (Rb_set.first phi) phi 0)

end ;;    
