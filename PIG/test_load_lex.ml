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
let read_word z ch =
  let line = input_line ch in
  let index = Quad_store.index line qs in
  let word = Quad_store.word index qs in
    if line <> word then
      (fprintf stdout "Error: '%s' != '%s'\n" line word; flush stdout) 
    else 
      (fprintf stdout "Ok: %s\n" line);
    assert (line = word);
    ()
in
let ch = open_in Sys.argv.(2) in
  Usual.until_eof_channel read_word () ch;
  close_in ch;
;;
