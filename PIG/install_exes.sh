#!/bin/bash

source config_vars.sh

for x in $@ ; do
  echo "install $x.exe $PREFIX/bin/pig_$x.exe"
  install -D $x.exe $PREFIX/bin/pig_$x
done
