for x in `cat context_list.txt` ; do 
  echo "generate_rank_context for $x..."
  ruby -I ../cass_browser/ ../cass_browser/calculate_ranks.rb \
    $x-stats.xml $x-rank.cxt $x-rank.xml
done
