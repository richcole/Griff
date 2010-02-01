mkdir tmp
pushd tmp
cvs -z3 -d:ext:rj-cole@cvs.sourceforge.net:/cvsroot/griff export -r HEAD htdocs
ssh rj-cole@shell.sourceforge.net "rm -rf /home/groups/g/gr/griff/htdocs/*"
scp -r htdocs rj-cole@shell.sourceforge.net:/home/groups/g/gr/griff
rm -rf htdocs
popd
rmdir tmp
