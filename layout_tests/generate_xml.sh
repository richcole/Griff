for x in `echo *.diagram | sed -e 's/\.diagram//g'` ; do 
  ../bin/ds_to_xml.rb $x
done
