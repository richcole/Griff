require 'concept_lattice_formats_ds'
require 'ds'
require 'ds_formats'
require 'metrics'

loader = DS::create_cache

diagram_xml_filename = ARGV[0]
stats_xml_filename = ARGV[1]

lattice_hash = Hash.new { 
  |h, filename|
  h[filename] = load_concept_lattice_from_ds(filename)
}

File.open(stats_xml_filename, "w") do 
  |stats_xml_file|

  stats_xml_file.puts "<ds>"
  doc = loader.get_document(diagram_xml_filename)
  doc.elements.each("ds/*") do 
    |element|

    if element.name =~ /.diagram$/ then
      diagram_filename = element.name
      stats_filename = diagram_filename.
        gsub('diagrams','stats').gsub('diagram','stats')
#      puts "file=#{stats_filename}"

      diagram = loader[diagram_xml_filename + "#" + diagram_filename]
      lattice, interp_context = lattice_hash[diagram.lattice]
      
      stats = DS::Stats.new(stats_xml_filename + '#' + stats_filename)
      stats.fields['edgeCrossing'] = num_edge_crossings(lattice, diagram)
      stats.fields['symChildren'] = num_symmetric_children(lattice, diagram)
      stats.fields['symChildrenNZ'] = num_symmetric_children_nz(lattice, diagram)
      stats.fields['reflections'] = num_reflections(lattice, diagram)
      stats.fields['vectors'] = num_vectors(lattice, diagram)
      stats.fields['chains'] = num_chains(lattice, diagram)
      stats.fields['edgeLength'] = total_edge_length(lattice, diagram)
      stats.fields['pathWidth'] = ave_path_width(lattice, diagram)
      stats.fields['meetVectors'] = num_meet_vectors(lattice, diagram)
      stats.fields['wellPlaced'] = num_well_placed_children(lattice, diagram)
      stats.fields['okPlaced'] = num_ok_placed_children(lattice, diagram)
      stats.fields['ballance'] = children_ballance(lattice, diagram)
      stats.fields['numThreeChains'] = num_three_chains(lattice, diagram)
      stats.fields['planar'] = (stats.fields['edgeCrossing'] == 0 ? 1 : 0)
      stats.fields['horizontalShift'] = horizontal_shift(lattice, diagram)
      stats.fields['num_unique_gradients'] = num_unique_gradients(lattice, diagram)
      stats.fields['num_unique_gradients_abs'] = num_unique_gradients_abs(lattice, diagram)

      write_stats_as_ds_xml(stats_xml_file, stats_filename, stats)
    end
  end
  stats_xml_file.puts "</ds>"
end




