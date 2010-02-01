#!/bin/bash

CVSROOT=:pserver:anonymous@cvs1:/cvsroot/griff
if [ -e sarl3 ] ; then
  rm -rf sarl3
fi

cvs -d $CVSROOT co sarl3
pushd sarl3
bash create_dist.sh
bash build_dist.sh
popd
