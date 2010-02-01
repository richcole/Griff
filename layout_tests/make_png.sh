#!/bin/bash

cxt=$1
num=$2

ruby -I ../cass_browser/ ../cass_browser/draw_diagram.rb \
  $cxt-diagrams.xml#$cxt-$num.diagram

scp $cxt-diagrams.xml#$cxt-$num.png kvo:
