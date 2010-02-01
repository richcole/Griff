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

type pmm_node_content = 
    Text of string
  | Children of pmm_node list

and pmm_node = {
  indent: int; label: string; content: pmm_node_content
}

let error_text_node (ni,nt) =
  [ {indent=ni; label="Error"; content=Text nt} ]

let rec insert_node n ns =
  match ns with
    | [] -> [n]
    | [last_n] ->
        begin
          if last_n.indent >= n.indent then
            last_n :: [n]
          else
            match last_n.content with
              | Text(t) -> last_n :: [n]
              | Children(c) -> 
                  [{ last_n with content=Children (insert_node n c )}]
        end
    | hd :: tl -> hd :: (insert_node n tl)
              

let rec insert_text (ni,nt) ns = 
  match ns with
    | []   -> [ { indent=ni; label="anon"; content=Text nt; } ]
    | [nl] ->
        begin
          match nl.content with
            | Text(t) -> [ {nl with content=Text(t ^ " " ^ nt)} ]
            | Children([]) -> [ {nl with content=Text(nt)} ]
            | Children(c) -> 
                if ni > nl.indent then
                  [ {nl with content=Children (insert_text (ni,nt) c)} ]
                else
                  (nl :: [ {indent=ni; label="Error"; content=Text nt} ])
        end
    | h :: tl ->
        h :: (insert_text (ni,nt) tl)

let rec insert_empty_label (ni,nlab) ns =
  let tnode = { indent=ni; label=nlab; content=Children [] } in
    match ns with
      | [] -> [ tnode ]
      | [nl] ->
          begin
            match nl.content with
              | Text(t) -> nl :: [tnode]
              | Children(s) -> 
                  if ni > nl.indent then
                    [ { 
                        nl with content=
                          Children (insert_empty_label (ni,nlab) s) 
                      } ]
                  else
                    nl :: [tnode]
          end
      | h :: t -> h :: insert_empty_label (ni,nlab) t

let insert_label (ni,nlab,ntex) ns =
  let r1 = insert_empty_label (ni,nlab) ns in
    if String.length (Usual.trim " \t\n" ntex) > 0 then
      insert_text (ni,ntex) r1
    else
      r1
