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

let qs = Quad_store.make_quad_store Sys.argv.(1) 
let rs = Query_string.result_set_from_string Sys.argv.(2) qs 

let print_context name i =
  printf "B\n";
  printf "%s\n" name;
  let gs = new set in
  let ms = new set in
    i#iter (fun (g,m) -> gs#insert g; ms#insert m);
  printf "%d\n%d\n" (gs#count) (ms#count);
  gs#iter (fun x -> printf "%s\n" x);
  ms#iter (fun x -> printf "%s\n" x);
  gs#iter
    (fun g ->
       ms#iter 
         (fun m -> 
            if is_member m (i#image g) then printf "x" else printf ".");
       printf "\n";
    )

let _ =
  while not rs#is_null do
    i#insert (rs#var "_g") (rs#var "_m");
    rs#next
  done;

  print_context "Formal Context" i;

