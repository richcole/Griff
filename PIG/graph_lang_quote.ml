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

  let dequote s =
    let r = String.copy s in
    let l = String.length s in
    let k = ref 0 in
    let i = ref 0 in
      while !i < l-1 do
	if ( (String.get s !i) = '\\' && (String.get s (!i+1)) == '"' ) then
	  begin
	    String.set r !k (String.get s (!i+1));
	    i := !i + 2;
	    k := !k + 1;
	  end
	else
	  begin
	    String.set r !k (String.get s !i);
	    i := !i + 1;
	    k := !k + 1;
	  end
      done;
      String.sub r 0 !k
  ;;

  let quote s =
    let rec count s i =
      try 
	1 + (count s (1+(String.index_from s i '"')))
      with 
	| Not_found -> 0
    in
    let l = ((String.length s) + (count s 0)) in
    let r = String.make l ' ' in
    let k = ref 0 in
    let i = ref 0 in
      while !i < (String.length s) do
	if (String.get s !i) = '"' then
	  begin
	    String.set r !k '\\';
	    String.set r (!k+1) '"';
	    k := !k + 2;
	    i := !i + 1;
	  end
	else
	  begin
	    String.set r !k (String.get s !i);
	    k := !k + 1;
	    i := !i + 1;
	  end
      done;
      r
  ;;
