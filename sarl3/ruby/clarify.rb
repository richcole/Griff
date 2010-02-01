require 'sarl'
require 'cxt_file_format'

include Sarl

ic = interp_context_from_cxt(ARGV[0])
cc = ClarifiedContext.new(ic.context)

puts "Number of Original Objects=#{ic.context.objects.count}"
puts "Number of Clarrified Objects=#{cc.context.objects.count}"

puts "Number of Original Attributes=#{ic.context.attributes.count}"
puts "Number of Clarrified Attributes=#{cc.context.attributes.count}"

imps = cc.context.object_imps

lc = Hash.new { |h,k| h[k] = [] }
for g in cc.context.objects do
  lc[imps[g].count].push(g)
end

puts "Histogram"
for r in lc.keys.to_a.sort do
  puts r.to_s + " " + lc[g].size.to_s
end

for r in 1..5 do
  ms = BitSet.new
  lc[r].map { 
    |x| cc.object_comap[x].each { |y| ms.set(y) }
  }
  puts r.to_s + ": " + ms.to_s
end

ms = BitSet.new
for r in 0..1 do
  lc[r].map { 
    |x| cc.object_comap[x].each { |y| ms.set(y) }
  }
end

sc = ic.context.object_subcontext(ms)
interp_context_to_cxt(
  InterpContext.new(sc, ic.st_objects, ic.st_attributes),
  'sub.cxt'
)



