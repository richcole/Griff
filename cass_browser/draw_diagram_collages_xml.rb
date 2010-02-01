require 'RMagick'
require 'griff/geom'
require 'diagram_drawer'

require 'concept_lattice_formats_ds'
require 'ds'
require 'lines'

include Griff

if ARGV.length == 0 then
  puts "Usage: draw_diagram_collages <pattern>"
  puts "  where <pattern> is a file pattern matching diagram names."
  puts "  output will be a collection of images in /tmp"
end
  

diagram_xml_filename = ARGV[0]
count = 1
i = 1

image_list = Magick::ImageList.new
loader = DS::create_cache

doc = loader.get_document(diagram_xml_filename)
doc.elements.each("ds/*") do 

  |element|

  if element.name =~ /.diagram$/ then
    diagram_filename = diagram_xml_filename + "#" + element.name
    diagram = loader[diagram_filename]
    lattice, interp_context = 
      load_concept_lattice_from_ds(diagram.lattice, loader)
    draw_lattice_diagram(lattice, diagram, element.name, image_list)
    
    count += 1
    if count > 25 then
      mt = image_list.montage() { 
        self.tile = Magick::Geometry.new(5,5) 
      }
      fname = "/tmp/test-#{i}.png"
      puts "Writing #{fname}"
      mt.write(fname)
      i += 1
      count = 1
      image_list = Magick::ImageList.new
    end
  end
end

if image_list.length != 0 then
  mt = image_list.montage() { 
    self.tile = Magick::Geometry.new(5,5) 
  }
  fname = "/tmp/test-#{i}.png"
  puts "Writing #{fname}"
  mt.write(fname)
end






