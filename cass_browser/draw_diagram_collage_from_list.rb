require 'RMagick'
require 'griff/geom'
require 'diagram_drawer'

require 'concept_lattice_formats_ds'
require 'ds'
require 'lines'

include Griff

loader = DS::create_cache

image_list = Magick::ImageList.new
lattice_hash = Hash.new { 
  |h,k| h[k] = load_concept_lattice_from_ds(k, loader) 
}

basename = ARGV[0]
outname = ARGV[1]
lexicon = Lexicon.new

for i in 2..(ARGV.length-1) do
  arg = ARGV[i]
  diagram_filename = basename.gsub('##', arg)
  diagram = loader[diagram_filename]
  stats_filename = diagram_filename.gsub('diagram','stats').gsub('statss','stats')
  stats = loader[stats_filename]
  for k, v in stats.fields do
    lexicon.index(k)
  end
  lattice, interp_context = lattice_hash[diagram.lattice]
  draw_lattice_diagram(lattice, diagram, diagram_filename, image_list, stats, lexicon)
end

mt = image_list.montage() {
  self.tile = Magick::Geometry.new(5,5)
}
mt.write(outname)
