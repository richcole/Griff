require 'formal_context_formats'
require 'default_set'
require 'set'
require 'metrics'
require 'concept_lattice_formats_ds'

class BestObjects

  attr_reader :best_objects, :context

  def initialize(diagram_xml_filename, interp_context)
    @interp_context = interp_context
    @context = interp_context.context
    @diagram_xml_filename = diagram_xml_filename

    @best_objects = make_default_set(@context.objects.lex)

    @loader = DS::create_cache
    @lattice_hash = Hash.new { 
      |h, filename|
      h[filename] = load_concept_lattice_from_ds(filename)
    }

    for g in @context.objects do
      add(g)
    end
  end

  def is_better(g1, g2)
    intent1 = context.object_intent(g1)
    intent2 = context.object_intent(g2)
    if intent2.eql?(intent1) then
      return false
    else
      if intent2.subseteq?(intent1) then
        return true
      else
        return false
      end
    end
    raise ProgramError, "Cannot get to here"
  end

  def add(g)

    g_intent = context.object_intent(g)
    is_g_inferior = false
    rem_set = Set.new

    $stderr.puts "g=#{g}, @best_objects.size=#{best_objects.size}"

    for x in @best_objects do
      x_intent = context.object_intent(x)
      if is_better(g, x) then
        rem_set.add(g)
      else
        if is_better(x, g) || diagrams_equal?(x, g) then
          is_g_inferior = true
          break
        end
      end
    end

    for x in rem_set do
      @best_objects.remove(x)
    end

    if ! is_g_inferior then
      @best_objects.add(g)
    end
  end

  def each(&block)
    @best_objects.each(&block)
  end

  def diagrams_equal?(g1, g2)
    diagram1 = @loader[diagram_name(g1)]
    diagram2 = @loader[diagram_name(g2)]
    lattice, interp_context = @lattice_hash[diagram1.lattice]
    return is_same_diagram(lattice, diagram1, diagram2)
  end

  def diagram_name(g)
    d = @interp_context.object_interp.full_name(g).gsub('stats', 'diagram')
    return @diagram_xml_filename + '#' + d
  end

end

if ARGV.length != 2 then    
  $stderr.puts "Usage: bottom_cochain.rb <diagram_xml_filename> <rank_context.cxt>"
  exit
end

diagram_xml_filename = ARGV[0]
context_filename = ARGV[1]

interp_context = load_formal_context_from_cxt_filename(context_filename)
best_objects = BestObjects.new(diagram_xml_filename, interp_context)

num_distinct_diagrams = 0
for x in best_objects do
  puts best_objects.diagram_name(x)
  num_distinct_diagrams += 1
end

File.open("diagram_lattice_stats.txt", "a+") do
  |file|
  classes = Set.new
  for g in best_objects do
    classes.add(interp_context.context.object_intent(g))
  end
  nm = diagram_xml_filename
  file.puts "#{nm} classes #{classes.size}"
  file.puts "#{nm} num_diagrams #{interp_context.context.objects.count}"
  file.puts "#{nm} num_distinct_diagram #{num_distinct_diagrams}"
end
