x=$1
ruby -I ../cass_browser/ ../cass_browser/calculate_stats_from_xml.rb \
  $x-diagrams.xml $x-stats.xml
ruby -I ../cass_browser/ ../cass_browser/calculate_ranks.rb $x-stats.xml \
  $x-rank.cxt
scp $x-rank.cxt kvo: