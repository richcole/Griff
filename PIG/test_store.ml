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

open Pig
open Quad_store 
open Printf 
open Sys
open Query_string

let query_list = [
  ( "_a _b _c.", "_a", "_b", "_c" );
  ( "_a _c _b.", "_a", "_c", "_b" );
  ( "_b _a _c.", "_b", "_a", "_c" );
  ( "_b _c _a.", "_b", "_c", "_a" );
  ( "_c _a _b.", "_c", "_a", "_b" );
  ( "_c _b _a.", "_c", "_b", "_a" )
]

let qs = make_quad_store argv.(1) 

let do_test (query_string, sa, sb, sc) =
  let rs = result_set_from_string query_string qs in
    while not rs#is_null do
      let tuple = make_tuple 
                    (index "std_context" qs) 
                    (index (rs#var sa) qs) 
                    (index (rs#var sb) qs) 
                    (index (rs#var sc) qs)
      in
        ignore ( test_tuple tuple qs );
        rs#next
    done

let _ =
  List.iter do_test query_list







