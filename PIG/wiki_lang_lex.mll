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
  open Wiki_lang_parse
  ;;
}

let not_sep = 
  ( [^'\n'] ) | ('\n'[^'-']) | ("\n-"[^'-']) | ("\n--"[^'-'])

let not_sep_not_sq =
  ( [^'\n''['] ) | ('\n'[^'-''[']) | ("\n-"[^'-''[']) | ("\n--"[^'-''['])
  | ( '\n' (' '+ [^' ''[''-']+)+)

let not_sep_not_sq =
  [^'\n''[''-'] | ( ( '-'[^'-''[''\n'] )* ) | ("--"[^'-''[''\n'])*

rule next_token = parse
    '[' ( ( ['a'-'z''A'-'Z']+':' [^']']+ ) as pmm ) ']'
      { Wiki_token_ppm pmm }
  | "---" ' '* '\n' ( ((not_sep)*)
 as verb ) "\n---" ' '* '\n'
      { Wiki_token_verb ("\n" ^ verb ^ "\n") }
  | '$'( [^'$']* as eq) '$' 
      { Wiki_token_inline_eq eq }
  | '\n'' '*'\n' as para
      { Wiki_token_para_sep para }
  | "\\[" '\n'* ( ('\\'[^'[']|[^'\\'])* as eq ) '\n'* "\\]"
      { Wiki_token_display_eq eq }
  | "\\begin{equation}" ( _* as eq ) "\\end{equation}"
      { Wiki_token_display_eq eq }
  | "\\begin{eqnarray}" ( _* as eq ) "\\end{eqnarray}"
      { Wiki_token_display_eq eq }
  | ((not_sep_not_sq)+ '\n') as text
      { Wiki_token_text text }
  | "\n" as text
      { Wiki_token_text (String.make 1 text) }
  | eof { EOF }
