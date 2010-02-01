#!/bin/bash

for x in sarl3 fca_debian ; do
  pushd $x;
  cvs update -P -d
  popd
done
