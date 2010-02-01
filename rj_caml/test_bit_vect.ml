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
open Printf

let print_set ch bs = 
  for i = 1 to (length bs) do
    output_string ch 
      (if get bs i then
	"1"
      else
	"0")
  done

let x = zeros 1
;;

printf "x=";
print_set stdout x;
printf "\n";

let y = set x 5 in
  printf "y=";
  print_set stdout y;
  printf "\n";
;;

let y = set x 10 in
  printf "y=";
  print_set stdout y;
  printf "\n";
;;

let y = set x 23 in
  printf "y=";
  print_set stdout y;
  printf "\n";
;;

let y = set x 64 in
  printf "y=";
  print_set stdout y;
  printf "\n";
;;

let y = set x 64 in
let z = set x 32 in
  printf "y&z=";
  print_set stdout (bit_and y z);
  printf "\n";
  printf "y|z=";
  print_set stdout (bit_or y z);
  printf "\n";
  printf "y-z=";
  print_set stdout (bit_minus y z);
  printf "\n";

  printf "z&y=";
  print_set stdout (bit_and z y);
  printf "\n";
  printf "z|y=";
  print_set stdout (bit_or z y);
  printf "\n";
  printf "z-y=";
  print_set stdout (bit_minus z y);
  printf "\n";
;;


  


