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
open Rb_set

let i  = new Power_map.power_map 
let objects = new set
let attributes = new set

let qs = Quad_store.make_quad_store Sys.argv.(1) 
let rs_g = Query_string.result_set_from_string Sys.argv.(2) qs 
let rs_m = Query_string.result_set_from_string Sys.argv.(3) qs 
let rs_i = Query_string.result_set_from_string Sys.argv.(4) qs 

let _ =
  while not rs_g#is_null do
    objects#insert (rs_g#var "_g");
    rs_g#next
  done;

  while not rs_m#is_null do
    attributes#insert (rs_m#var "_m");
    rs_m#next
  done;

  while not rs_i#is_null do
    i#insert (rs_i#var "_g") (rs_i#var "_m");
    objects#insert (rs_i#var "_g");
    attributes#insert (rs_i#var "_m");
    rs_i#next
  done;

  printf "B\n";
  printf "Test Context\n";
  printf "%d\n%d\n" objects#count attributes#count;
  objects#iter (fun g -> printf "%s\n" g);
  attributes#iter (fun m -> printf "%s\n" m);
  objects#iter 
    (fun g ->
      attributes#iter 
        (fun m -> 
          if is_member m (i#image g) then printf "x" else printf ".");
      printf "\n"
    )

