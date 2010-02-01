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
{
open Action_lang_parse 
open Rb_set

let reserved_map = 
  List.fold_right 
    (fun (x,y) mp -> insert_mapping x y mp) 
    [ 
       "for", FOR; "do", DO; "assert", ASSERT;
       "done", DONE; "print", PRINT; "println", PRINTLN;
       "retract", RETRACT;
    ]
    empty

let get_reserved s =
  match image s reserved_map with
    | Some(t) -> t
    | None -> IDENT s

}

rule token = parse
  '"' ( ( [^'"'] | "\\\"" ) * as content ) '"'	
      { 
         let dequoted = Str.global_replace (Str.regexp "\\\\\"") "\"" content in
	   (* Printf.fprintf stderr "Quoted string: '%s'\n" dequoted; *)

           STRING dequoted
      }     
  | ['a'-'z''A'-'Z']+ as ident
      { get_reserved ident }
  | [' ''\t''\n''\r']+
      { token lexbuf }
  | eof            { EOF }
