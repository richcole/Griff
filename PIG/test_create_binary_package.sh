#!/bin/bash

VERSION=`bash get_version.sh` 
MACHNAME=`uname -m`
PKGNAME=griff-pig-$VERSION-$MACHNAME
PREFIX=/tmp

bash configure --prefix=$PREFIX
make binary_tgz_package
rm -rf $PKGNAME
tar -xzf $PKGNAME.tar.gz
pushd $PKGNAME
bash install.sh

if [ -e $PREFIX/tmp/bin/griff_assert_csv ] ; then
  echo "Error: couldn't find installed version of assert_csv"
fi
