def write_concept_lattice_as_xml_file(file, layout)
  lattice = layout.lattice
  file.puts "<lattice>"
  for x in lattice do
    p = layout.pos_of(x)
    file.puts "<node id=\"#{x}\" "
    file.puts "  upset=\"#{minus_x(lattice.upset[x],x).to_a.join(",")}\" "
    file.puts "  downset=\"#{minus_x(lattice.downset[x],x).to_a.join(",")}\" "
    file.puts "  lower_covers=\"#{lattice.lower_covers[x].to_a.join(",")}\" "
    file.puts "  x=\"#{p[0]*50}\" y=\"#{p[1]*50}\" type=\"real\" />"
  end
  file.puts "</lattice>"
end

def write_concept_lattice_as_xml(filename, layout)
  File.open(filename, "w") { |file|
    write_concept_lattice_as_xml_file(file, layout)
  }
end

  
