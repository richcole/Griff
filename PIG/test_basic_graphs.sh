./assert_graph.exe test '
  _a has-url "http://www.google.com",
  _a has-title "Google Search",
  _a has-keyword "Search Engine",
  _a has-type "Homepage",
  _a has-quality "high".
'

./assert_graph.exe test '
  _a has-url "file:/home/rcole",
  _a has-title "Home Directory of RCOLE",
  _a has-keyword "Home Directory",
  _a has-type "Filesystem Directory".
'

./assert_graph.exe test '
  _a has-title "ITEE Homepage",
  _a has-url "itee.uq.edu.au",
  _a has-organisation "ITEE",
  _a has-type "Homepage".
'

./assert_graph.exe test '
  _a has-title "Ocaml Homepage",
  _a has-url "www.ocaml.org",
  _a has-keyword "Ocaml",
  _a has-type "Homepage".
'

./query_graph.exe test '_a has-type Homepage.'




  