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
  open Pmm_lang_ast ;;
%}

%token <int * string * string> LabelLine
%token <int * string>  TextLine
%token EOF
%start main             /* the entry point */
%type <Pmm_lang_ast.pmm_node list> node_list main

%%

main:
    node_list EOF        { $1 }
;

node_list:
    TextLine             { Pmm_lang_ast.insert_text  $1 [] }
  | LabelLine            { Pmm_lang_ast.insert_label $1 [] }
  | node_list TextLine   { Pmm_lang_ast.insert_text  $2 $1 }
  | node_list LabelLine  { Pmm_lang_ast.insert_label $2 $1 }
;

