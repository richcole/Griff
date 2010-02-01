#!/bin/bash
# script to create distribution

. config.sh

DEST=dist/$NAME

if [ -e dist ] ; then
  rm -rf dist;
fi

install -d $DEST
install *.{cpp,c,h,list} $DEST
install build{,_config,_funcs,_deps,{,_dep}_rules}.tcl $DEST
install projects.tcl Makefile $DEST
install build_config.txt $DEST

install -d $DEST/ruby
install ruby/*.{cpp,h,rb} ruby/{Makefile,{projects,build{,_deps}}.tcl} $DEST/ruby

pushd dist;
tar -czf ../$NAME.tar.gz $NAME
popd