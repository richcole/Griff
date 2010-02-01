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

open Quad_store ;;
open Quad_graph ;;
open Query_string ;;
open Printf ;;
open Rb_set ;;

let print_document_header s =
  printf "Document %s:\n" s;
in

let print_attribute doc m w =
  printf "  %s: %s\n" m w;
in

let rec read_pairs ch ps qs =
  try 
    let m = index (input_line ch) qs in
    let w = index (input_line ch) qs in
    let p =  (m,w) in
      read_pairs ch (insert p ps) qs
  with
      End_of_file -> ps
in

let write_query ps qs =
  let write_p (m,w) z =
    if z = "" then
      sprintf "_a \"%s\" \"%s\"." (word m qs) (word w qs)
    else
      sprintf "_a \"%s\" \"%s\", %s" (word m qs) (word w qs) z
  in
    "derived_context -- " ^ (fold_left write_p "" ps)
in

let qs     = Quad_store.make_quad_store Sys.argv.(1) in
let ps     = read_pairs stdin empty qs in
let a_dash_query = write_query ps qs in
  printf "\n%% %s\n" a_dash_query; flush stdout;
let a_dash = result_set_from_string a_dash_query qs in
let phi    = new map in
  
  (* calculate phi *)

  while not a_dash#is_null do
    let g_dash_query = sprintf "derived_context -- \"%s\" _m _w." (a_dash#var "_a") in
      printf "\n%% %s\n" g_dash_query; flush stdout;
    let g_dash_rs = result_set_from_string g_dash_query qs in
    let g_dash = new set in

      while not g_dash_rs#is_null do
        g_dash#insert (g_dash_rs#var_index "_m", g_dash_rs#var_index "_w");
        g_dash_rs#next
      done;

      g_dash#remove_if (fun x -> is_member x ps);
      
      g_dash#iter 
        (fun (m,w) ->
           phi#transform_mapping 
           (m,w) 
           (intersection g_dash#tree) 
           g_dash#tree
        );
      
      a_dash#next
  done;

  (* filter phi according to: for n in phi(m) phi(n) = phi(m) *)


  let is_maximal (m,phi_m) phi =
    fold_left (fun x z -> z && ((some (image x phi)) = phi_m)) true phi_m 
  in
    
  let generators = 
    fold_left
      (fun (m,phi_m) z -> 
	 if is_maximal (m,phi_m) phi#tree then
	   insert m z
	 else z
      )
      empty
      phi#tree
  in

  let print_concept m phi =
    let ms = some (image m phi) in
      printf "Concept: ";
      iter (fun (m,w) -> printf "%s:%s, " (word m qs) (word w qs)) ms;
      printf "\n";
  in

  let visited = new set in

    iter 
      (fun m -> 
	 if not (visited#is_member m) then
	   begin
	     print_concept m phi#tree;
	     iter (fun n -> visited#insert n) (some (image m phi#tree))
	   end
	 else ()
      )
      generators
  
;;
