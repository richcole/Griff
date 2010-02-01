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

open Wiki_lang
open Pmm_lang
open Pmm_util
open Printf
;;

let rec print_pmm_as_source ch n =
  match n.Pmm_lang_ast.label with
    | "verbatim" ->
        fprintf ch "%s" (get_text n);
    | _ -> 
        match n.Pmm_lang_ast.content with 
          | Pmm_lang_ast.Children(c) -> List.iter (print_pmm_as_source ch) c;
          | _ -> ()
;;

let ch = stdout in
  List.iter (print_pmm_as_source ch) (wiki_of_channel_name Sys.argv.(1));
    


  
    
  
