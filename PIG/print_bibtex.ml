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
open Pmm_util
open Pmm_lang_ast
open Printf
open Rb_set
open Usual

let warning s =
  fprintf stderr "WARNING: %s\n" s; flush stderr

let log s = 
  fprintf stderr "LOG: %s\n" s; flush stderr

let output s =
  fprintf stdout "%s" s; flush stdout

let trim_text = trim "\n\t \"'{}," 

let get_field_text name node =
  get_text (get_first_field name node)

let collect_asl_from_n 
  (field: string) (node: pmm_node) (z: string Rb_set.tree) =
  let collect_asl field_node z =
    let pat = (Str.regexp " and ") in
    let asl = Str.split pat (get_text field_node) in
      List.fold_right Rb_set.insert asl z
  in
    fold_right_children field collect_asl node z

let collect_asl_field_from_nl field nodes =
  List.fold_right (collect_asl_from_n field) nodes Rb_set.empty

let collect_field_from_n field node field_set =
  try
    match node.label with
      | "paper" | "proceedings" | "journal" ->
          let text = get_field_text field node in
            if is_member text field_set then
              warning (sprintf "Repeated id: %s" text)
            else ();
            insert text field_set
      | x -> field_set
  with
    | Failure(f) ->
        warning "paper has no id.";
        field_set

let collect_field_from_nl field nodes =
  List.fold_right (collect_field_from_n field) nodes Rb_set.empty

let bibtex_type_of_entry node =
  try
    let t = get_field_text "type" node in
      match t with
        | "conference paper" -> "InCollection"
        | "journal paper" -> "Article"
        | "paper in conference proceedings" -> "InCollection"
        | "conference paper?" -> "Article"
        | "technical report" -> "TechnicalReport"
        | x -> 
            warning 
              (sprintf 
                 "Paper with id: %s has unrecognised type '%s', assuming article." 
                 (get_field_text "id" node)
                 x
              );
            "Article"
  with
    | Failure(f) -> 
        let bt = 
          match node.label with 
            | "book" -> "Book"
            | "paper" -> "Article"
            | "technical report" -> "TechReport"
            | x ->
                warning 
                  (sprintf "Paper with unknown entry type: %s" x);
                  "Article"
        in
          warning 
            (sprintf 
               "Paper with id: %s has no type entry, assuming %s." 
               (get_field_text "id" node)
               bt
          );
          bt

let print_bibtex_simple_field field bibtex_field node =
  try
    let text = trim_text (get_field_text field node) in
      sprintf "  %s = {%s},\n" bibtex_field text
  with
      Failure(f) ->
        warning
          (sprintf
             "Paper with id: %s has no field %s"
             (get_field_text "id" node)
             field
          );
        ""

let process_published node nodes =
  let result = ref empty_map in
  let try_insert bibtex_field field node fields =
    try
      let fv = trim_text (get_field_text field node) in
        if (String.length fv) != 0 then
          insert_mapping bibtex_field fv fields
        else
          fields
    with
      | _ -> fields 
  in
    begin
      try
        let published = 
          get_first_field "published" node in
        let _ = 
          log "Found published."
        in
        let booklet = 
          let b = get_first_field "in" published in
            match b.content with
              | Text(t) -> 
                  begin
                    try node_with_field "id" t nodes with
                      | Failure(f) ->
                          warning 
                            (sprintf "Unable to find entry for booklet '%s'" t);
                          result := try_insert "booktitle" "in" published !result;
                          b
                  end
              | Children(c) -> b
        in
        let _ = 
          log "Found booklet."
        in
          result := try_insert "booktitle" "title" booklet !result;
          result := try_insert "booktitle" "title" booklet !result;
          result := try_insert "editors" "editors" booklet !result;
          result := try_insert "year" "year" booklet !result;
          
          result := try_insert "pages" "pages" published !result;
          result := try_insert "volume" "volume" published !result;
          result := try_insert "pages" "pages" published !result;
          result := try_insert "year" "year" published !result;
      with _ -> ()
    end;
    log (sprintf "Count=%d" (length !result));
    !result

let print_bibtex_entry nodes id =
  try
    let node  = node_with_field "id" id nodes in
    let bt    = bibtex_type_of_entry node in
    let pp    = process_published node nodes in
      output (sprintf "\n@%s{%s,\n" bt (trim_text id));
      output (print_bibtex_simple_field "title" "title" node);
      output (print_bibtex_simple_field "authors" "authors" node);
      iter (fun (bf,fv) -> output (sprintf "  %s = {%s},\n" bf fv)) pp;
      output "  note = {}\n}\n"
  with
    | Failure(f) -> warning (sprintf "Exception Failure: id='%s'\n" id)

let print_bibtex print_bibtex_entry nodes =
  let ids: string tree = collect_field_from_nl "id" nodes in
    Rb_set.iter (print_bibtex_entry nodes) ids

let ch = stdout 
let nl = pmm_of_channel_name Sys.argv.(1) 

let _ =
    print_bibtex print_bibtex_entry nl;

