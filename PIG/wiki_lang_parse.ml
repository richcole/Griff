type token =
  | Wiki_token_ppm of (string)
  | Wiki_token_verb of (string)
  | Wiki_token_inline_eq of (string)
  | Wiki_token_para_sep of (string)
  | Wiki_token_display_eq of (string)
  | Wiki_token_text of (string)
  | EOF

open Parsing;;
# 21 "wiki_lang_parse.mly"
  open Pmm_lang_ast ;;
# 14 "wiki_lang_parse.ml"
let yytransl_const = [|
    0 (* EOF *);
    0|]

let yytransl_block = [|
  257 (* Wiki_token_ppm *);
  258 (* Wiki_token_verb *);
  259 (* Wiki_token_inline_eq *);
  260 (* Wiki_token_para_sep *);
  261 (* Wiki_token_display_eq *);
  262 (* Wiki_token_text *);
    0|]

let yylhs = "\255\255\
\001\000\002\000\002\000\002\000\002\000\002\000\002\000\002\000\
\002\000\002\000\002\000\002\000\002\000\000\000"

let yylen = "\002\000\
\002\000\001\000\001\000\001\000\001\000\001\000\001\000\002\000\
\002\000\002\000\002\000\002\000\002\000\002\000"

let yydefred = "\000\000\
\000\000\000\000\002\000\003\000\004\000\006\000\005\000\007\000\
\014\000\000\000\008\000\009\000\010\000\012\000\011\000\013\000\
\001\000"

let yydgoto = "\002\000\
\009\000\010\000"

let yysindex = "\255\255\
\001\255\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\001\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000"

let yyrindex = "\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000"

let yygindex = "\000\000\
\000\000\000\000"

let yytablesize = 263
let yytable = "\001\000\
\017\000\003\000\004\000\005\000\006\000\007\000\008\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\
\000\000\011\000\012\000\013\000\014\000\015\000\016\000"

let yycheck = "\001\000\
\000\000\001\001\002\001\003\001\004\001\005\001\006\001\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\255\
\255\255\001\001\002\001\003\001\004\001\005\001\006\001"

let yynames_const = "\
  EOF\000\
  "

let yynames_block = "\
  Wiki_token_ppm\000\
  Wiki_token_verb\000\
  Wiki_token_inline_eq\000\
  Wiki_token_para_sep\000\
  Wiki_token_display_eq\000\
  Wiki_token_text\000\
  "

let yyact = [|
  (fun _ -> failwith "parser")
; (fun parser_env ->
    let _1 = (peek_val parser_env 1 : Pmm_lang_ast.pmm_node list) in
    Obj.repr(
# 37 "wiki_lang_parse.mly"
                         ( _1 )
# 148 "wiki_lang_parse.ml"
               : Pmm_lang_ast.pmm_node list))
; (fun parser_env ->
    let _1 = (peek_val parser_env 0 : string) in
    Obj.repr(
# 41 "wiki_lang_parse.mly"
                          ( Pmm_lang.pmm_of_string _1 )
# 155 "wiki_lang_parse.ml"
               : Pmm_lang_ast.pmm_node list))
; (fun parser_env ->
    let _1 = (peek_val parser_env 0 : string) in
    Obj.repr(
# 42 "wiki_lang_parse.mly"
                          ( [Pmm_lang.text_node_of  2 "verbatim" _1] )
# 162 "wiki_lang_parse.ml"
               : Pmm_lang_ast.pmm_node list))
; (fun parser_env ->
    let _1 = (peek_val parser_env 0 : string) in
    Obj.repr(
# 43 "wiki_lang_parse.mly"
                          ( [Pmm_lang.text_node_of  2 "inline eq" _1] )
# 169 "wiki_lang_parse.ml"
               : Pmm_lang_ast.pmm_node list))
; (fun parser_env ->
    let _1 = (peek_val parser_env 0 : string) in
    Obj.repr(
# 44 "wiki_lang_parse.mly"
                          ( [Pmm_lang.text_node_of  2 "display eq" _1] )
# 176 "wiki_lang_parse.ml"
               : Pmm_lang_ast.pmm_node list))
; (fun parser_env ->
    let _1 = (peek_val parser_env 0 : string) in
    Obj.repr(
# 45 "wiki_lang_parse.mly"
                          ( [Pmm_lang.child_node_of 0 "para"] )
# 183 "wiki_lang_parse.ml"
               : Pmm_lang_ast.pmm_node list))
; (fun parser_env ->
    let _1 = (peek_val parser_env 0 : string) in
    Obj.repr(
# 46 "wiki_lang_parse.mly"
                          ( [Pmm_lang.text_node_of  2 "text" _1] )
# 190 "wiki_lang_parse.ml"
               : Pmm_lang_ast.pmm_node list))
; (fun parser_env ->
    let _1 = (peek_val parser_env 1 : Pmm_lang_ast.pmm_node list) in
    let _2 = (peek_val parser_env 0 : string) in
    Obj.repr(
# 47 "wiki_lang_parse.mly"
                             ( 
      let insert n = 
        Pmm_lang_ast.insert_node { n with indent=n.indent+1 } in
      List.fold_right insert (Pmm_lang.pmm_of_string _2) _1
    )
# 202 "wiki_lang_parse.ml"
               : Pmm_lang_ast.pmm_node list))
; (fun parser_env ->
    let _1 = (peek_val parser_env 1 : Pmm_lang_ast.pmm_node list) in
    let _2 = (peek_val parser_env 0 : string) in
    Obj.repr(
# 52 "wiki_lang_parse.mly"
                              ( 
      Pmm_lang_ast.insert_node (Pmm_lang.text_node_of 2 "verbatim" _2) _1
    )
# 212 "wiki_lang_parse.ml"
               : Pmm_lang_ast.pmm_node list))
; (fun parser_env ->
    let _1 = (peek_val parser_env 1 : Pmm_lang_ast.pmm_node list) in
    let _2 = (peek_val parser_env 0 : string) in
    Obj.repr(
# 55 "wiki_lang_parse.mly"
                                   ( 
      Pmm_lang_ast.insert_node (Pmm_lang.text_node_of 2 "inline eq" _2) _1
    )
# 222 "wiki_lang_parse.ml"
               : Pmm_lang_ast.pmm_node list))
; (fun parser_env ->
    let _1 = (peek_val parser_env 1 : Pmm_lang_ast.pmm_node list) in
    let _2 = (peek_val parser_env 0 : string) in
    Obj.repr(
# 58 "wiki_lang_parse.mly"
                                    ( 
      Pmm_lang_ast.insert_node (Pmm_lang.text_node_of 2 "display eq" _2) _1
    )
# 232 "wiki_lang_parse.ml"
               : Pmm_lang_ast.pmm_node list))
; (fun parser_env ->
    let _1 = (peek_val parser_env 1 : Pmm_lang_ast.pmm_node list) in
    let _2 = (peek_val parser_env 0 : string) in
    Obj.repr(
# 61 "wiki_lang_parse.mly"
                                  ( 
      Pmm_lang_ast.insert_node (Pmm_lang.child_node_of 0 "para") _1
    )
# 242 "wiki_lang_parse.ml"
               : Pmm_lang_ast.pmm_node list))
; (fun parser_env ->
    let _1 = (peek_val parser_env 1 : Pmm_lang_ast.pmm_node list) in
    let _2 = (peek_val parser_env 0 : string) in
    Obj.repr(
# 64 "wiki_lang_parse.mly"
                              ( 
      Pmm_lang_ast.insert_node (Pmm_lang.text_node_of 2 "text" _2) _1
    )
# 252 "wiki_lang_parse.ml"
               : Pmm_lang_ast.pmm_node list))
(* Entry main *)
; (fun parser_env -> raise (YYexit (peek_val parser_env 0)))
|]
let yytables =
  { actions=yyact;
    transl_const=yytransl_const;
    transl_block=yytransl_block;
    lhs=yylhs;
    len=yylen;
    defred=yydefred;
    dgoto=yydgoto;
    sindex=yysindex;
    rindex=yyrindex;
    gindex=yygindex;
    tablesize=yytablesize;
    table=yytable;
    check=yycheck;
    error_function=parse_error;
    names_const=yynames_const;
    names_block=yynames_block }
let main (lexfun : Lexing.lexbuf -> token) (lexbuf : Lexing.lexbuf) =
   (yyparse yytables 1 lexfun lexbuf : Pmm_lang_ast.pmm_node list)
