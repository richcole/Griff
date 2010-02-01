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
  for i = 1 to Pig.index_count qs.Quad_store.lex do
    let word = (Quad_store.word i qs) in
    let index = (Quad_store.index word qs)  in
      if not (index = i) then
	begin
	  printf "Failure: index=%d, i=%d, word=%s\n" index i word;
	  flush stdout;
	end
      else
	()
  done
;;

