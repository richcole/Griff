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
  

pattern = ARGV[0]
count = 1
i = 1

image_list = Magick::ImageList.new
loader = DS::create_cache

Dir.glob(pattern) do |diagram_filename|
  diagram = loader[diagram_filename]
  lattice, interp_context = load_concept_lattice_from_ds(diagram.lattice)

  draw_lattice_diagram(lattice, diagram, diagram_filename, image_list)

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

if image_list.length != 0 then
  mt = image_list.montage() { 
    self.tile = Magick::Geometry.new(5,5) 
  }
  fname = "/tmp/test-#{i}.png"
  puts "Writing #{fname}"
  mt.write(fname)
end






