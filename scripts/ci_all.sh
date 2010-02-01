#!/bin/bash

for x in sarl3 fca_debian ; do
  pushd $x;
  cvs ci -m"$1" $x;
  popd
done
