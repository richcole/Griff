for x in `cat context_list.txt` ; do 
  ruby -I ../cass_browser/ ../cass_browser/build_metric_lattice.rb "stats/$x*"
done
