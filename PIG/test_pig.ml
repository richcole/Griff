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

open Pig
open Printf;;

let x1 = make_tuple 1 1 1 1 
and x2 = make_tuple 1 0 0 0
and x3 = make_tuple 1 0 0 1
and x4 = make_tuple 1 4 0 1
and x5 = make_tuple 1 0 1 1
;;

exception TestFailed ;;

let print_tuple ch x = 
  Printf.fprintf ch "%d, %d, %d, %d" x.(0) x.(1) x.(2) x.(3)
;;

let p = create_tree_new "testing.tree" ;;

fprintf stderr "++ compact 0.\n"; flush stderr;
Gc.compact();

insert p x1;
let r1 = first_gte p x2 in
  assert (r1 = x1)
;;

fprintf stderr "++ compact 1.\n"; flush stderr;
Gc.compact()
;;

insert p x2 ;;
let r1 = first_gte p x2 in
  assert (r1 = x2);
  let r2 = succ p r1 in
    assert (r2 = x1)
;;

fprintf stderr "++ compact 2.\n"; flush stderr;
Gc.compact()
;;

let l = create_lex_new "testing.lex" ;;

fprintf stderr "++ compact 3.\n"; flush stderr;
Gc.compact()
;;

let x = index l "test" ;;
let y = index l "test" ;;
let z = index l "testing" ;;

fprintf stderr "++ compact 4.\n"; flush stderr;
Gc.compact()
;;

assert (x = y);
assert ((word l x) = "test");

fprintf stderr "++ compact 5.\n"; flush stderr;
Gc.compact()
;;

assert (not (x = z));
assert ((word l z) = "testing");

fprintf stderr "++ z=%s\n" (word l z);

fprintf stderr "++ compact 6.\n"; flush stderr;
Gc.compact();

assert ( (index_count l) == 2 );

assert ((word l (index l "#1")) = "#1");
assert ((word l (index l "#2")) = "#2");
assert ((word l (index l "#3")) = "#3");
assert ((word l (index l "#4")) = "#4");

for i = 1 to 100 do
  let uid = sprintf "#%d" i in
    printf "++ i=%d\n" i;
    assert ((word l (index l uid)) = uid)
done
