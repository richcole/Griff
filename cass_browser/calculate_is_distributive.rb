require 'metrics'
require 'formal_context_formats'

ARGV.each {
  |filename|
  File.open(filename) { 
    |file|
    interp_context = load_formal_context_from_cxt_file(file)
    lattice = ConceptLattice.new(interp_context.context)
    is_distrib = is_distributive(lattice)
    is_distrib_half = is_distributive_half(lattice)
    puts "#{filename} #{is_distrib} #{is_distrib_half}"
  }
}
