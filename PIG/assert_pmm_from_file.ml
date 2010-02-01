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

open Pmm_lang
open Pmm_lang_ast 
open Quad_store
open Printf
open Pmm_util

let pmm_file = pmm_of_channel_name Sys.argv.(1)
let qs       = make_quad_store Sys.argv.(2)

exception SemanticError of string

let assert_triple qs a b c =
  printf "assert: std_context %s %s %s\n" a b c;
  Quad_store.insert (tuple_of_strings "std_context" a b c qs) qs

let rec assert_pmm_node qs (node: pmm_node) parent =
  let get_id node =
    try
      get_text (get_first_field "id" node)
    with
      | _ -> (Quad_store.create_uid qs)
  in
  match node.content, parent with
    | Text(t), None    -> raise (SemanticError "Text element with no parent")
    | Text(t), Some(p) -> 
        if node.label = "id" then
          ()
        else assert_triple qs p (node.label) t
    | Children(s), Some(p) -> 
        let nd = get_id node in
          assert_triple qs p node.label nd;
          assert_triple qs nd "is-a" node.label;
          assert_pmm_node_list qs s (Some nd)
    | Children(s), None ->
        let nd = get_id node in
          assert_triple qs nd "is-a" node.label;
          assert_pmm_node_list qs s (Some nd)

and assert_pmm_node_list qs node_list parent = 
  List.iter (fun c -> assert_pmm_node qs c parent) node_list

let _ =
  assert_pmm_node_list qs pmm_file None

          



