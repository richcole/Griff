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

open Wiki_lang
open Pmm_lang
open Pmm_util
open Printf
;;

let rec print_pmm_children_as_latex ch n =
  match n.Pmm_lang_ast.content with 
      Pmm_lang_ast.Text(t) -> fprintf ch "%s" t;
    | Pmm_lang_ast.Children(c) -> List.iter (print_pmm_as_latex ch) c;
and
  print_pmm_as_latex ch n =
  match n.Pmm_lang_ast.label with
    | "header" ->
        fprintf ch "\\title{%s}\n" (get_first_field_text "title" n);
        fprintf ch "\\author{%s}\n" (get_first_field_text "authors" n);
        fprintf ch "\\begin{document}\n\\maketitle\n\n";
    | "para" ->
        print_pmm_children_as_latex ch n;
        fprintf ch "\n\n";
    |  "text" ->
        print_pmm_children_as_latex ch n; 
    |  "inline eq" ->
        fprintf ch "$"; 
        print_pmm_children_as_latex ch n;
        fprintf ch "$";
    |  "display eq" ->
        fprintf ch "\\["; 
        print_pmm_children_as_latex ch n;
        fprintf ch "\\]";
        fprintf ch "\n";
    |  "table" ->
         fprintf ch "\\begin{table}\\begin{center}\n";
         fprintf ch "{\\small \\input{%s} }\n" 
           (get_first_field_text "local file" n);
         fprintf ch "\\caption{\n  \\label{tab: %s}" 
           (get_first_field_text "id" n);
         fprintf ch "%s\n}\n" (get_first_field_text "title" n);
         fprintf ch "\\end{center}\\end{table}\n";
    |  "verbatim" ->
        fprintf ch "\n\\begin{verbatim}\n";
        print_pmm_children_as_latex ch n;
        fprintf ch "\n\\end{verbatim}";
        fprintf ch "\n";
    |  _ -> ()
;;

let ch = stdout in
  fprintf ch "\\documentclass{article}\n";
  fprintf ch "\\usepackage{rj-tech-report}\n";
  fprintf ch "\\usepackage{amsmath}\n";
  fprintf ch "\\usepackage{amsfonts}\n";
  List.iter (print_pmm_as_latex ch) (wiki_of_channel_name Sys.argv.(1));
  fprintf ch "\\end{document}\n";
    


  
    
  
