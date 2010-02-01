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

let qs     = Quad_store.make_quad_store Sys.argv.(1) in


(* iterate over rules *)

let rule_rs = result_set_from_string 
		"_a is-a rule, _a has-premise _p, _a has-disclusion _c."  qs 
in

let subst_in_graph g rs qs =
  let tr = function
    | Var(u) -> 
	if (is_member u rs#vars) then
	  begin
	    printf "@@ subst u=%s -> %s\n" u (word (rs#var_index u) qs);
	    Const(rs#var_index u)
	  end
	else
	  Var(u)
    | Const(s) -> Const(s)
  in
    fold_left 
      (fun e z -> 
	 Quad_graph.insert (make_edge (tr e.(0), tr e.(1), tr e.(2), tr e.(3))) z)
      Quad_graph.empty g.es
in

let finished = ref false in

while not !finished do

  finished := true;

  while not rule_rs#is_null do
    printf "** rule: %s -> %s\n" (rule_rs#var "_p") (rule_rs#var "_c");
    flush stdout;
    let premise_rs = result_set_from_string (rule_rs#var "_p") qs in
    let conc_graph = graph_from_string (rule_rs#var "_c") qs in
      while not premise_rs#is_null do
	let conc_sub_graph = subst_in_graph conc_graph premise_rs qs in
	let conc_sub_rs = new Quad_graph.result_set conc_sub_graph qs in
	  printf "** conc_graph="; print_graph conc_sub_graph qs;
	  printf "\n"; flush stdout;
	  if not conc_sub_rs#is_satisfied then
	    begin
	      printf "** Retracting\n"; flush stdout;
	      Retract_string.retract_graph conc_sub_graph qs;
	      finished := false;
	    end
	  else
	    ();
	  premise_rs#next
      done;
      rule_rs#next;
  done;

done
