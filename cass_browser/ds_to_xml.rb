require 'ds'

include DS

def print_xml(loader, basename, diagram, file)
  lattice = loader[diagram.lattice]
  upset = loader[lattice.upset]
  downset = loader[lattice.downset]
  lowercovers = loader[lattice.lowercovers]
  uppercovers = loader[lattice.uppercovers]

  file.puts "<lattice title=\"#{basename}\" >"
  for i in 1..diagram.points.length-1 do
    p = diagram.points[i]
    file.puts "  <node "
    file.puts "    id=\"#{i}\" "
    file.puts "    x=\"#{p.x}\" y=\"#{p.y}\" "
    file.puts "    upset=\"#{upset.map[i].join(",")}\" "
    file.puts "    downset=\"#{downset.map[i].join(",")}\" "
    file.puts "    lower_covers=\"#{lowercovers.map[i].join(",")}\" "
    file.puts "    upper_covers=\"#{uppercovers.map[i].join(",")}\" "
    file.puts "    node_type=\"real\" "
    file.puts "  />"
  end
  file.puts "</lattice>"
end

ARGV.each { |basename|
  basename = basename.gsub('.diagram','')
  outname = basename + '.xml'

  puts "Loading... #{basename}"
  loader = create_cache
  diagram = loader[basename + '.diagram']
  
  puts "Writing... #{outname}"
  
  File.open(outname, "w") { |file|
    print_xml(loader, basename, diagram, file)
  }
}
