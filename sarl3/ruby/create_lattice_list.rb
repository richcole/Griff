require 'sarl'
require 'cxt_file_format'

include Sarl

def lattice_to_xml(ic, diagram, file)
  lattice = diagram.lattice
  file.puts "<lattice title=\"#{ic.title}\">"
  for concept in lattice do
     id = concept
     upset = lattice.upset[concept].join(",")
     downset = lattice.downset[concept].join(",")
     lower_covers = lattice.lower_covers[concept].join(",")
     x = diagram[concept].x.to_f / 20.0
     y = diagram[concept].y.to_f / 20.0
     extent_size = lattice.extents[concept].count
     file.puts <<EOF
   <node id="#{id}" layer="0" upset="#{upset}" downset="#{downset}"
     lower_covers="#{lower_covers}" alpha_factor="0" alpha="" beta_factor="0" 
     beta="" x="#{x}" y="#{y}" node_type="real" extent_size="#{extent_size}" 
     contingent_size="0" feedback="0"
   />
EOF
  end
  file.puts "</lattice>"
end

latticelist = File.open("latticelist.xml", "w")
title = "Test Lattices"

latticelist.puts "<latticelist title=\"#{title}\">"

Dir.glob("contexts/*.cxt") {
  |path|
  xml = path.gsub(/\.cxt$/,".xml").gsub(/contexts\//, "diagrams/")
  puts "path=#{path}, xml=#{xml}"
  ic = interp_context_from_cxt(path)
  diagram = Diagram.layout(ic.context)

  File.open(xml, "w") { |file| lattice_to_xml(ic, diagram, file) }

  latticelist.puts "<inputlattice name=\"#{path.split('/')[-1].to_s}\" " +
    "href=\"#{xml}\" />"
}

latticelist.puts "</latticelist>"
