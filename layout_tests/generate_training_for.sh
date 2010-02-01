#!/bin/bash

x=$1

ruby -I ../cass_browser/ ../cass_browser/bottom_cochain.rb \
    $x-diagrams.xml $x-rank.cxt > $x-best_diagrams.txt
ruby -I ../cass_browser/ ../cass_browser/draw_diagram_collage_from_filelist.rb \
    $x-best_diagrams.txt $x-best_diagrams.png
