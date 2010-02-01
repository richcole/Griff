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

open Printf ;;

let
  attr s   = function Some(t) -> sprintf " %s=\"%s\"" s t | None -> "" ;;

let
  mime_header = sprintf "Content-type: text/html\n\n" and
  h1      t = sprintf "<h1>%s</h1>\n"       t and
  h2      t = sprintf "<h1>%s</h1>\n"       t and
  para    t = sprintf "<p>%s</p>\n"         t and
  bold    t = sprintf "<b>%s</b>\n"         t and
  emph    t = sprintf "<i>%s</i>\n"         t and
  body    t = sprintf "<body>%s</body>"     t and
  header  t = sprintf "<header>%s</header>" t and
  title   t = sprintf "<title>%s</title>"   t and
  html    t = sprintf "<html>%s</html>"     t and
  center  t = sprintf "<center>%s</center>"  t and
  table   t = sprintf "<table>%s</table>"  t and
  input   ?name ?typ ?value t =
    sprintf "<input %s%s%s>%s</input>" 
      (attr "name" name) (attr "type" typ) (attr "value" value) t and
  tr      t = sprintf "<tr>%s</tr>" t and
  td      t = sprintf "<td>%s</td>" t and
  p       t = sprintf "<p>%s</p>" t
;;

