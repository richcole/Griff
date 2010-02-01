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

open Quad_graph ;;
open Query_string ;;
open Printf ;;

let print_document_header s =
  printf "Document %s:\n" s;
in

let print_attribute doc m w =
  printf "  %s: %s\n" m w;
in

let qs = Quad_store.make_quad_store Sys.argv.(1) in
let docs = result_set_from_string "_a is-a document." qs in
  while not docs#is_null do
    let doc = (docs#var "_a") in
      print_document_header doc;
      let attrs = result_set_from_string (sprintf "\"%s\" _at _va." doc) qs in
	while not attrs#is_null do
	  print_attribute doc (attrs#var "_at") (attrs#var "_va");
	  attrs#next
	done;
	docs#next
  done
;;
	
