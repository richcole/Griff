for x in `cat context_list.txt` ; do 
  echo "~/Source/sarl_caml/test_layout.exe $x..."
  ~/Source/sarl_caml/test_layout.exe plain $x $x-diagrams.xml

  echo "calculate_stats for $x..."
  ruby -I ../cass_browser/ ../cass_browser/calculate_stats_from_xml.rb \
    $x-diagrams.xml $x-stats.xml 

  echo "generate_rank_context for $x..."
  ruby -I ../cass_browser/ ../cass_browser/calculate_ranks.rb \
    $x-stats.xml $x-rank.cxt $x-rank.xml
done
