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
open Pmm_lang

let finally_in f g ch =
  let result = 
    try f ch with x -> g ch; raise x 
  in
    g ch; result

let with_in_channel f ch =
  finally_in f close_in ch

(* wiki_of... *)


let wiki_of_lexbuf = 
  generic_ppm_of_lexbuf Wiki_lang_lex.next_token Wiki_lang_parse.main 

let wiki_of_string s =
  wiki_of_lexbuf (Lexing.from_string s)

let wiki_of_channel ch = 
  wiki_of_lexbuf (Lexing.from_channel ch)

let wiki_of_channel_name s =
  with_in_channel wiki_of_channel (open_in s)

