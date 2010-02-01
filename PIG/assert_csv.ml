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

open Usual
open Str
open Csv_lang_lex
open Printf
;;

let lexbuf = Lexing.from_string 
;;

let insert_triple_from_line qs line = 
  let lexbuf = Lexing.from_string line 
  in

  let read_token lexbuf = 
    match token lexbuf with
        IDENT(s) -> s
      | EOF -> raise End_of_file
  in

    try
      let a = read_token lexbuf in
      let b = read_token lexbuf in
      let c = read_token lexbuf in
        Quad_store.insert
          (Quad_store.tuple_of_strings "std_context" a b c qs) qs;
    with
      | End_of_file -> printf "End_of_file.\n";
;;



let qs = Quad_store.make_quad_store Sys.argv.(1) in
let ch = open_in Sys.argv.(2) in
  until_eof_channel (fun () ch -> insert_triple_from_line qs (Pervasives.input_line ch)) () ch;
  close_in ch;
