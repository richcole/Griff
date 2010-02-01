
type index = int
type tuple = int array

type pig_tree
type pig_lex

val create_tree : string -> pig_tree
val create_tree_new : string -> pig_tree

val create_lex : string -> pig_lex
val create_lex_new : string -> pig_lex

val insert : pig_tree -> tuple -> unit
val remove : pig_tree -> tuple -> unit
val first_gte : pig_tree -> tuple -> tuple
val succ   : pig_tree -> tuple -> tuple

val index  : pig_lex -> string -> int 
val word   : pig_lex -> int -> string 
val index_count : pig_lex -> int

val make_tuple : int -> int -> int -> int -> tuple


