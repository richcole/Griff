#!/bin/bash

source config_vars.sh

for x in $@ ; do
  echo "rm $PREFIX/bin/pig_$x.exe"
  rm -f $PREFIX/bin/pig_$x
done
