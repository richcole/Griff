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
open Pmm_lang_ast 

(* a couple of utility functions *)


let finally_in f g ch =
  let result = 
    try f ch with x -> g ch; raise x 
  in
    g ch; result

let with_in_channel f ch =
  finally_in f close_in ch

(* ppm_of... *)


let generic_ppm_of_lexbuf next_token main lexbuf = 
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

let pmm_of_lexbuf =
  generic_ppm_of_lexbuf Pmm_lang_lex.next_token Pmm_lang_parse.main 

let pmm_of_string s =
  pmm_of_lexbuf (Lexing.from_string s)

let pmm_of_channel ch = 
  pmm_of_lexbuf (Lexing.from_channel ch)

let pmm_of_channel_name s =
  with_in_channel pmm_of_channel (open_in s)

(* manip ppm_nodes *)


let text_node_of indent label text =
  { indent=indent; label=label; content=Text(text) }

let child_node_of indent label =
  { indent=indent; label=label; content=Children []}

let spaces n = String.make n ' '

let rec print_pmm ch node = 
  fprintf ch "%s%s: " (spaces node.indent) (node.label);
  print_pmm_content ch node.content;
and print_pmm_content ch = function
  | Text(t) -> fprintf ch "%s\n" t;
  | Children(s) -> fprintf ch "\n"; List.iter (print_pmm ch) s

let print_pmm_as_xml ch node =
  let field_quote s = 
    Usual.tr (Usual.trim "\n\t " s) ' ' '_'
  in
  let rec print_pmm_as_xml_rec node =
    fprintf ch "%s<%s>" (spaces node.indent) (field_quote node.label);
    print_pmm_content_as_xml_rec node.content;
    fprintf ch "</%s>\n" (field_quote node.label);
  and print_pmm_content_as_xml_rec = 
    function
      | Text(t) -> printf "%s" t;
      | Children(s) -> printf "\n"; List.iter print_pmm_as_xml_rec s
  in
    print_pmm_as_xml_rec node



  
  

