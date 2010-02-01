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

open Printf
open Pmm_lang
open Pmm_lang_ast
open Pmm_util
;;


let print_comment node =
  printf "<table bgcolor=\"yellow\" width=100%%>";
  printf "<tr><td>Comment:</tr></td>\n";
  printf "<tr><td>Author:%s</tr></td>\n" 
    (comma_sep_url_list "author" (get_first_field_text "author" node));
  printf "<tr><td>Text:%s</td></tr>\n" (get_first_field_text "text" node);
  printf "</table>\n";
;;

let print_paper paper =
  if paper.label = "paper" then
    begin
      printf "<table bgcolor=\"blue\" width=100%%>\n";
      printf "<tr><td><font size=+2>%s</font></td></tr>" 
        (get_first_field_text "title" paper);
      printf "<tr><td>\n";
      printf "Authors: %s <br>\n" (get_first_field_text "authors" paper);
      printf "Keywords: %s" 
        (comma_sep_url_list "keyword" (get_first_field_text "keywords" paper));
      printf "</tr></td>\n";
      iter_children "comment" print_comment paper;
      printf "</table>\n<br><br>";
    end
  else
    ()

let () = 
  printf "<html><body>";
  List.iter print_paper (pmm_of_channel_name Sys.argv.(1));
  printf "</html></body>";

