
require 'formal_context_formats'
require 'concept_lattice'
require 'contraordinal_scale'
require 'misc_dumpers'

def load_context(file) 
  result = load_formal_context_from_cxt_file(file)
  context = result.context
  return result.context
end

def calculate_contraordinal_scale(context) 
    
  lattice = ConceptLattice.new(context)
  rep = MeetIrrRep.new(lattice)
  attribute_order = rep.attr_order
  scale = ContraOrdinalScale.new(attribute_order)
  return scale
  
end

ARGV.each do |filename| 
  File.open(filename, "r") do |file|

    context = load_context(file)
    lattice = ConceptLattice.new(context)
    rep = MeetIrrRep.new(lattice)
    attribute_order = rep.attr_order

    scale = ContraOrdinalScale.new(attribute_order)
    scale_lattice = ConceptLattice.new(scale.context)
    scale_rep = AttrRep.new(scale_lattice, rep.irreducibles)

    File.open("contra_" + filename, "w") do |outfile|
      interp = ToStringInterp.new()
      interp_context = new FormalContextAndInterps(
        scale.context, interp, interp
      )
      write_formal_context_to_cxt_file(interp_context, outfile)
    end

    fname = "contra_" + filename.sub('.cxt', '.m')
    File.open(fname, "w") do |outfile| 
      dump_matrix(outfile, "A", scale_rep.rep_matrix);
      outfile.puts("V = [ (rand(size(A,2), 1) - 0.5) rand(size(A,2), 1) ]")
      dump_edges(outfile, "E", scale_lattice);
    end

    base_name = "contra_" + filename.sub('.cxt', '')

    File.open(base_name + "_l.ascii", "w") do |file|
      for x in 1..lattice.count do
        s = ""
        for y in rep.irreducibles do
          if lattice.upset[x].include?(y) then
            s << "1 "
          else
            s << "0 "
          end
        end
        file.puts(s)
      end
    end
          
    File.open(base_name + "_a.ascii", "w") do |file|
      for x in 1..scale_lattice.count do
        s = ""
        for y in rep.irreducibles do
          if scale_lattice.concept(x).intent.include?(y) then
            s << "1 "
          else
            s << "0 "
          end
        end
        file.puts(s)
      end
    end

    dump_edges_simple(File.open(base_name + "_e.ascii", "w"), scale_lattice)
  end
end
  
