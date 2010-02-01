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

exception LogicError 

type node = 
    Const of string
  | Var of string

type triple = node option * node * node * node

let rec last = function
  | v :: [] -> Some v
  | [] -> None
  | h :: t -> last t
;;
    
let first_object = function 
  | (c,s,p,o) :: t -> o
  | _ -> raise LogicError
;;

let last_subject list = 
  match last list with
    | Some (c,s,p,o) -> s
    | None -> raise LogicError
;;

let first_context = function
  | (c,s,p,o) :: t -> c
  | _ -> raise LogicError
;;

let propogate_context c = 
  List.map (function (None,s,p,o) -> (c,s,p,o) | x -> x)
;;

