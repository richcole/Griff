require 'formal_context_formats'
require 'concept_lattice'
require 'diagram'

def dump_matrix(file, a_name, a)
  file.puts a_name + " = [..."
  for i in 1..a.num_rows do
    file.write "   "
    for j in 1..a.num_columns do
      file.write(a[i-1,j-1].to_s + " ")
    end
    file.puts "; ..."
  end
  file.puts "]"
end

def dump_edges(file, name, lattice)
  file.puts name + " = [..."
  for a in 1..(lattice.index_lex.count) do
    file.write("   ")
    for b in 1..(lattice.index_lex.count) do
      if lattice.lower_covers[a].include?(b) then
        file.write("1 ")
      else
        file.write("0 ")
      end
    end
    file.puts "; ..."
  end
  file.puts "]"
end

def dump_matrix_simple(file, a)
  for i in 1..a.num_rows do
    for j in 1..a.num_columns do
      file.write(a[i-1,j-1].to_s + " ")
    end
    file.puts ""
  end
end

def dump_edges_simple(file, lattice)
  for a in lattice do
    for b in lattice.lower_covers[a] do
      file.puts(a.to_s + " " + b.to_s)
    end
  end
end
