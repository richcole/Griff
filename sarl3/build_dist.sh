. config.sh

if [ -e tmp ] ; then
  rm -rf tmp;
fi

mkdir tmp
pushd tmp

tar -xzf ../$NAME.tar.gz
pushd $NAME
make

popd
popd
