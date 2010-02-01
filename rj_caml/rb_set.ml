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

open Printf

type color    = R | B
type 'a tree_node  = E | T of color * 'a tree_node * 'a * 'a tree_node

type ('a,'b) non_default_tree = {
  cmp:  'b -> 'b -> int;
  root: 'a tree_node
}

type 'a       tree     = ('a,'a) non_default_tree
type ('a, 'b) map_tree = ('a*'b,'a) non_default_tree

exception LogicError
exception NotFound

let some = function 
    None -> raise NotFound 
  | Some(x) -> x

let some_default z = function
    None -> z
  | Some(x) -> x

let empty     = { cmp=Pervasives.compare; root=E }
let empty_map : ('a, 'b) map_tree = { cmp=Pervasives.compare; root=E }
let empty_with (cmp_func: 'a -> 'a -> int) : ('a, 'a) non_default_tree = { cmp=cmp_func; root=E }
let empty_map_with (cmp_func: 'a -> 'a -> int) : ('a, 'b) map_tree = { cmp=cmp_func; root=E }

let map_of_set (s: ('a * 'b) tree) : ('a, 'b) map_tree = 
  { s with cmp=Pervasives.compare }

let rec is_member_cmp compare x = function
  | E -> false
  | T (_, a, y, b) ->
      let lt = compare x y in
        if lt < 0 then is_member_cmp compare x a
        else if lt > 0 then is_member_cmp compare x b
        else true

let is_member x tr = is_member_cmp tr.cmp x tr.root

let balance = function
  | B, T (R, T (R, a, x, b), y, c), z, d
  | B, T (R, a, x, T (R, b, y, c)), z, d
  | B, a, x, T (R, T (R, b, y, c), z, d)
  | B, a, x, T (R, b, y, T (R, c, z, d)) ->
      T (R, T (B, a, x, b), y, T (B, c, z, d))
  | a, b, c, d -> T (a, b, c, d)

let insert_cmp compare x s =
  let rec ins = function
    | E -> T (R, E, x, E)
    | T (color, a, y, b) as s ->
	let lt = compare x y in
          if lt < 0 then 
	    balance (color, ins a, y, b)
          else if lt > 0 then 
	    balance (color, a, y, ins b)
          else s 
  in
    match ins s with  (* guaranteed to be non-empty *)
      | T (_, a, y, b) -> T (B, a, y, b)
      | _ -> raise LogicError

let insert x tr = { tr with root=insert_cmp tr.cmp x tr.root }

let insert_mapping_cmp compare x y s =
  let rec ins = function
    | E -> T (R, E, (x,y), E)
    | T (color, a, (u,v), b) ->
	let lt = compare x u in
	  if lt < 0 then
	    balance (color, ins a, (u,v), b)
	  else if lt > 0 then
	    balance (color, a, (u,v), ins b)
	  else
	    T(color, a, (x,y), b)
  in
    match ins s with
      | T (_, a, y, b) -> T (B, a, y, b)
      | _ -> raise LogicError

let insert_mapping x y tr = 
  { tr with root=insert_mapping_cmp tr.cmp x y tr.root }

let rec map_mapping_cmp f =
  function
    | E -> E
    | T(color, l, (u,v), r) ->
	T(color, map_mapping_cmp f l, (u, f (u,v)), map_mapping_cmp f r)

let rec map_mapping f tr = { tr with root=map_mapping_cmp f tr.root }

let transform_mapping_cmp compare x f z s = 
  let rec ins = function
    | E -> T (R, E, (x,f z), E)
    | T (color, a, (u,v), b) ->
	let lt = compare x u in
	  if lt < 0 then
	    balance (color, ins a, (u,v), b)
	  else if lt > 0 then
	    balance (color, a, (u,v), ins b)
	  else
	    T(color, a, (x,f v), b)
  in
    match ins s with
      | T (_, a, y, b) -> T (B, a, y, b)
      | _ -> raise LogicError

let transform_mapping x f z s =
  { s with root=transform_mapping_cmp s.cmp x f z s.root }

let rec image_cmp cmp x = function
  | E -> None
  | T (_, a, (u,v), b) ->
      let lt = cmp x u in
        if lt < 0 then image_cmp cmp x a
        else if lt > 0 then image_cmp cmp x b
        else Some(v)

let image x tr = image_cmp tr.cmp x tr.root

let rec incr x y z s =
  let tr a = a +. y in
    transform_mapping x tr z s

let rec incr_int x y z s =
  let tr a = a + y in
    transform_mapping x tr z s

let rec iter_cmp f = function
  | E -> ()
  | T(color, l, v, r) -> 
      begin
	iter_cmp f l;
	f v; 
	iter_cmp f r;
      end

let iter f tr = iter_cmp f tr.root

let rec iter_pred f pred tr = 
  let g x =
    if pred x then
      f x
    else
      ()
  in
    iter g tr

let rec fold_cmp f g z = function 
  | E -> z
  | T(color, l, v, r) -> 
      begin
	f (fold_cmp f g z l) (g v (fold_cmp f g z r));
      end

let fold f g z tr = fold_cmp f g z tr.root

let rec fold_left_cmp f z = function
  | E -> z
  | T(c, l, v, r) ->
      fold_left_cmp f (f v (fold_left_cmp f z r)) l

let fold_left f z tr = fold_left_cmp f z tr.root

let rec fold_right_cmp f z = function
  | E -> z
  | T(c, l, v, r) ->
      let lz = fold_right_cmp f z l in
      let rz = fold_right_cmp f (f lz v) r in
        rz

let fold_right f z tr = fold_right_cmp f z tr.root

let list_of s = 
  let 
    concat x y = x @ y and
    append x y = x :: y 
  in
    fold concat append [] s

let from_list x =
  List.fold_right insert x empty 

let map_from_list x = 
  List.fold_right (fun (u,v) m -> insert_mapping u v m) x empty_map

let list_of_tr f s = 
  let compose v r = (f v) :: r in
    fold (@) compose [] s

let length s =
  let 
    plus_one v r = 1 + r and
    plus l r = l + r 
  in
    fold plus plus_one 0 s

let count s = length s

let is_empty tr = match tr.root with 
  | E -> true
  | _ -> false

let lbalance x1 x2 x3 = match x1, x2, x3 with
  | T(R, T(R, a,x,b), y, c), z, d -> 
      T(R, T(B, a,x,b), y, T(B, c,z,d))
  | T(R, a, x, T(R, b,y,c)), z, d ->
      T(R, T(B, a,x,b), y, T(B, c,z,d))
  | a,x,b -> 
      T(B, a,x,b)

let rbalance x1 x2 x3 = match x1, x2, x3 with
  | a, x, T(R, T(R, b,y,c), z, d) ->
      T(R, T(B, a,x,b), y, T(B, c,z,d))
  | a, x, T(R, b, y, T(R, c,z,d)) ->
      T(R, T(B, a,x,b), y, T(B, c,z,d))
  | a,x,b -> 
      T(B, a,x,b)

(* [unbalanced_left] repares invariant (2) when the black height of the 
   left son exceeds (by 1) the black height of the right son *)

let unbalanced_left = function
  | T(R, T(B, t1, x1, t2), x2, t3) ->
      lbalance (T(R, t1, x1, t2)) x2 t3, false
  | T(B, T(B, t1, x1, t2), x2, t3) ->
      lbalance (T(R, t1, x1, t2)) x2 t3, true
  | T(B, T(R, t1, x1, T(B, t2, x2, t3)), x3, t4) ->
      T(B, t1, x1, lbalance (T(R, t2, x2, t3)) x3 t4), false
  | _ ->
      assert false

(* [unbalanced_right] repares invariant (2) when the black height of the 
   right son exceeds (by 1) the black height of the left son *)

let unbalanced_right = function
  | T(R, t1, x1, T(B, t2, x2, t3)) ->
      rbalance t1 x1 (T(R, t2, x2, t3)), false
  | T(B, t1, x1, T(B, t2, x2, t3)) ->
      rbalance t1 x1 (T(R, t2, x2, t3)), true
  | T(B, t1, x1, T(R, T(B, t2, x2, t3), x3, t4)) ->
      T(B, rbalance t1 x1 (T(R, t2, x2, t3)), x3, t4), false
  | _ ->
      assert false
	
(* [remove_min s = (s',m,b)] extracts the minimum [m] of [s], [s'] being the
   resulting set, and indicates with [b] whether the black height has
   decreased *)

let rec remove_min_cmp compare = function
  | E -> 
      assert false
	(* minimum is reached *)
  | T(B, E, x, E) -> 
      E, x, true
  | T(B, E, x, T(R, l, y, r)) ->
      T(B, l, y, r), x, false
  | T(B, E, _, T(B, _, _, _)) ->
      assert false
  | T(R, E, x, r) ->
      r, x, false
	(* minimum is recursively extracted from [l] *)
  | T(B, l, x, r) ->
      let l',m,d = remove_min_cmp compare l in
      let t = T(B, l', x, r) in
	if d then
	  let t,d' = unbalanced_right t in t,m,d'
	else
	  t, m, false
  | T(R, l, x, r) ->
      let l',m,d = remove_min_cmp compare l in
      let t = T(R, l', x, r) in
	if d then
	  let t,d' = unbalanced_right t in t,m,d'
	else
	  t, m, false

let blackify = function
  | T(R, l, x, r) -> T(B, l, x, r), false
  | s -> s, true

(* [remove_aux x s = (s',b)] removes [x] from [s] and indicates with [b] 
   whether the black height has decreased *)

let remove_cmp compare x s =
  let rec remove_aux = function
    | E ->
	E, false
    | T(B, l, y, r) ->
	let c = compare x y in
	  if c < 0 then
	    let l',d = remove_aux l in
	    let t = T(B, l', y, r) in
	      if d then unbalanced_right t else t, false
	  else if c > 0 then
	    let r',d = remove_aux r in
	    let t = T(B, l, y, r') in
	      if d then unbalanced_left t else t, false
	  else (* x = y *) 
	    (match r with
	       | E -> 
		   blackify l
	       | _ ->
		   let r',m,d = remove_min_cmp compare r in
		   let t = T(B, l, m, r') in
		     if d then unbalanced_left t else t, false)
    | T(R, l, y, r) ->
	let c = compare x y in
	  if c < 0 then
	    let l',d = remove_aux l in
	    let t = T(R, l', y, r) in
	      if d then unbalanced_right t else t, false
	  else if c > 0 then
	    let r',d = remove_aux r in
	    let t = T(R, l, y, r') in
	      if d then unbalanced_left t else t, false
	  else (* x = y *)
	    (match r with
	       | E -> 
		   l, false
	       | _ ->
		   let r',m,d = remove_min_cmp compare r in
		   let t = T(R, l, m, r') in
		     if d then unbalanced_left t else t, false)
  in
  let s',_ = remove_aux s in s'

let remove x s = { s with root=remove_cmp s.cmp x s.root }

let rec first_that_cmp pred = function
    E -> None
  | T(color, l, v, r) ->
      match first_that_cmp pred l with
	  Some(v) -> Some(v)
	| None -> 
	    if pred v then 
	      Some(v) 
	    else 
	      first_that_cmp pred r

let first_that pred tr = first_that_cmp pred tr.root

let rec last_that_cmp pred = function
    E -> None
  | T(color, l, v, r) ->
      match last_that_cmp pred r with
	  Some(v) -> Some(v)
	| None -> 
	    if pred v then 
	      Some(v) 
	    else 
	      last_that_cmp pred l

let last_that pred tr = last_that_cmp pred tr.root

let rec remove_if pred tr =
  match first_that pred tr with
      Some(v) -> remove_if pred (remove v tr)
    | None -> tr

let dot_plus_i i tr =
  let pred x = (tr.cmp x i) < 0 in
    insert i (remove_if pred tr)

let rec filter pred tr =
  let not_pred x = not (pred x) in
    remove_if not_pred tr

let minus s t =
  let pred x = (is_member x t) in
    remove_if pred s

let intersection s t =
  let pred x = not (is_member x t) in
    remove_if pred s

let union s t =
  let r = ref s in
    iter (fun x -> r := insert x !r) t;
    !r

let is_subseteq a b =
  match first_that (fun x -> not (is_member x b)) a with
      Some(x) -> 
	false
    | None -> 
	true

let rec iter_in_range_cmp compare s lower upper f = match s with
  | E -> ()
  | T (c, l, v, r) ->
      if (compare lower v) <= 0 then
	iter_in_range_cmp compare l lower upper f
      else ();
      if (compare lower v) <= 0 && (compare v upper) <= 0 then
	f v
      else ();
      if (compare v upper) <= 0 then
	iter_in_range_cmp compare r lower upper f
      else ()

let iter_in_range tr lower upper f =
  iter_in_range_cmp tr.cmp tr.root lower upper f 

let fold_left_in_range_cmp compare lower upper f z s =
  let rec fold z = function
    | E -> z
    | T(c, l, v, r) ->
	if (compare v lower) < 0 then
	  fold z r
	else if (compare v upper) > 0 then
	  fold z l
	else
	  fold (f v (fold z r)) l
  in
    fold z s

let fold_left_in_range lower upper f z s =
  fold_left_in_range_cmp s.cmp lower upper f z s.root

let domain_cmp cmp (b,c) a = cmp b a

let fold_left_map_in_range lower upper f z s =
  fold_left_in_range_cmp (domain_cmp s.cmp) lower upper f z s.root

let rec iter_mapping_in_range_cmp compare s lower upper f = 
  match s with
    | E -> ()
    | T (c, l, (v,fv), r) ->
        if (compare lower v) <= 0 then
	  iter_mapping_in_range_cmp compare l lower upper f
        else ();
        if (compare lower v) <= 0 && (compare v upper) <= 0 then
	  f v fv
        else ();
        if (compare v upper) <= 0 then
	  iter_mapping_in_range_cmp compare r lower upper f
        else ()

let iter_mapping_in_range s lower upper f = 
  iter_mapping_in_range_cmp s.cmp s.root lower upper f

let is_domain_member x t =
  match image x t with
    | Some(v) -> true
    | None -> false

let domain_of tr =
  fold_left (fun (d,r) s -> insert d s) empty tr

let succ v tr =
  first_that (fun x -> (tr.cmp x v) > 0) tr

let map_succ v tr =
  first_that (fun (dx,vx) -> (tr.cmp dx v) > 0) tr

let prev v tr =
  last_that (fun x -> (tr.cmp x v) < 0) tr

let first t =
  first_that (fun x -> true) t

let last t =
  last_that (fun x -> true) t


let symbol_seperated sym rep s =
  fold_left 
    (fun x -> function "" -> rep x | p -> p ^ sym ^ (rep x))
    "" s

let string_of_int_tree x = symbol_seperated "," string_of_int x

let first_not_in t2 t1 =
  first_that (fun x -> not (is_member x t2)) t1

let tree_compare (t1: 'a tree) (t2: 'a tree) =
(*  fprintf stderr "Compare: t1=%s, t2=%s" (string_of_int_tree t1) (string_of_int_tree t2); *)
  let result = 
    match first_not_in t2 t1, first_not_in t1 t2 with
      | Some(x), Some(y) -> - (t1.cmp x y)
      | Some(x), None -> 1
      | None, Some(y) -> -1
      | None, None -> 0
  in
(*    fprintf stderr ", result=%d\n" result; *)
    result
     

let create_empty_tree_tree () = empty_with tree_compare
let create_empty_map_tree () = empty_map_with tree_compare

exception NotFound

let some = function
  | None -> raise NotFound
  | Some(x) -> x

class ['a,'b] tes = 
object(self)
  val mutable tree : ('a,'b) non_default_tree = empty

  method set_tree (x: ('a,'b) non_default_tree) =
    tree <- x

  method count =
    count tree

  method tree = tree

  method iter (f:('a -> unit)) =
    iter f tree

  method fold_left : 'b. ('a -> 'b -> 'b) -> 'b -> 'b 
    = fun f z -> 
      fold_left f z tree

  method to_channel ch =
    output_value ch tree.root

  method from_channel ch =
    tree <- { cmp=Pervasives.compare; root=input_value ch }

end 

class ['a] set =
object(self)
  inherit ['a, 'a] tes

  method remove (x:'a) =
    tree <- remove x tree

  method first_that (pred: 'a -> bool) =
    first_that pred tree

  method first =
    first_that (fun x -> true) tree

  method last_that (pred: 'a -> bool) =
    last_that pred tree

  method last =
    last_that (fun x -> true) tree

  method succ (v:'a) =
    first_that (fun x -> (tree.cmp v x) > 0) tree

  method union (s: 'a tree) = 
    tree <- union tree s

  method intersection s = 
    tree <- intersection tree s

  method first_not_in (a: 'a set) =
    let pred x = not (a#is_member x) in
      self#first_that pred

  method last_not_in (a: 'a set) =
    let pred x = not (a#is_member x) in
      self#last_that pred

  method succ_not_in (a: 'a set) (i: 'a) =
    let pred x = (not (a#is_member x)) && (tree.cmp x i > 0) in
      self#first_that pred

  method is_member (x:'a) = 
    is_member x tree

  method filter (pred:'a -> bool) =
    tree <- filter pred tree

  method insert (x:'a) =
    tree <- insert x tree

  method copy =
    let cp : 'a set = new set in
      self#iter (fun x -> cp#insert x);
      cp

  method remove_if (pred:'a -> bool) =
    tree <- remove_if pred tree

end

let make_set (x: ('a,'b) non_default_tree) =
  let s: 'a set = new set in
    s#set_tree x; 
    s

class ['a, 'b] map =
object(self) 
  inherit ['a * 'b, 'a] tes
    
  method remove (x:'a * 'b) =
    let new_cmp : ('a*'b) -> ('a*'b) -> int = 
      fun (a,b) (c,d) -> tree.cmp a c 
    in
    let nt : ('a*'b,'a*'b) non_default_tree = 
      { cmp=new_cmp; root=tree.root } 
    in
      tree <- {tree with root=(remove x nt).root }

  method insert_mapping (x:'a) (y:'b) =
    tree <- insert_mapping x y tree

  method  image (x:'a) =
    image x tree 

  method transform_mapping (x:'a) (f:('b -> 'b)) (z:'b) =
    tree <- transform_mapping x f z tree

  method is_domain_member (x: 'a) =
    match self#image x with
	Some(x) -> true
      | None -> false

  method remove_if (pred:('a*'b) -> bool) =
    let new_cmp : ('a*'b) -> ('a*'b) -> int = 
      fun (a,b) (c,d) -> tree.cmp a c 
    in
    let nt : ('a*'b,'a*'b) non_default_tree = { cmp=new_cmp; root=tree.root } in
      tree <- {tree with root=(remove_if pred nt).root }

end

let make_map (x: ('a * 'b, 'a) non_default_tree) =
  let m = new map in
    m#set_tree x;
    m


let incr_float_mapping m x v =
  let plus_v a = a +. v in
    m#transform_mapping x plus_v 0.0


class ['a] partition =
object(self)

  val mutable count     = 0
  val color     : ('a, int) map = new map
  val merge_map : (int,int) map = new map

  method tail (x:'a) =
    let rec tl y =
      match merge_map#image y with
	| Some(y) -> tl y
	| None -> y
    in
      match color#image x with
	| Some(y) -> 
	    tl y
	| None -> 
	    count <- count + 1;
	    color#insert_mapping x count;
	    count;

  method merge (x:'a) (y:'a) =
    let tx = self#tail x and ty = self#tail y in
	if tx == ty then () else
	  merge_map#insert_mapping tx ty;

  method color x = self#tail x
end

let iter_pairs f t =
  iter 
    (fun x ->
       let pred y = x < y in
       let func y = f x y in
	 iter_pred func pred t
    )
    t

let trace_tree rep ch tr =
  let f s t = 
    if (String.length s) == 0 then
      t 
    else if (String.length t) == 0 then
      s
    else
      s ^ ", " ^ t
  in
  let g v t = 
    if (String.length t) == 0 then
      (rep v) ^ " "
    else 
      (rep v) ^ ", " ^ t
  in
    output_string ch (fold f g "" tr)


