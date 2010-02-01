require 'concept_lattice_formats'
require 'concept_lattice_formats_ds'
require 'formal_context_formats'
require 'formal_context'
require 'concept_lattice'

cxt_filename = 'b3.cxt'

interp_context = nil
File.open(cxt_filename, 'r') { |cxt_file|
  interp_context = load_formal_context_from_cxt_file(cxt_file)
}

context = interp_context.context
lattice = ConceptLattice.new(context)

File.open('1/b3.cxt', 'w') { |file|
  write_formal_context_to_cxt_file(interp_context, file)
}

write_concept_lattice_as_ds('1/b3', '1/b3.context', lattice)
write_formal_context_as_ds('1/b3', context)

lattice, context = load_concept_lattice_from_ds('1/b3.lattice')
write_concept_lattice_as_ds('2/b3', '2/b3.context', lattice)
write_formal_context_as_ds('2/b3', context)

