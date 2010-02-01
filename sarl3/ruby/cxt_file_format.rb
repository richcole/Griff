require 'sarl'
require 'interp_context'
require 'interp_diagram'

CHAR_CROSS='x'[0]
CHAR_DOT='.'[0]

def interp_context_from_cxt_file(file)
  
  if file == nil then
    raise SyntaxError, "Invalid File Passes"
  end    

  line_num = 0
  char_num = 0

  header = file.gets.chomp 
  if header != "B"  then
    raise SyntaxError, "Expected 'B', got '#{header}' on line #{file.lineno}"
  end

  title = file.gets.chomp
  num_g = file.gets.chomp.to_i
  num_m = file.gets.chomp.to_i

  context = Sarl::Context.new
  objects = Sarl::StringTable.new
  attributes = Sarl::StringTable.new

  for i in 1..num_g do
    objects[i] = file.gets.chomp
    context.insert_object i
  end

  for i in 1..num_m do
    attributes[i] = file.gets.chomp
    context.insert_attribute i
  end

  for g in 1..num_g do
    s = file.gets.chomp
    for m in 1..num_m do
      c = s[m-1]
      if c == CHAR_CROSS then
        context.insert(g, m)
      else
        if c != CHAR_DOT then
          raise SyntaxError, "Expected 'x' or '.' on " + 
            "line #{file.lineno}, line='#{s}', c='#{c}'"
        end
      end
    end
  end

  return Sarl::InterpContext.new(context, objects, attributes, title)
end

def interp_context_from_cxt(filename)
  File.open(filename) {
    |file|
    return interp_context_from_cxt_file(file)
  }
end

def interp_diagram_from_cxt(filename)
  ic = interp_context_from_cxt(filename)
  lattice = Sarl::Lattice.new(ic.context)
  diagram = Sarl::Diagram.new(lattice)
  return Sarl::InterpDiagram.new(diagram, ic.st_objects, ic.st_attributes)
end

def interp_context_to_cxt(ic, filename)
  File.open(filename, 'w') {
    |file|
    file << "B\n"
    file << "Navigation Point\n"
    file << ic.context.objects.count << "\n"
    file << ic.context.attributes.count << "\n"
    for g in ic.context.objects do
      file << ic.st_objects[g] << "\n"
    end
    for m in ic.context.attributes do 
      file << ic.st_attributes[m] << "\n"
    end
    for g in ic.context.objects do
      for m in ic.context.attributes do
        if ic.context.object_intent(g).is_set(m) then
          file << "x"
        else
          file << "."
        end
      end
      file << "\n"
    end
  }
end

