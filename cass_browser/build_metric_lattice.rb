require 'ds'
require 'lexicon'
require 'set'

require 'formal_context_formats'
require 'formal_context'
require 'interps'

include DS

pattern = ARGV[0]
loader = DS::create_cache()
regex = /(^.*?)-[0-9]+/


lex = Lexicon.new

values = Hash.new { |h,k| h[k]=Hash.new {|h,k| h[k]=Set.new } }
rank = Hash.new { |h,k| h[k]=Hash.new { |h,k| h[k]=Hash.new } }

dirn = { 
  "NumberOfChains" => :gte,
  "EdgeCrossing" => :leq,
  "UniqueEdgeGradients" => :leq,
  "SymmetricSiblings" => :gte,
  "Symmetry" => :gte,
  "AveragePathWidth" => :leq,
  "TotalEdgeLength" => :leq,
  "SymmetryPlus" => :gte
}

def is_better(value, test_value, dir)
  case dir
    when :gte then return test_value >= value
    when :leq then return test_value <= value
  else
    raise "Program Error, invalid dir=#{dir}"
  end
end

Dir.glob(pattern) { |filename|
  stats = loader[filename]
  diagram = filename.gsub(/^stats\//,'')
  if diagram =~ regex then
    diagram_name = $1
  else
    diagram_name = "Unknown"
  end
  for key, value in stats.fields do
    lex.index(key)
    values[diagram_name][key].add(value)
  end
}

for diagram, vals in values do 
  for key, set in vals do
    for value in set do
      count = 0
      for test_value in set do
        if is_better(value, test_value, dirn[key]) then
          count += 1
        end
      end
      rank[diagram][key][value] = count
    end
  end
end

puts <<-EOF
  CREATE TABLE Layout (
    diagram varchar(255) default NULL,
    name varchar(255) default NULL,
EOF

columns = lex.words.map { |x| "    " + x + " int(2) default NULL" }

puts columns.join(",\n")
puts "  );"

contexts = Hash.new { |h,k| h[k]=FormalContext.new }

Dir.glob(pattern) { |filename|
  stats = loader[filename]
  diagram = filename.gsub(/^stats\//,'')
  diagram_name = "Unknown"
  if diagram =~ regex then
    diagram_name = $1
  end
  data = []
  for key, value in stats.fields do
    data[lex.index(key)-1] = rank[diagram_name][key][value]
  end
  puts "INSERT INTO Layout VALUES ('#{diagram}','#{diagram_name}'," + data.join(",") + ");"

  contexts[diagram_name].insert_object(diagram)
  for key, value in stats.fields do
    for m in rank[diagram_name][key][value]..2 do
      scale_attribute = key + "<" + m.to_s
      contexts[diagram_name].insert_pair(diagram, scale_attribute)
    end
  end
}

interp = IdentInterp.new
for diagram_name, context in contexts do
  File.open(diagram_name + ".cxt", "w") { |file|
    write_formal_context_to_cxt_file(
      FormalContextAndInterps.new(context, interp, interp), file
    )
  }
end







