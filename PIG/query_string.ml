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
open Printf
;;

let graph_from_string s qs = 
  let tl = List.hd (Graph_lang.triple_list_list_of_string s) in
  let rep = function
    | Graph_lang_ast.Const(s) -> Quad_graph.Const (Quad_store.index s qs)
    | Graph_lang_ast.Var(x) -> Quad_graph.Var x in
  let std_cxt = Quad_graph.Const (Quad_store.index "std_context" qs) in
    List.fold_right 
      (
	fun (c,s,p,o) g -> 
	  let cxt = match c with 
	      None -> Graph_lang_ast.Const "std_context" | Some(cxt) -> cxt
	  in 
	    Quad_graph.insert (array_from_quad (rep cxt, rep s, rep p, rep o)) g
      )
      tl Quad_graph.empty  
;;
  
let result_set_from_string s qs =
  let graph = graph_from_string s qs in
    new Quad_graph.result_set graph qs
;;

let query_string s qs =
  let rs = result_set_from_string s qs in
    printf "%s\n" (
      Usual.symbol_seperated "," (fun x -> sprintf "\"%s\"" x) rs#vars);
    while not rs#is_null do
      let s = 
        Usual.symbol_seperated "," 
          (fun x -> sprintf "\"%s\"" (rs#var x)) rs#vars in
      printf "%s\n" s;
      rs#next
    done;
    flush stdout;
;;

