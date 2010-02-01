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
open Rb_set ;;

exception NotFound

let remove_mapping x m n =
  begin
    match (m#image x) with
      | Some(fx) -> 
	  n#remove (fx,x);
	  m#remove (x,fx);
      | _ -> ()
  end;

type 'a lex_body = { 
  mutable wm : ('a, int) map;
  mutable im : (int, 'a) map;
  mutable count: int
}

type 'a lex =
  Empty | Lexicon of 'a lex_body

let empty = Empty

let index lex w =
  let get_index lex =
    match (lex.wm#image w) with
      | Some(x) -> x
      | None ->
	  lex.count <- lex.count + 1;
	  lex.wm#insert_mapping w lex.count;
	  lex.im#insert_mapping lex.count w;
	  lex.count
  in
    match lex with
      | Empty ->
	  let new_lex = {wm=new map; im=new map; count=0} in
	    get_index new_lex;
      | Lexicon(lex) ->
	  get_index lex
;;

let word lex i =
  match lex with
    | Empty -> raise NotFound
    | Lexicon(lex) ->
	some (lex.im#image i)
;;

let count lex = 
  match lex with
    | Empty -> 0
    | Lexicon(lex) -> lex.count
;;

class ['a] lexicon =
  object
    val wm : ('a, int) map = new map
    val im : (int, 'a) map = new map
    val mutable count = 0

    method index w =
      match (wm#image w) with
	| Some(x) -> x
	| None ->
	    count <- count + 1;
            wm#insert_mapping w count;
            im#insert_mapping count w;
            count

    method word i = some (im#image i)

    method index_iter (f : (int * 'a) -> unit) =
      im#iter f

    method word_iter (f : ('a * 'int) -> unit) =
      wm#iter f

    method index_fold_left : 'b. ((int * 'a) -> 'b -> 'b) -> 'b -> 'b =
      fun f z ->
        im#fold_left f z

    method replace word index =
      remove_mapping word wm im;
      remove_mapping index im wm;
      wm#insert_mapping word index;
      im#insert_mapping index word;

    method count =
      count

    method is_word_member w =
      match wm#image w with
	  Some(index) -> true
	| None -> false

    method to_channel ch =
      wm#to_channel ch;
      im#to_channel ch;
      output_value ch count;

    method from_channel ch =
      wm#from_channel ch;
      im#from_channel ch;
      count <- input_value ch;

end ;;

let a : string lex = empty ;;
let b : int lex = empty ;;

ignore (index a "that") ;;
ignore (index b 5) ;;
