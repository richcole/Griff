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

type 'a symbol = Symbol of 'a | End
type 'a trie = T of ('a symbol, 'a trie) Rb_set.map_tree

let empty : 'a trie = 
  T { Rb_set.cmp=Pervasives.compare; Rb_set.root=Rb_set.E }

let rec insert (xs: 'a list) (t: 'a trie) : 'a trie =
    match t, xs with
      | T(t), [] -> 
          let e : 'a trie = empty in
            T (Rb_set.insert_mapping End e t)
      | T(t), s::st ->
          T (Rb_set.transform_mapping (Symbol s) (insert st) empty t)

let rec prefix_list 
  (xs: 'a list) (path: 'a list) (z: 'a list list) (t: 'a trie) 
  : 'a list list 
  =
  let app_z path z = 
    match path with 
      | [] -> z
      | _ -> path :: z
  in
    match t, xs with
      | T(t), [] -> 
          app_z path z
      | T(t), s::st ->
          begin
            match Rb_set.image (Symbol s) t with
              | Some(next_t) ->
                  if (Rb_set.count t) = 1 then
                    prefix_list st (s :: path) z next_t
                  else
                    prefix_list st (s :: path) (app_z path z) next_t
              | None ->
                  (path :: z)
          end

let rec fold_left_cmp 
  (f: 'a list -> 'b -> 'b)
  ((path: 'a list),(z: 'b)) 
  (tr: ('a symbol * 'a trie) Rb_set.tree_node)
  = 
  match tr with
    | Rb_set.E -> z
    | Rb_set.T(color, l, (Symbol(s),T(c)), r) ->
        let np : 'a list = s :: path in
        let right_z : 'b = fold_left_cmp f (path, z) r in
        let curr_z : 'b = fold_left_cmp f (np, right_z) c.Rb_set.root in
        let left_z : 'b = fold_left_cmp f (path, curr_z) l in
          left_z
    | Rb_set.T(color, l, (End,c), r) ->
        f path z

let fold_left 
  (f: 'a list -> 'b -> 'b) 
  (z: 'b) 
  (tr: 'a trie) 
  = 
  match tr with
    | T(tr) -> fold_left_cmp f ([],z) tr.Rb_set.root




