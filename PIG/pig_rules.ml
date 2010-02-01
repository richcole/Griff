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

open Quad_store
open Quad_graph
open Query_string
open Printf
open Rb_set

let subst_in_graph g rs qs =
  let tr = function
    | Var(u) -> 
	if (is_member u rs#vars) then
	  begin
(*	    printf "@@ subst u=%s -> %s\n" u (word (rs#var_index u) qs); *)

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

