require 'RMagick'
require 'griff/geom'
require 'diagram_drawer'

require 'concept_lattice_formats_ds'
require 'ds'
require 'lines'
require 'metric_list'

include Griff

diagram_filename = ARGV[0]
ext = ARGV[1] || "png"

loader = DS::create_cache
diagram = loader[diagram_filename]
lattice, interp_context = load_concept_lattice_from_ds(diagram.lattice)

image_list = Magick::ImageList.new
stats_filename = diagram_filename.
  gsub('diagram','stats').gsub('statss','stats').gsub('-stats','-rank')
stats = loader[stats_filename]

draw_lattice_diagram_with_metrics(
  lattice, diagram, diagram_filename, image_list, stats, $metric_lexicon)
outname = diagram_filename.gsub(/.diagram$/,'.' + ext)
puts "Output: #{outname}"
image_list.write(outname)



