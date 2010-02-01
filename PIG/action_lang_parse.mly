/* 
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
*/
%{
  open Action_lang_ast ;;
%}

%token <string> STRING IDENT
%token FOR STRING DO DONE ASSERT RETRACT PRINT PRINTLN EOF
%start main             /* the entry point */
%type <Action_lang_ast.stmt> stmt
%type <Action_lang_ast.stmt list> stmt_list main

%%

main:
    stmt_list EOF        { $1 }
;

stmt_list:
    stmt                 { [$1] }
  | stmt_list stmt       { List.append $1 [$2] }
;

stmt:
  | FOR STRING DO stmt_list DONE { Action_lang_ast.For    ($2, $4) }
  | ASSERT STRING                { Action_lang_ast.Assert $2       }
  | RETRACT STRING                { Action_lang_ast.Retract $2       }
  | PRINT STRING                 { Action_lang_ast.Print  $2       }
  | PRINTLN STRING                 { Action_lang_ast.PrintLn  $2       }
;


