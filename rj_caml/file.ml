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
type input_file
type output_file

external open_input_file : string -> input_file = "open_input_file"
external open_output_file : string -> output_file = "open_output_file"

external input_int : input_file -> int = "input_int"
external input_float : input_file -> float = "input_float"

external output_int : output_file -> int -> unit = "output_int"
external output_float : output_file -> float -> unit = "output_float"

external close_output_file : output_file -> unit = "close_output_file"
external close_input_file : input_file -> unit = "close_input_file"

external end_of_file : input_file -> bool = "end_of_file"
external tell_percent : input_file -> float = "tell_percent"

exception Eof

let _ = Callback.register_exception "File.Eof" Eof
