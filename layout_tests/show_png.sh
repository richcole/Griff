#!/bin/bash

cxt=$1
num=$2
ext=$3

ruby -I ../cass_browser/ ../cass_browser/draw_diagram.rb \
  $cxt-diagrams.xml#$cxt-$num.diagram $ext

if [ -n $ext ] ; then
ext=png
fi

xv $cxt-diagrams.xml#$cxt-$num.$ext &
