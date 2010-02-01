
VERSION=`bash get_version.sh` 
TMPNAME=tmp
MACHNAME=`uname -m`
DIRNAME=griff-pig-$VERSION-$MACHNAME
PATHNAME=$TMPNAME/$DIRNAME
FROMNAME=`pwd`

echo "VERSION=$VERSION"
echo "TMPNAME=$TMPNAME"
echo "MACHNAME=$MACHNAME"
echo "DIRNAME=$DIRNAME"
echo "PATHNAME=$PATHNAME"
echo "FROMNAME=$FROMNAME"

rm -rf $PATHNAME
install -d $PATHNAME
pushd $PATHNAME
for x in $@ ; do
  basename=`echo $x | sed -e 's/\(.*\\).exe$/\1/'`
  cp -v $FROMNAME/$x .
  EXES="$EXES $basename"
done

cp -v $FROMNAME/README .
cp -v $FROMNAME/COPYING .
cp -v $FROMNAME/config_vars.sh .
cp -v $FROMNAME/install_exes.sh .

echo "#!/bin/bash" >> install.sh
echo "" >> install.sh
echo "source config_vars.sh" >> install.sh
echo "EXES=\"$EXES\"" >> install.sh
echo "bash install_exes.sh \$EXES" >> install.sh

cd ..
tar -czf $FROMNAME/$DIRNAME.tar.gz $DIRNAME
rm -rf $DIRNAME
popd

echo "Binary package is $FROMNAME/$DIRNAME.tar.gz"




  
