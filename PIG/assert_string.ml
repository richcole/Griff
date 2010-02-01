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

open Graph_lang 
open Quad_graph
open Rb_set ;;


let assert_graph_from_source source s qs =
  let rep x mp qs = 
    match x with
      | Graph_lang_ast.Const(xv) -> xv
      | Graph_lang_ast.Var(xv) -> 
	  match mp#image xv with
	      Some(y) -> y
	    | None -> 
		let y = (Quad_store.create_uid qs) in 
		  mp#insert_mapping xv y; 
		  y
  in
  let tll = source s in
    List.iter 
      (fun tl -> 
         let mp = new map in
         List.iter
         (fun (c,s,p,o) -> 
	    Quad_store.insert 
	    (Quad_store.tuple_of_string 
	       (match c with 
		    None -> ("std_context") | Some(cxt) -> (rep cxt mp qs) )
	       (rep s mp qs) (rep p mp qs) (rep o mp qs) qs)
	    qs
         )
         tl)
      tll
;;

let assert_string s qs = 
  assert_graph_from_source triple_list_list_of_string s qs
;;

let assert_graph g qs =
  let rep x mp qs = 
    match x with
      | Quad_graph.Const(i) -> (Quad_store.word i qs)
      | Quad_graph.Var(x) -> 
	  match mp#image x with
	      Some(y) -> y
	    | None -> 
		let y = (Quad_store.create_uid qs) in 
		  mp#insert_mapping x y; 
		  y
  in
  let mp = new map in
    Rb_set.iter 
      (fun e -> 
	 Quad_store.insert  
	 (Quad_store.tuple_of_string 
	    (rep e.(0) mp qs) (rep e.(1) mp qs) 
	    (rep e.(2) mp qs) (rep e.(3) mp qs) 
	    qs
	 )
	 qs
      )
      g.es
;;

