type token =
  | VAR of (string)
  | CONST of (string)
  | PERIOD
  | COMMA
  | EOF
  | DASH
  | SEMI

val main :
  (Lexing.lexbuf  -> token) -> Lexing.lexbuf -> Graph_lang_ast.triple list list
