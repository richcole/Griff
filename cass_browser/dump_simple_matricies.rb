require 'misc_dumpers' 

def context_of_filename(cxt_name)
  File.open(cxt_name, 'r') do |file|
    r = load_formal_context_from_cxt_file(file)
    return  r['context']
  end
  return nil
end

def dump_matricies_from_context(filename)

  context = context_of_filename(filename)
  lattice = ConceptLattice.new(context)
  rep = MeetIrrRep.new(lattice)
  base_name = filename.sub('.cxt', '');
  
  lattice = ConceptLattice.new(context)
  rep = MeetIrrRep.new(lattice)
  pos = AdditiveDiagram.new(rep,lattice)

  dump_matrix_simple(File.open(base_name + "_a.ascii", "w"), rep.rep_matrix)
  dump_edges_simple(File.open(base_name + "_e.ascii", "w"), lattice)
end

ARGV.each { |arg|
  dump_matricies_from_context(arg)
}
  
