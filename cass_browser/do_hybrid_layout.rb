require 'concept_lattice'
require 'formal_context_formats'
require 'hybrid_layout'
require 'concept_lattice_formats'

input_filename = ARGV[0]
output_filename = ARGV[1]

puts "Input: #{input_filename}, Output: #{output_filename}"

if 0 then
  File.open(input_filename) do |file|
    result = load_formal_context_from_cxt_file(file)
    context = result.context
    lattice = ConceptLattice.new(context)
    layout = HybridLayout.new(lattice)
    write_concept_lattice_as_xml(output_filename, layout)
  end
end
