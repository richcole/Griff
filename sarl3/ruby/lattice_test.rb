require 'sarl'
require 'cxt_file_format'

include Sarl

ic = interp_context_from_cxt('two.cxt')
lattice = Lattice.new(ic.context)

for x in lattice do
  puts "Extent: " + lattice.extents[x].to_s
  puts "Contingent: " + lattice.object_contingents[x].to_s
end
