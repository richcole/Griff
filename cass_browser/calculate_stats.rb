require 'concept_lattice_formats_ds'
require 'ds'
require 'ds_formats'
require 'metrics'

ARGV.each do |diagram_filename| 
  stats_filename = diagram_filename.
    gsub('diagrams','stats').gsub('diagram','stats')
  puts "Writing '#{stats_filename}'"

  loader = DS::create_cache
  diagram = loader[diagram_filename]
  lattice, interp_context = load_concept_lattice_from_ds(diagram.lattice)
  
  edges = num_edge_crossings(lattice, diagram)
  
  stats = loader[stats_filename]
  stats.fields['EdgeCrossing'] = edges
  
  write_stats_as_ds_filename(stats_filename, stats)
end




