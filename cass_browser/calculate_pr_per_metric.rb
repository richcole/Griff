require 'set'
require 'ds'
require 'metric_list'

avg_p = Hash.new {|h,k| h[k] = [] }
avg_r = Hash.new {|h,k| h[k] = [] }

for diagram in ARGV do

begin
 File.ftype(diagram + "-best_diagrams.txt") 
 File.ftype(diagram + "-good_diagrams.txt") 
rescue Errno::ENOENT
 puts "File not found! (" + diagram + ")"
  next
end


  best_diagrams = File.readlines(diagram + "-best_diagrams.txt").map{|x| x.chomp}
  good_diagrams = File.readlines(diagram + "-good_diagrams.txt").map{|x| x.chomp}
  
  totals = Set.new
  loader = DS::create_cache

  
  #this is the metrics (will contain sets of objects under different conditions)
  h1 = Hash.new { |h,k| h[k] = Set.new }
  hleq2 = Hash.new { |h,k| h[k] = Set.new }
  
  
  for x in best_diagrams do
  
    
    ranks_name = x.
      gsub('diagrams','stats').gsub('statss','stats').
      gsub('diagram','stats').gsub('-stats','-rank')
    ranks = loader[ranks_name].fields
    
    
    for r, v in ranks do
        
      if v.to_i == 1 then
        h1[r].add(x)
      end
      if v.to_i == 2 then
        h1[r].add(x)
        hleq2[r].add(x)
      end
    end
  end
  
  for x in $metric_names do
    #puts x
    good = Set.new( good_diagrams)
    good1 = good.intersection h1[x]
    goodleq2 = good.intersection hleq2[x]
    
    #puts "p 1      " + (good1.size.to_f/h1[x].size.to_f).to_s
    avg_p[x].push good1.size.to_f/h1[x].size.to_f

    #puts "r 1      " + (good1.size.to_f/good_diagrams.size.to_f).to_s
    avg_r[x].push good1.size.to_f/good_diagrams.size.to_f

  end

end

puts
puts

res = Hash.new {|h,k| h[k] = [] }

for x,y in avg_p do
  ap = 0
  for yy in y do
    ap += yy
  end
  ap = ap.to_f / y.size

  puts "P:" + x + "=" +  ap.to_s 
  res[x].push ap.to_s
end

for x,y in avg_r do
  ap = 0
  for yy in y do
    ap += yy
  end
  ap = ap.to_f / y.size
  puts "R:" + x + "=" +  ap.to_s
  res[x].push ap.to_s
end


puts 
puts
puts "MATLAB mode"
puts

for x,y in res do
  puts x + " " + y.join(" ") 
end
