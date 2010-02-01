rm -f test*.tree;
rm -f test.lex;

./assert_graph.exe test '
  _a is-a file,
  _a has-url "http://www.google.com" leq top,
  _a has-title "Google Search" leq top,
  _a has-keyword "Search Engine" leq top,
  _a has-type "Homepage" leq keyword leq top,
  _a has-quality "high" leq top.
'

./assert_graph.exe test '
  _a is-a file,
  _a has-url "file:/home/rcole" leq top,
  _a has-title "Home Directory of RCOLE" leq top,
  _a has-keyword "Home Directory" leq keyword leq top,
  _a has-type "Filesystem Directory" leq top.
'

./assert_graph.exe test '
  _a is-a file,
  _a has-title "ITEE Homepage" leq top,
  _a has-url "itee.uq.edu.au" leq top,
  _a has-organisation "ITEE" leq keyword leq top,
  _a has-type "Homepage" leq top.
'

./assert_graph.exe test '
  _a is-a file,
  _a has-title "Ocaml Homepage" leq top,
  _a has-url "www.ocaml.org" leq top,
  _a has-keyword "Ocaml" leq keyword leq top,
  _a has-type "Homepage" leq top.
'

./assert_graph.exe test '
  _a is-a rule,
  _a has-premise "std_context -- _a _m _w.",
  _a has-conclusion "derived_context -- _a _m _w.".
'

./assert_graph.exe test '
  _a is-a rule,
  _a has-premise "derived_context -- _a _m _w, _m leq _n.",
  _a has-conclusion "derived_context -- _a _n _w.".
'

./assert_graph.exe test '
  _a is-a rule,
  _a has-premise "derived_context -- _a _m _w, _w leq _u.",
  _a has-conclusion "derived_context -- _a _m _u.".
'

./assert_graph.exe test '
  _a is-a rule,
  _a has-premise "derived_context -- _a _m _w, _w leq _u.",
  _a has-conclusion "derived_context -- _a _m _u.".
'

./assert_graph.exe test '
  _a is-a rule,
  _a has-premise "derived_context -- _a leq _b, _b leq _c.",
  _a has-conclusion "derived_context -- _a leq _c.".
'

./apply_rules.exe test
