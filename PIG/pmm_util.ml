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
open Printf
open Usual

let comma_sep_url_list arg s = 
  Usual.symbol_seperated_list ", " 
    (fun k -> sprintf "<a href=\"?%s=%s\">" arg
       (Usual.tr (Usual.trim "\n\t " k) ' ' '+' ))
    (Str.split (Str.regexp ", ") s)

let rec first_that pred =
  function 
    | [] -> None
    | h :: t when pred h -> Some h
    | h :: t -> first_that pred t

let is_node_label_eq name node = 
  node.label = name

let get_text node = 
  match node.content with Text(t) -> t | _ -> "Child is not text"

let get_fields name node =
  match node.content with
    | Children(s) -> List.filter (is_node_label_eq name) s;
    | Text(t) -> []

let get_label node = 
  node.label

let get_first_field name node =
  List.hd (get_fields name node)

let get_first_field_text name node =
  try
    get_text (get_first_field name node)
  with
    | Not_found -> "Error: Not Found"
    | Failure (s) -> "Error: Not Found"

let iter_children name f node =
  List.iter f (get_fields name node)

let fold_right_children name f node z =
  List.fold_right f (get_fields name node) z

let select_nodes = List.filter

let with_field_contains expr field value node =
  let contains_value field_node =
    List.mem value 
      (Str.split expr (get_text field_node))
  in
    List.length (List.filter contains_value (get_fields field node)) > 0

let with_field_contains_csv = 
  with_field_contains (Str.regexp ", +") 

let with_field_contains_asv =
  with_field_contains (Str.regexp " and ") 

let with_field field value node =
  let has_field field_node =
    (get_text field_node) = value
  in
    List.length (List.filter has_field (get_fields field node)) > 0

let with_trim_field field value node =
  let has_field field_node =
    (trim (get_text field_node)) = (trim value)
  in
    List.length (List.filter has_field (get_fields field node)) > 0

let nodes_with_field field value node_list = 
  List.filter (with_field field value) node_list

let node_with_field field value node_list = 
  List.hd (nodes_with_field field value node_list)

let nodes_with_trim_field field value node_list = 
  List.filter (with_trim_field field value) node_list

let node_with_trim_field field value node_list =
  List.hd (nodes_with_trim_field field value node_list)
  

