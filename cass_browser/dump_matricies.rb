
require 'formal_context_formats'
require 'concept_lattice'
require 'diagram'

def dump_matrix(a_name, a)
  File.open(a_name, "w") { |file|
    for i in 1..a.num_rows do
      for j in 1..a.num_columns do
        file.write(a[i-1,j-1].to_s + " ")
      end
      file.puts ""
    end
  }
end

def dump_matricies_from_context(filename)

  r = nil
  File.open(filename) { |file|
    r = load_formal_context_from_cxt_file(file)
  }
  k = r['context']

  l = ConceptLattice.new(k)
  rep = MeetIrrRep.new(l)
  pos = AdditiveDiagram.new(rep,l)

  short_name = filename.sub('.cxt', '')
  a_name = "A_" + short_name + ".ascii"
  b_name = "B_" + short_name + ".ascii"

  dump_matrix(a_name, rep.rep_matrix)
  dump_matrix(b_name, pos.vect_matrix)
end

ARGV.each { |arg|
  dump_matricies_from_context(arg)
}

        
      

  
  
