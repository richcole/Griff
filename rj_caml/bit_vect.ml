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
type bit_vect

external zeros: int -> bit_vect = "ml_bit_vect_new_with_bits"
external set: bit_vect -> int -> bit_vect = "ml_bit_vect_set"
external clear: bit_vect -> int -> bit_vect = "ml_bit_vect_clear"
external get: bit_vect -> int -> bool = "ml_bit_vect_get"
external bit_and: bit_vect -> bit_vect -> bit_vect = "ml_bit_vect_and"
external bit_or: bit_vect -> bit_vect -> bit_vect = "ml_bit_vect_or"
external bit_minus: bit_vect -> bit_vect -> bit_vect = "ml_bit_vect_minus"
external length: bit_vect -> int = "ml_bit_vect_length"

