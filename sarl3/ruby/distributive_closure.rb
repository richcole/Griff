require 'sarl'
require 'cxt_file_format'

include Sarl

ic = interp_context_from_cxt(ARGV[0])
lattice = Lattice.new(ic.context)
dc = lattice.distributive_closure_context

for c in lattice do
  puts c.to_s + ": " + lattice.upset[c].to_s
end

st = StringTable.new
st.populate(1, dc.attributes.last)

interp_context_to_cxt(
  InterpContext.new(dc, st, st), ARGV[1])


