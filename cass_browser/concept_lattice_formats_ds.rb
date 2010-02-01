require 'concept_lattice'
require 'formal_context_formats'
require 'ds'

def index_array_to_bit_set(array, lex)
  bset = make_default_set(lex)
  array.each { |x| bset.bit_set.set(x, 1) }
  return bset
end
  
def copy_rep_into_bit_set(ds_rep, bit_set)
  ds_rep.each { |x| 
    bit_set.bit_set.set(x, 1) 
  }
end

def load_concept_lattice_from_ds(filename, loader=nil)
  loader = DS::create_cache() if loader == nil
  ds_lattice = loader[filename]

  interp_context = load_formal_context_from_ds(ds_lattice.context, loader)
  context = interp_context.context

  ds_extents = loader[ds_lattice.extent]
  ds_intents = loader[ds_lattice.intent]
  ds_upset = loader[ds_lattice.upset]
  ds_downset = loader[ds_lattice.downset]
  ds_uppercovers = loader[ds_lattice.uppercovers]
  ds_lowercovers = loader[ds_lattice.lowercovers]

  lattice = ConceptLattice.new

  # load the concepts
  for i in 1..ds_extents.map.size do
    extent = index_array_to_bit_set(ds_extents.map[i], context.objects.lex)
    intent = index_array_to_bit_set(ds_intents.map[i], context.attributes.lex)
    concept = Concept.new(extent, intent)
    lattice.concept_lex.index(concept)
  end

  # load the upper and lower covers, upsets and downsets
  for i in 1..ds_extents.map.size do
    lattice.index_lex.index(i)
    copy_rep_into_bit_set(ds_upset.map[i], lattice.upset[i])
    copy_rep_into_bit_set(ds_downset.map[i], lattice.downset[i])
    copy_rep_into_bit_set(ds_uppercovers.map[i], lattice.upper_covers[i])
    copy_rep_into_bit_set(ds_lowercovers.map[i], lattice.lower_covers[i])
  end

  lattice.compute_top_bottom

  return [lattice, interp_context]
end

def write_lattice_rep_as_ds(rep_name, lattice, meth)
  File.open(rep_name, "w") { |file|
    for x in lattice do
      ys = lattice.send(meth)[x]
      ys_str = ys.to_a.sort.join(" ")
      if ys.count == 0 then
        file.puts "-"
      else
        raise "Program Error"  if (ys_str.length == 0) 
        file.puts ys_str
      end
    end
  }
end

def write_concept_lattice_as_ds(basename, context_name, lattice)
  File.open(basename + ".lattice", 'w') { |file|
    file.puts "context=" + context_name
    file.puts "extent=" + basename + ".extent.rep"
    file.puts "intent=" + basename + ".intent.rep"
    file.puts "upset=" + basename + ".upset.rep"
    file.puts "downset=" + basename + ".downset.rep"
    file.puts "uppercovers=" + basename + ".uppercovers.rep"
    file.puts "lowercovers=" + basename + ".lowercovers.rep"
  }

  write_lattice_rep_as_ds(basename + ".extent.rep", lattice, :extent)
  write_lattice_rep_as_ds(basename + ".intent.rep", lattice, :intent)
  write_lattice_rep_as_ds(basename + ".upset.rep", lattice, :upset)
  write_lattice_rep_as_ds(basename + ".downset.rep", lattice, :downset)
  write_lattice_rep_as_ds(basename + ".lowercovers.rep", lattice, :lower_covers)
  write_lattice_rep_as_ds(basename + ".uppercovers.rep", lattice, :upper_covers)
end    


  

  
