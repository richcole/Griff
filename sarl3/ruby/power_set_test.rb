require 'sarl'
require 'cxt_file_format'

include Sarl

ic = interp_context_from_cxt('two.cxt')

ps = PowerSet.new()

for m in ic.context.attributes do 
  ps.insert(ic.context.attribute_extent(m))
end

extents = ps.closure(ic.context.objects)

puts "extents.count=#{extents.count}"
for x in extents do
  puts "Extents=" + x.to_s
end



