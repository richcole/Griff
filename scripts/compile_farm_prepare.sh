if [ ! -e $HOME/usr ] ; then
  mkdir $HOME/usr
fi

tar -xzf tcl8.4.12-src.tar.gz 
pushd tcl8.4.12/unix
./configure --prefix=$HOME/usr
make -j8
make install
popd

tar -xxf gc6.3.tar.gz
push gc6.3
./configure --prefix=$HOME/usr
make -j8
make install
popd




