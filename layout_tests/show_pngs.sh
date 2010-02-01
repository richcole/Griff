#!/bin/bash

cxt=$1
num=$2
output=/tmp/$cxt-diagrams.png 
input=$cxt-diagrams.xml#$cxt-\#\#.diagram 

echo ruby -I ../cass_browser/ ../cass_browser/draw_diagram_collage_from_list.rb \
  $input $output $num
ruby -I ../cass_browser/ ../cass_browser/draw_diagram_collage_from_list.rb \
  $input $output $num

xv $output &
