FCA_DEBIAN=..

zcat *.gz | $FCA_DEBIAN/packages_to_cnf

../../sarl3/dump_uninstallable_packages.exe \
  packages.cnf packages.lex 1 30000 > check_installable.txt

grep Uninstallable check_installable.txt | \
  sed -e 's/Checking \(.*\): num vars.*/\1/' | sort > uninstallable.txt

sed -e 's/\([^ ]*\) .*/\1/' reasons.txt | sort > simply_uninstallable.txt

comm -13 simply_uninstallable.txt uninstallable.txt > complex_uninstallable.txt

