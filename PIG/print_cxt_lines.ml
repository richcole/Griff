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

open Graph_lang
open Quad_graph
open Printf
;;

let qs = Quad_store.make_quad_store Sys.argv.(1) in
let rs = Query_string.result_set_from_string Sys.argv.(2) qs in
  while not rs#is_null do
    let g_query = sprintf "\"%s\" _m _w." (rs#var "_a") in
    let g_rs = Query_string.result_set_from_string g_query qs in
      while not g_rs#is_null do
	printf "%s:%s, " (g_rs#var "_m") (g_rs#var "_w");
	g_rs#next;
      done;
      printf "\n";
      rs#next
  done
;;
