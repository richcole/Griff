
require 'misc_dumpers' 

def dump_matricies_from_context(filename)

  r = nil
  File.open(filename) { |file|
    r = load_formal_context_from_cxt_file(file)
  }
  k = r['context']

  lattice = ConceptLattice.new(k)
  rep = MeetIrrRep.new(lattice)
  pos = AdditiveDiagram.new(rep,lattice)

  File.open("output.ascii", "w") do |file|
    dump_matrix(file, "A", rep.rep_matrix)
    dump_matrix(file, "V", pos.vect_matrix)
    dump_edges(file, "E", lattice)
  end
end

ARGV.each { |arg|
  dump_matricies_from_context(arg)
}

        
      

  
  
