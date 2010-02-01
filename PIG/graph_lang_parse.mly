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
  open Graph_lang_ast ;;
%}

%token <string> VAR CONST
%token PERIOD COMMA EOF DASH SEMI
%start main             /* the entry point */
%type <Graph_lang_ast.triple> triple
%type <Graph_lang_ast.triple list> graph triple_list triple_seq 
%type <Graph_lang_ast.triple list list> main graph_list
%type <Graph_lang_ast.context>

%%

main:
    graph_list EOF        { $1 }
;

graph_list:
    graph                 { [ $1 ] }
  | graph_list graph      { (List.append $1 ([$2])) }
;

graph:
    triple_list PERIOD             { $1 }
;

triple:
    node node node               { (None, $1, $2, $3) }
  | CONST DASH node node node    { (Some (Const $1), $3, $4, $5) }
;

triple_seq:
    triple                       { [$1] }
  | triple_seq node node         
      { 
	(first_context $1, first_object $1, $2, $3) :: $1 
      }
  | triple_seq SEMI node node    
      { 
	(first_context $1, last_subject $1, $3, $4) :: $1 
      }
;

triple_list:
    triple_seq                   { $1 }
  | triple_list COMMA triple_seq 
      { 
	List.append $1 (propogate_context (first_context $1) $3)
      }
;

node:
    VAR                     { Var $1 }
  | CONST                   { Const $1 }
;

