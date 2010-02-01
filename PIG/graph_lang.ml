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

let triple_list_list_of_string 
  : string -> Graph_lang_ast.triple list list
  = fun s ->
  let lexbuf = Lexing.from_string s in
    try
      Graph_lang_parse.main Graph_lang_lex.token lexbuf
    with
      | Parsing.Parse_error ->
          let p = lexbuf.Lexing.lex_curr_p in
            Printf.fprintf stderr "Error Line %d, Char %d\n" 
              p.Lexing.pos_lnum p.Lexing.pos_bol;
            raise Parsing.Parse_error
;;

let triple_list_list_of_file_name
  : string -> Graph_lang_ast.triple list list
  = fun s ->
  let lexbuf = Lexing.from_channel (open_in s) in
    try
      Graph_lang_parse.main Graph_lang_lex.token lexbuf
    with
      | Parsing.Parse_error ->
          let p = lexbuf.Lexing.lex_curr_p in
            Printf.fprintf stderr "Error Line %d, Char %d\n" 
              p.Lexing.pos_lnum p.Lexing.pos_bol;
            raise Parsing.Parse_error
;;
