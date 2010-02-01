 
# Copyright 2004, Richard Cole
#
# This file is part of Griff.
# 
# Griff is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 2 of the License, or (at your
# option) any later version.
# 
# Griff is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
# 
# You should have received a copy of the GNU General Public License
# along with Griff; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


require 'interps'
require 'formal_context'
require 'default_set'
require 'ds'

CHAR_CROSS = 'x'[0]
CHAR_DOT = '.'[0]

class FormalContextAndInterps
  attr_reader :context, :object_interp, :attribute_interp, :title
  attr_writer :context, :object_interp, :attribute_interp, :title

  def initialize(context, object_interp, attribute_interp, title="")
    @context = context
    @object_interp = object_interp
    @attribute_interp = attribute_interp
    @title = title
  end
end

def load_formal_context_from_cxt_file(file)

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

  context = FormalContext.new
  g_names = Array.new
  m_names = Array.new

  for i in 1..num_g do
    g_names[i] = file.gets.chomp
    context.insert_object i
  end

  for i in 1..num_m do
    m_names[i] = file.gets.chomp
    context.insert_attribute i
  end

  for g in 1..num_g do
    s = file.gets.chomp
    for m in 1..num_m do
      c = s[m-1]
      if c == CHAR_CROSS then
        context.insert_pair(g, m)
      else
        if c != CHAR_DOT then
          raise SyntaxError, "Expected 'x' or '.' on " + 
            "line #{file.lineno}, line='#{s}', c='#{c}'"
        end
      end
    end
  end

  object_interp = ArrayInterp.new(g_names)
  attribute_interp = ArrayInterp.new(m_names)
  return FormalContextAndInterps.new(
    context, object_interp, attribute_interp, title
  )
end

def load_formal_context_from_cxt_filename(filename)
  result = nil
  File.open(filename) { |file| result = load_formal_context_from_cxt_file(file) }
  return result
end


def write_formal_context_to_cxt_file(interp_context, file)

  context = interp_context.context
  object_interp = interp_context.object_interp
  attribute_interp = interp_context.attribute_interp
  title = interp_context.title

  file.puts("B")
  file.puts(interp_context.title)
  file.puts(context.objects.count.to_s)
  file.puts(context.attributes.count.to_s)
  for x in context.objects do
    file.puts(object_interp.full_name(x))
  end
  for x in context.attributes do
    file.puts(attribute_interp.full_name(x))
  end
  for g in context.objects do
    l = ""
    for m in context.attributes do
      if context.object_intent(g).include?(m) then
        l << "x"
      else
        l << "."
      end
    end
    file.puts(l)
  end

end

def write_formal_context_to_cxt_filename(interp_context, filename)
  File.open(filename, "w") do
    |file|
    write_formal_context_to_cxt_file(interp_context, file)
  end
end

def write_set_as_ds(basename, set, interp)
  File.open(basename + ".set", "w") { |file|
    for g in set do
      file.puts interp.full_name(g)
    end
  }
end

def write_set_as_ds_rep(base_name, domain_name, range_name, set, interp)
  File.open(base_name + ".rep", "w") { |file|
    file.puts "range=" + range_name
    file.puts "domain=" + domain_name
    for g in set do
      ys = interp.indexes(g)
      if ys.count == 0 then
        file.puts "-"
      else
        file.puts ys.join(" ")
      end
    end
  }
end

def write_formal_context_bit_as_ds(
  basename, objects_name, attributes_name, context
)
  File.open(basename + '.context', "w") { |file|
    file.puts("objects=" + objects_name)
    file.puts("attributes=" + attributes_name)
    for g in context.objects do
      intent = context.object_intent(g)
      if intent.count == 0 then
        file.puts "-"
      else
        file.puts intent.indexes.join(" ")
      end
    end
  }
end

def write_clarrified_context_as_ds(base_name, interp_context)
  write_formal_context_bit_as_ds(
    base_name + "_clarrified", 
    base_name + "_clarrified.objects.rep", 
    base_name + ".attributes.set", 
    interp_context.context
  )
  write_set_as_ds(
    base_name + ".objects",
    interp_context.object_interp.objects,
    interp_context.object_interp.object_interp
  )
  write_set_as_ds_rep(
    base_name + "_clarrified.objects",
    base_name + "_clarrified.context",
    base_name + ".objects.set",
    interp_context.context.objects, 
    interp_context.object_interp
  )
  write_set_as_ds(
    base_name + ".attributes", 
    interp_context.context.attributes,
    interp_context.attribute_interp
  )
end

def write_formal_context_as_ds(base_name, interp_context)
  write_formal_context_bit_as_ds(
    base_name, 
    base_name + ".objects.set", 
    base_name + ".attributes.set", 
    interp_context.context
  )
  write_set_as_ds(
    base_name + ".objects", 
    interp_context.context.objects, 
    interp_context.object_interp
  )
  write_set_as_ds(
    base_name + ".attributes", interp_context.context.attributes,
    interp_context.attribute_interp
  )
end

def load_set_from_ds(set_name, loader=nil)
  loader = DS.create_cache if loader == nil
  ds_set = loader[set_name]
  interp = ArrayInterp.new(ds_set.members)
  set = make_default_set
  for i in 1..ds_set.members.size do
    set.add(i)
  end
  return set, interp
end

def load_formal_context_from_ds(context_name, loader=nil)
  loader = DS.create_cache if loader == nil
  ds_context = loader[context_name]
  ds_objects = loader[ds_context.objects]
  ds_attributes = loader[ds_context.attributes]

  objects, object_interp = 
    load_set_from_ds(ds_context.objects, loader)
  attributes, attribute_interp = 
    load_set_from_ds(ds_context.attributes, loader)
  
  context = FormalContext.new
  context.replace_sets(
    objects,
    attributes
  )

  for x in 1..ds_objects.members.size do
    for y in ds_context.map[x] do
      context.insert_pair(x, y)
    end
  end

  FormalContextAndInterps.new(
    context, object_interp, attribute_interp
  );
end

def load_clarrified_context_from_ds(context_name, loader=nil)
  loader = DS.create_cache if loader == nil
  ds_context = loader[context_name]
  ds_objects = loader[ds_context.objects]
  ds_attributes = loader[ds_context.attributes]

  # Load the original objects
  org_objects, org_object_interp = load_set_from_ds(ds_objects.range, loader)
  puts "org_objects=#{org_objects}, names=#{org_objects.map{|x| org_object_interp.full_name(x)}.join(",")}"

  # load the clarrification relation
  clarrify_map = make_default_power_map(org_objects.lex)

  for x in 1..ds_objects.map.count do
    for y in ds_objects.map[x] do
      clarrify_map[x].bit_set.set(y, 1)
    end
  end

  clarrified_interp = ClarifyInterpFromDS.new(
    clarrify_map, org_object_interp, org_objects
  )

  attributes, attribute_interp = 
    load_set_from_ds(ds_context.attributes, loader)
  objects = make_default_set

  context = FormalContext.new
  context.replace_sets(
    objects, attributes
  )

  for x in 1..ds_objects.map.size do
    intent = make_default_set(attributes.lex)
    for y in ds_context.map[x] do
      intent.bit_set.set(y, 1)
    end
    context.insert_object(x)
    for y in intent do
      context.insert_pair(x, y)
    end
  end

  return FormalContextAndInterps.new(
    context, clarrified_interp, attribute_interp
  );
end

    
      
      
  
