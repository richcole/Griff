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
open Mmatrix ;;
open File    ;;

let m = new matrix ;;
let n = new matrix ;;
let s = new matrix ;;

m#resize 2 5 ;;

for i = 1 to 2 do
  for j = 1 to 5 do
    m#set i j (float_of_int (((i - 1) * 5) + j))
  done
done
;;

prerr_endline "Writing Matrix...";

let ch = open_output_file "test.txt" in
  m#to_file ch;
  close_output_file ch;
;;

prerr_endline "Reading Matrix...";

let ch = open_input_file "test.txt" in 
  n#from_file ch;
  close_input_file ch;
;;

prerr_endline "Comparing Matrix...";

if not ((compare m n) = 0) then
  begin
    prerr_endline "Error in Comparison...";
    m#print stdout;
    n#print stdout;
  end
else ();

s#resize 2 5;

if (compare n s) = 0 then
  begin
    prerr_endline "Error in Comparison"
  end
    else ();

