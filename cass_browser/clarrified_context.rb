require 'power_map'
require 'formal_context'
require 'interps'

def calc_clarrified_context(interp_context)
  
  context = interp_context.context

  # for each intent aggregate the objects that have that intent
  clarrify_lex = Lexicon.new
  clarrify_map = make_default_power_map(context.objects.lex)

  for g in context.objects do
    intent = context.object_intent(g)
    index = clarrify_lex.index(intent)
    clarrify_map[index].add(g)
  end

  new_context = FormalContext.new()
  new_object_interp = ClarifyInterp.new(clarrify_map, interp_context)
  for g in 1..clarrify_lex.count do
    new_context.insert_object(g)
    for m in clarrify_lex.word(g) do
      new_context.insert_pair(g, m)
    end
  end

  return FormalContextAndInterps.new(
    new_context, new_object_interp, interp_context.attribute_interp
  );

end

class BasicClarifyInterp < BasicInterp
  def full_name i
    xs = @clarrify_map[i].map{ |x| 
      @interp_context.object_interp.full_name(x) 
    }
    return xs.join(" ")
  end

  def indexes i
    return @clarrify_map[i]
  end

  def short_name i
    return @clarrify_map[i].count.to_s
  end

  def object_interp
    return @interp_context.object_interp
  end

  def objects
    return @interp_context.context.objects
  end
end

class ClarifyInterp < BasicClarifyInterp
  def initialize(clarrify_map, interp_context)
    @clarrify_map = clarrify_map
    @interp_context = interp_context
  end

end

class ClarifyInterpFromDS < BasicClarifyInterp
  attr_reader :object_interp, :objects

  def initialize(clarrify_map, object_interp, objects)
    @clarrify_map = clarrify_map
    @object_interp = object_interp
    @objects = objects
  end
end

    
