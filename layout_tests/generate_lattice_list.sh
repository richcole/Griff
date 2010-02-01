echo '<?xml version="1.0"?>' > latticelist.xml
echo '<latticelist title="Movies">' >> latticelist.xml
for x in *[0-9].xml ; do
  echo '<inputlattice name="'$x'" href="'$x'" />' >> latticelist.xml
done
echo '</latticelist>' >> latticelist.xml

