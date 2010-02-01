 
# Copyright 2004, Richard Cole
#
# This file is part of Griff.
# 
# Griff is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 2 of the License, or (at your
# option) any later version.
# 
# Griff is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
# 
# You should have received a copy of the GNU General Public License
# along with Griff; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

set contexts { pig tree test html_pig pmm pmm_pig } 

set CCC g++

foreach x $contexts {
    set CPP_LIBDIR($x) { /usr/lib/gcc/i486-linux/3.4.4/ }
}

set CPP_SOURCES(tree) {
  payload tuple tree tree_node tree_node_factory tree_node_iterator 
  avl_tree_node_ballance mmap_alloc hash
}

set CPP_OPTS(tree) {-O}

set CPP_TESTS(tree) {
  test_payload test_tree test_tree_from_file test_tree_remove test_tree_random 
  time_tree prof_tree test_cmp test_tree_remap 
}

set CPP_TARGETS(tree) {
}

set CPP_LIBRARY(tree) {
   tree
}

set INCLUDES(tree)        {
}

set LIBRARY(pig)          {
  pig
}

set LIBRARY(html_pig)          {
  html_pig
}

set CPP_OPTS(pig) {-O}

set CPP_OBJECTS(pig) $CPP_SOURCES(tree)

set CPP_SOURCES(pig) {
  ml_pig
}

set CPP_INCLUDES(pig) {
  -I\$(CAML_INCLUDE)
}

set ML_SOURCES(pig) {
  pig quad_store quad_graph 
  graph_lang_quote graph_lang_ast action_lang_ast
  graph_lang assert_string retract_string query_string
  pig_util
  pig_rules
}

set MLI_SOURCES(pig) {
  pig
}

set MLL_SOURCES(pig) {
  graph_lang_lex csv_lang_lex action_lang_lex
}

set MLY_SOURCES(pig) {
  graph_lang_parse action_lang_parse
}

set ML_CPP_LIBS(pig) {
  -lstdc++ -lgcc_s
}

set ML_CPP_OPTS(pig) {
  -ggdb
}

set ML_CMO_OPTS(pig) {
  -g
}

set ML_MKLIB_OPTS(pig) {
}

set ML_TESTS(pig) {
    test_pig test_quad_store test_quad_graph test_graph_lang
    test_store
    dump_quad_store 
    print_extent print_lower_covers 
    print_lex test_lex test_load_lex print_cxt_lines print_cxt print_cxt2 retract_graph
}

set ML_TARGETS(pig) {
    assert_graph assert_graph_from_file 
    query_graph assert_csv apply_rules 
    apply_neg_rules execute_action
}

set ML_SOURCES(html_pig) {
  html
}

set ML_TESTS(html_pig) {
}
#  edit_document_cgi

set ML_LIBRARY(pig) {
  pigml
}

set ML_LIBRARY(html_pig) {
  html_pig
}

set ML_PACKAGES(pig) {
  rj_caml
}

set ML_PACKAGES(html_pig) {
  rj_cgi rj_caml
}

set ML_LIBS(pig) {
  str rj_caml
}

set ML_LIBS(html_pig) {
  str rj_caml pigml
}

# ------------------------------------------------------------
# pmm - poor mans markup
# ------------------------------------------------------------

set CPP_SOURCES(pmm) {
}

set CPP_OPTS(pmm)  "-ggdb"

set CPP_TESTS(pmm) {
}

set TARGETS(pmm) {
}

set CPP_LIBRARY(pmm) {
}

set CPP_INCLUDES(pmm) {
}

set ML_SOURCES(pmm) {
  pmm_util wiki_lang
  pmm_lang_ast pmm_lang 
}

set ML_BEFORE_SOURCES(pmm) {
}

set MLI_SOURCES(pmm) {
}

set MLL_SOURCES(pmm) {
  pmm_lang_lex wiki_lang_lex
}

set MLY_SOURCES(pmm) {
  pmm_lang_parse wiki_lang_parse
}

set ML_CPP_LIBS(pmm) {
}

set ML_CPP_OPTS(pmm) {
}

set ML_CMO_OPTS(pmm) {
  -g
}

set ML_MKLIB_OPTS(pmm) {
}

set ML_TESTS(pmm) {
}

set ML_TARGETS(pmm) {
  test_pmm_lang print_bookmarks 
  print_keywords print_authors
  print_pmm_as_xml print_wiki_as_xml
  print_wiki_as_latex print_wiki_as_source
  print_bibtex
}

set ML_LIBRARY(pmm) {
  pmm_caml
}

set ML_PACKAGES(pmm) {
  rj_caml
}

set ML_LIBS(pmm) {
  str rj_caml
}

set ML_OBJS(pmm) {
}

# ------------------------------------------------------------
# pmm_pig - poor mans markup
# ------------------------------------------------------------

set ML_CMO_OPTS(pmm_pig) {
  -g
}

set ML_TARGETS(pmm_pig) {
  assert_pmm_from_file
}

set ML_PACKAGES(pmm_pig) {
  rj_caml
}

set ML_LIBS(pmm_pig) {
  str rj_caml pmm_caml pigml
}

# ------------------------------------------------------------
# create the makefile
# ------------------------------------------------------------

source super_create_makefile.tcl