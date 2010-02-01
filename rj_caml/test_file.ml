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
;;

let range s e =
  let rec range_rec s e l =
    if (compare s e) < 0 then
      range_rec (s + 1) e (l @ [s])
    else
      l @ [e]
  in
    range_rec s e []
;;

let list_of_func f =
  let rec list_of_func_rec f l =
    try
      list_of_func_rec f (l @ [f ()])
    with
      | _ -> l
  in
    list_of_func_rec f []
;;

  
let l1 = List.map float_of_int (range 1 10) in

let o = open_output_file "test.txt" in
  List.iter (output_float o) l1; 
  close_output_file o;


let i = open_input_file "test.txt" in
let l2 = list_of_func (fun x -> (input_float i)) in
  if not (l1 = l2) then
    begin
      Printf.printf "Error, lists don't match\n";
      Printf.printf "l1="; List.iter (fun x -> Printf.printf "%f " x) l1;
      Printf.printf ", l2="; List.iter (fun x -> Printf.printf "%f " x) l2;
      Printf.printf "\n"
    end
  else
    ()
  ;
  close_input_file i




