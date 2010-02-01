for x in `cat context_list.txt` ; do 
  echo "~/Source/sarl_caml/test_layout.exe $x..."
  ~/Source/sarl_caml/test_layout.exe plain $x $x-diagrams.xml
done
