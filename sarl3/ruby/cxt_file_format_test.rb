require 'cxt_file_format.rb'
require 'test_assert'

id = interp_diagram_from_cxt('b4.cxt')

test_assert(id.diagram.lattice.count == 16)

