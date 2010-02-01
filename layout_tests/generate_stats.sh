for x in `cat context_list.txt` ; do 
  echo "calculate_stats for $x..."
  ruby -I ../cass_browser/ ../cass_browser/calculate_stats_from_xml.rb \
    $x-diagrams.xml $x-stats.xml 
done
