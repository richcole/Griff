require 'RMagick'
require 'griff/geom'
require 'diagram_drawer'

require 'concept_lattice_formats_ds'
require 'ds'
require 'lines'
require 'lexicon'
require 'metric_list'

include Griff

loader = DS::create_cache

image_list = Magick::ImageList.new
lattice_hash = Hash.new { 
  |h,k| h[k] = load_concept_lattice_from_ds(k, loader) 
}

if ARGV.length != 2 then
  puts "Usage: draw_diagram_collage_from_filelist <filelist_file> <output_filename>"
end

File.open(ARGV[0]) {
  |file|
  file.readlines.each {
    |diagram_filename|
    stats_filename = diagram_filename.
      gsub('diagram','stats').gsub('statss','stats').gsub('-stats','-rank')
    stats = loader[stats_filename]
    diagram = loader[diagram_filename]
    lattice, interp_context = lattice_hash[diagram.lattice]
    draw_lattice_diagram_with_metrics(
      lattice, diagram, diagram_filename, image_list, stats, $metric_lexicon)
  }
}

mt = image_list.montage() {
  self.tile = Magick::Geometry.new(5,4)
  self.geometry = "400x400"
}
mt.write(ARGV[1])
