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

open Cgi;;
open Printf;;
open Rb_set;;
open Graph_lang_quote;;
open Html;;

open Query_string;;
 
let args = let args = Rb_set.from_list (parse_args ()) in

let document_header = 
  sprintf "<html><body><h1> Document </h1>\n"
in

let document_trailer = 
  sprintf "</body></html>\n"
in

let print_row i attr value =
  let attr_input_name = sprintf "attr_%d" i in
  let value_input_name = sprintf "value_%d" i in
  (tr (
     (td (input ~name:attr_input_name ~typ:"text" ~value:(quote attr) "")) ^
     (td (input ~name:value_input_name ~typ:"text" ~value:(quote value) "")
))
  )
in

let invalid_usage () =
  printf "%s\n" mime_header;
  printf "%s\n" (h1 "Invalid Usage");
  printf "%s\n" (p  "Expected argument doc_id was not sent.");
in

try
  let qs = Quad_store.make_quad_store "test" in
  let docid = some (image "doc_id" args) in
  let rs = result_set_from_string 
	     (sprintf "%s _z_attr _value." (quote docid)) qs
  in

  printf "%s" mime_header;
  printf "%s" document_header;
  printf "<table>";

  let prev  = ref None in
  let value = ref "" in
  let i = ref 1 in
  while not rs#is_null do
    begin
      match !prev with
	| Some(prev_attr) ->
	    if prev_attr == rs#var "_z_attr" then
	      if not (!value = "") then
		value := !value ^ ", " ^ (rs#var "_value")
	      else
		value := (rs#var "_value")
	    else
	      begin
		printf "%s" (print_row !i prev_attr !value);
		i := !i + 1;
		value := (rs#var "_value")
	      end
	| None ->	    
	    value := (rs#var "_value")
    end;
    prev := Some (rs#var "_z_attr");
    rs#next;
  done;

  printf "</table>";

    printf "%s\n" document_trailer;
  
  
with
  | Not_found | NotFound -> 
      invalid_usage ()
	

  
