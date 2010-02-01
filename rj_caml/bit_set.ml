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
open Bit_vect
open Lexicon

type 'a bit_set = {
  lex: 'a lex;
  vec: bit_vect;
}

let insert s x = 
  let ix = index s.lex x in
    { lex=s.lex; vec=set s.vec ix }
;;

let remove s x =
  let ix = index s.lex x in
    { lex=s.lex; vec=clear s.vec ix }
;;

let is_member s x =
  let ix = index s.lex x in
    get s.vec ix
;;

let empty lex =
  { lex=lex; vec=zeros 1; }

let singleton_with_lex lex x =
  insert (empty lex) x

let fold_left s f z =
  let res = ref z in
    for i = 1 to min (length s.vec) (Lexicon.count s.lex) do
      if get s.vec i then
	res := f (word s.lex i) !res
      else ()
    done;
    !res
;;

let intersection s t =
  if s.lex == t.lex then
    { lex=s.lex; vec=bit_and s.vec t.vec }
  else
    let rem x r = 
      if not (is_member t x) then 
	remove r x 
      else r
    in
      fold_left s rem s
;;

let union s t =
  if s.lex == t.lex then
    { lex=s.lex; vec=bit_or s.vec t.vec }
  else
    let ins x s = insert s x in
      fold_left t ins s
;;

let minus s t =
  if s.lex == t.lex then
    { lex=s.lex; vec=bit_minus s.vec t.vec }
  else
    let rem x r = 
      if is_member t x then 
	remove r x 
      else
	r
    in
      fold_left s rem s
;;


