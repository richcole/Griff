#!/bin/bash

FCA_DEBIAN=..
SARL=../../sarl3
x=$1

# id=`grep " $x" packages.lex | cut -f 1 -d " "`
# ./dump_uninstallable_packages.exe packages.{cnf,lex} $id 0

$SARL/reduce_clauses.exe $x.cnf $x.reduced.cnf $x.mapping.txt
$SARL/min_unsat.exe $x.reduced.cnf $x.reduced.min_clauses.cnf

ruby $FCA_DEBIAN/clauses_to_dot.rb \
  $x.reduced.min_clauses.cnf packages.lex $x.mapping.txt \
  > $x.reduced.min_clauses.dot

dot -Tpng $x.reduced.min_clauses.dot -o $x.reduced.min_clauses.png
# gv $x.reduced.min_clauses.ps &


