require 'set'
require 'ds'

totals = Set.new
loader = DS::create_cache

precision = Hash.new
recall = Hash.new
n = 5

ave_precision = Hash.new { |h,k| h[k] = 0 }
ave_recall = Hash.new { |h,k| h[k] = 0 } 
instance_count = Hash.new { |h,k| h[k] = 0 } 
zero_count = Hash.new { |h,k| h[k] = 0 } 

File.open("top_n_diagrams.txt", "w") {
  |top_n|

  for n in 1..5 do 
    
    ARGV.each { 
      |basename|
      
      best_diagrams_name = basename + '-best_diagrams.txt'
      good_diagrams_name = basename + '-good_diagrams.txt'
      
      best_diagrams = File.readlines(best_diagrams_name).map{|x| x.chomp}
      good_diagrams = File.readlines(good_diagrams_name).map{|x| x.chomp}
      
      h = Hash.new { |h,k| h[k] = [] }
      
      for x in best_diagrams do
        ranks_name = x.
          gsub('diagrams','stats').gsub('statss','stats').
          gsub('diagram','stats').gsub('-stats','-rank')
        ranks = loader[ranks_name].fields
        total = 0
        for r, v in ranks do
          if v.to_i == 1 then
            total += 1
          end
        end
        h[total].push(x)
        totals.add(-total)
      end
      
      class_rank = []
      for x in totals.to_a.sort do
        for y in h[-x] do
          class_rank.push(y)
        end
      end
      
      good_diagram_set = Set.new(good_diagrams)
      numerator = 0
      
      for x in class_rank[0..(n-1)] do
        top_n.puts "n=#{n}, basename=#{basename}, diagram=#{x}"
        if good_diagram_set.include?(x) then
          numerator += 1
        end
      end
      
      precision[basename] = numerator.to_f / n
      recall[basename] = numerator.to_f / good_diagram_set.size
      
      printf "%d %s %2.2f %2.2f\n", n, basename, precision[basename], recall[basename]
      ave_precision[n] += precision[basename]
      ave_recall[n] += recall[basename]
      if precision[basename] == 0 then
        zero_count[n] += 1
      end
      instance_count[n] += 1
    }
    
    ave_precision[n] = ave_precision[n].to_f / instance_count[n]
    ave_recall[n] = ave_recall[n].to_f / instance_count[n]
    
    printf "%d %s %2.2f %2.2f %2.2f\n", n, "Average", ave_precision[n], ave_recall[n], zero_count[n]
  end
}
