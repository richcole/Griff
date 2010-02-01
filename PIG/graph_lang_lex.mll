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
{
  open Graph_lang_parse ;;
  open Graph_lang_quote ;;
}

rule token = parse
  [' ''\t''\n']+     
      { token lexbuf }     
  | ( '"' ( ( ( '\\' '"' ) | ([ ^ '"' ]) ) * as ident ) '"' )
      {  
	CONST ident
      }
  | '_' [^' ''\t''\n'',''.'';']+ as ident
      {  VAR ident }
  | [^' ''\t''\n'',''.''-'';'][^' ''\t''\n'',''.'';']* as ident
      {  CONST ident }
  | "--"           { DASH }
  | '.'            { PERIOD }
  | ','            { COMMA }
  | ';'            { SEMI }
  | eof            { EOF }
