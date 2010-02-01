open Cgi;;

let print_page () =
  header "";
  let print_arg (field,value) =
    Printf.printf "Argument: field=%s, value=%s <br>\n" field value
  in
    Rb_set.iter print_arg args
;;

print_page ()

  
  
  
    
