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

open Printf
open Action_lang_ast 
open Pig_util
open Query_string
open Assert_string
open Retract_string
open Pig_rules

let generic_action_of_lexbuf next_token main lexbuf = 
  let next_and_report lexbuf =
    try
      next_token lexbuf
    with
      x -> 
        let p = lexbuf.Lexing.lex_curr_p in
          Printf.fprintf stderr 
            "Error %s(%d,%d,%d): Lexer failed.\n  buf=%s\n" 
            p.Lexing.pos_fname p.Lexing.pos_lnum 
            (p.Lexing.pos_cnum) (p.Lexing.pos_bol)
            (Lexing.lexeme lexbuf);
        raise x
  in    
    main next_and_report lexbuf

let action_of_lexbuf =
  generic_action_of_lexbuf Action_lang_lex.token Action_lang_parse.main 

let action_of_string s =
  action_of_lexbuf (Lexing.from_string s)

let action_of_channel ch = 
  action_of_lexbuf (Lexing.from_channel ch)

let action_of_channel_name s =
  if (String.compare s "-") == 0 then
    action_of_channel stdin
  else
    with_in_channel action_of_channel (open_in s)

let rec replace_in_string pat fil text =
  let fst = Usual.find pat text 0 in
    if fst = (String.length text) then
      text
    else
      replace_in_string pat fil 
	(
	  (String.sub text 0 fst) ^ fil ^ 
	  (String.sub text 
	     (fst + (String.length pat)) 
	     ((String.length text) - (String.length pat) - fst)
	  )
	)

(* ---------------------------------------------------------------------- *)


let result_set_from_graph graph qs = 
  new Quad_graph.result_set graph qs

let rec execute_action qs rs_list stmt = 
  let rs_of_string graph_string = 
      let graph = graph_from_string (graph_string ^ ".") qs in
      let sub_graph = 
	List.fold_right (fun rs g -> subst_in_graph g rs qs) rs_list graph 
      in
        result_set_from_graph sub_graph qs
  in
  let sub_rs_list_in_string rs_list string =
    let sub_rs_in_string rs string =
      let sub_var_in_string string var =
	replace_in_string ("{" ^ var ^ "}") (rs#var var) string
      in
	Rb_set.fold_right sub_var_in_string string rs#vars
    in
      List.fold_right sub_rs_in_string rs_list string 
  in
    
    match stmt with
      | For (graph_string, stmt_list) ->
	  let rs = rs_of_string graph_string in
          while not rs#is_null do
            execute_action_list qs (rs :: rs_list) stmt_list;
	    rs#next
          done
      | Print (graph_string) ->
	  print_string (sub_rs_list_in_string rs_list graph_string)
      | PrintLn (graph_string) ->
	  print_endline (sub_rs_list_in_string rs_list graph_string)
      | Assert (graph_string) ->
	  let graph = graph_from_string (graph_string ^ ".") qs in
	  let sub_graph = 
	    List.fold_right (fun rs g -> subst_in_graph g rs qs) rs_list graph 
	  in
	    assert_graph sub_graph qs
      | Retract (graph_string) ->
	  let graph = graph_from_string (graph_string ^ ".") qs in
	  let sub_graph = 
	    List.fold_right (fun rs g -> subst_in_graph g rs qs) rs_list graph 
	  in
	    retract_graph sub_graph qs
and
  execute_action_list qs rs_list stmt_list =
    List.iter 
      (fun stmt -> 
	 execute_action qs rs_list stmt)
      stmt_list

;;

let qs        = Quad_store.make_quad_store Sys.argv.(1) in
let stmt_list = action_of_channel_name Sys.argv.(2) in
  execute_action_list qs [] stmt_list



