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

open Printf ;;
open Graph_lang_parse ;;
open Graph_lang ;;

let print_token = function
    CONST(s) -> printf "Lex: CONST '%s'\n" s
  | VAR(s) -> printf "Lex: VAR '%s'\n" s
  | PERIOD -> printf "Lex: PERIOD\n"
  | COMMA -> printf "Lex: COMMA\n"
  | EOF -> printf "Lex: EOF\n"
  | SEMI -> printf "Lex: SEMI\n"
  | DASH -> printf "Lex: DASH\n"
;;

let print_triple_list l =
  let pr = function
      Graph_lang_ast.Var(s) -> "var:" ^ s
    | Graph_lang_ast.Const(s) -> "const:" ^ s
  in
  List.iter (fun (c,s,p,o) -> 
	       let cxt = match c with
		 | Some (cxt) -> pr cxt 
		 | None -> "<<none>>" 
	       in
		 Printf.printf "(%s, %s, %s, %s)\n" cxt (pr s) (pr p) (pr o)) l
;;

let test_input_list = [
  "_a is-a _b.";
  "_a is-a _b, _b is-a _c, _c is-a _d."; 
  "_a is-a _b is-a _c, _a is-a _d."; 
  "\"std_context\" -- _a is-a _b is-a _c, _a is-a _d."; 
  "_a is-a _b; needs _c, _a is-a _d."; 
  "derived_context -- _a is-a file, _a has-type top."
] in
  List.iter (
    fun (s : string) -> 
      printf "Testing: %s\n" s;
      List.iter print_triple_list (triple_list_list_of_string s);
  ) test_input_list
;;
