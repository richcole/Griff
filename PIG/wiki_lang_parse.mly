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

%token <string> Wiki_token_ppm 
%token <string> Wiki_token_verb
%token <string> Wiki_token_inline_eq
%token <string> Wiki_token_para_sep
%token <string> Wiki_token_display_eq
%token <string> Wiki_token_text
%token EOF
%start main             /* the entry point */
%type <Pmm_lang_ast.pmm_node list> node_list main

%%

main:
    node_list EOF        { $1 }
;

node_list:
    Wiki_token_ppm        { Pmm_lang.pmm_of_string $1 }
  | Wiki_token_verb       { [Pmm_lang.text_node_of  2 "verbatim" $1] } 
  | Wiki_token_inline_eq  { [Pmm_lang.text_node_of  2 "inline eq" $1] } 
  | Wiki_token_display_eq { [Pmm_lang.text_node_of  2 "display eq" $1] } 
  | Wiki_token_para_sep   { [Pmm_lang.child_node_of 0 "para"] } 
  | Wiki_token_text       { [Pmm_lang.text_node_of  2 "text" $1] }
  | node_list Wiki_token_ppm { 
      let insert n = 
        Pmm_lang_ast.insert_node { n with indent=n.indent+1 } in
      List.fold_right insert (Pmm_lang.pmm_of_string $2) $1
    }
  | node_list Wiki_token_verb { 
      Pmm_lang_ast.insert_node (Pmm_lang.text_node_of 2 "verbatim" $2) $1
    }
  | node_list Wiki_token_inline_eq { 
      Pmm_lang_ast.insert_node (Pmm_lang.text_node_of 2 "inline eq" $2) $1
    }
  | node_list Wiki_token_display_eq { 
      Pmm_lang_ast.insert_node (Pmm_lang.text_node_of 2 "display eq" $2) $1
    }
  | node_list Wiki_token_para_sep { 
      Pmm_lang_ast.insert_node (Pmm_lang.child_node_of 0 "para") $1
    }
  | node_list Wiki_token_text { 
      Pmm_lang_ast.insert_node (Pmm_lang.text_node_of 2 "text" $2) $1
    }
;

