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
open File 
open Rb_set

let square x = x *. x 

(* fold_left on f and z until eof occurs on the file FILE. *)

let rec until_eof_file f z file =
  if File.end_of_file file then 
    z 
  else 
    until_eof_file f (try f z file with File.Eof -> z) file

let until_eof_file_name f z file_name =
  let file = open_input_file file_name in
  let res = until_eof_file f z file in
    close_input_file file;
    res

let until_eof_channel f z file =
  let rec until_eof_channel_rec f (fin,z) file =
    if fin then z else
      until_eof_channel_rec f 
        (try (false,f z file) with End_of_file -> (true,z)) file
  in
    until_eof_channel_rec f (false,z) file

let words_of_channel_name name =
  let ch = open_in name in
  let process_line z ch =
    List.append z (Str.split (Str.regexp " +") (input_line ch))
  in
    until_eof_channel process_line [] ch

open File

let symbol_seperated sym rep s =
  Rb_set.fold_left 
    (fun x -> function "" -> rep x | p -> p ^ sym ^ (rep x))
    "" s

let symbol_seperated_list sym rep s =
  List.fold_right 
    (fun x -> function "" -> rep x | p -> (rep x) ^ sym ^ p )
    s ""

let string_of_int_tree x = symbol_seperated "," string_of_int x

(* get the index of the first occurence of sep in text return the end
   of the string if it was not found. 
*)
let rec find sep text i =
  let rec is_prefix s t si ti =
    if (String.length s) == si then
      true
    else if (String.length t) == ti then
      false
    else if (String.get s si) == (String.get t ti) then
      is_prefix s t (si+1) (ti+1)
    else
      false
  in
  let rec first c s si =
    if (String.length s) == si || (String.get s si) == c then
      si
    else
      first c s (si+1)
  in
  let j = first (String.get sep 0) text i in
    if j == (String.length text) || (is_prefix sep text 0 j) then
      j
    else
      find sep text (j+1)

let tr s c1 c2 = 
  let result = String.copy s in
    for i = 0 to (String.length result) - 1 do
      if (String.get result i) == c1 then
	String.set result i c2
      else ()
    done;
    result

(* split text based on seperator as a string between items. *)

let split sep text = 
  let rec remains sep text i accum =
    if i >= (String.length text) then
      accum
    else
      let j = find sep text i in
      let next_i = j + (String.length sep) in
	if j < (String.length text) then
	  remains sep text next_i (accum @ [ String.sub text i (j-i) ])
	else
	  accum @ [ String.sub text i (j-i) ]
  in
    remains sep text 0 []

let read_lexicon name = 
  let lex = new Lexicon.lexicon in
  let ch = (open_in_bin name) in
    lex#from_channel ch;
    close_in ch;
    lex

let write_lexicon name lex = 
  let ch = (open_out_bin name) in
    lex#to_channel ch;
    close_out ch


let read_matrix name =
  let matrix = new Smatrix.matrix in
  let ch = File.open_input_file name in
    matrix#from_file ch;
    close_input_file ch;
    matrix


let write_matrix name m =
  let ch = File.open_output_file name in
    m#to_file ch;
    close_output_file ch


let write_matrix_tree name m =
  let ch = File.open_output_file name in
  let print_value ((i,j),x) =
    output_float ch (float_of_int i);
    output_float ch (float_of_int j);
    output_float ch x;
  in
    output_int ch (Rb_set.count m);
    output_int ch 3;
    Rb_set.iter print_value m;
    close_output_file ch

let list_range lower upper =
  let rec remaining lower upper rest =
    if upper > lower then
      remaining lower (upper - 1) (upper :: rest)
    else
      rest
  in
    remaining lower upper []

let rec iter_pairs (f: 'a -> 'a -> unit) =
  function
    | [] -> ()
    | x :: t -> List.iter (f x) t; iter_pairs f t

let compose f g x = f (g x) 
let apply f x = f x 

let spaces n = String.make n ' '

let trim_left chars s = 
  let rec trim_left s i =
    if (String.length s) <= i then
      ""
    else
      if is_member (String.get s i) chars then
	trim_left s (i+1)
      else
	String.sub s i ((String.length s) - i)
  in
    trim_left s 0

let trim_right chars s = 
  let rec trim_right s i =
    if i <= 0 then
      ""
    else
      if is_member (String.get s (i-1)) chars then
	trim_right s (i-1)
      else
	String.sub s 0 i
  in
    trim_right s (String.length s)

let chars_of s = 
  let rec chars_of_agg i s z =
    if i < (String.length s) then
      chars_of_agg (i+1) s (insert (String.get s i) z)
    else z
  in
    chars_of_agg 0 s empty

let trim chrs s = 
  let chars = chars_of chrs in
    trim_left chars (trim_right chars s) 

let string_of_int_set s =
  let text x str = 
    if str = "" then
      string_of_int x
    else
      (string_of_int x) ^ ", " ^ str
  in
    Rb_set.fold_left text "" s

