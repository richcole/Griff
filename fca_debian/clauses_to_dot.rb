require 'set'

clauses_filename = ARGV[0]
lex_filename = ARGV[1]
mapping_filename = ARGV[2]
reasons_filename = ARGV[3]

clauses = File.readlines(clauses_filename).to_a.map {
  |line|
  line.chomp.split(/\s+/).select{|x| x.size > 0}.map { |x| x.to_i }
}

lex = {}
File.readlines(lex_filename).to_a.each {
  |line|
  if line =~ /\A([0-9]+)\s+(.*)/ then
    lex[$1.to_i]=$2
  end
}

mapping = {}
File.readlines(mapping_filename).to_a.each {
  |line|
  if line =~ /\A\s+([0-9]+)\s+([0-9]+)/ then
    mapping[$1.to_i]=$2.to_i
  end
}

reasons = {}
File.readlines(reasons_filename).to_a.each {
  |line|
  if line =~ /\A([^\s]+)\s+(.*)/
    reasons[$1]=$2
  end
}


def find_root(x, mapping)
  if mapping.keys.include?(x) then
    return find_root(mapping[x], mapping)
  else
    return x
  end
end

root = {}
for x, v in mapping do
  root[x] = find_root(v, mapping)
end

def clause_split(a)
  neg = a.select { |x| x < 0 }.map { |x| -x }
  pos = a.select { |x| x > 0 }
  return [neg, pos]
end

or_count = 1
uninst = Set.new


nodes = Set.new
puts "digraph {
"
for clause in clauses do
  neg, pos = clause_split(clause)
  neg.each { |n| nodes.add(n) }
  pos.each { |n| nodes.add(n) }
  if neg.size == 1 then
    if pos.size == 0 then
      res = "\"No match for:\\n #{reasons[lex[neg[0]]]}\""
      uninst.add(res)
      puts "#{neg[0]} -> " + res
    elsif pos.size == 1 then
      puts "#{neg[0]} -> #{pos[0]}"
    else
      puts "#{neg[0]} -> OR#{or_count}"
      for p in pos do
        puts "OR#{or_count} -> #{p}"
      end
      or_count += 1
    end
  else
    if pos.size == 1 && neg.size == 0 then
      puts "Install -> #{pos[0]}"
    elsif pos.size != 0 || neg.size != 2 then
      $stderr.puts "Error: can't draw this clause [#{clause.join(" ")}]"
    else
      puts "#{neg[0]} -> #{neg[1]} [arrowtail=normal,color=\"red\",label=\"NOT\",style=\"dashed\"]"
    end
  end
end

nodes.each { |n|

  child_nodes = []
  rn = root[n] ? root[n] : n
  for k, v in root do
    if v == rn then
      child_nodes.push k
    end
  end

  if child_nodes.length > 0 then
    puts "#{n} [label=\"#{lex[n]}\\n#{child_nodes.map{|x| lex[x]}.join("\\n")}\"]"
  else
    puts "#{n} [label=\"#{lex[n]}\"]"
  end
}

for i in uninst do
  puts i + " [shape=box]"
end

for i in 1..(or_count-1) do
  puts "OR#{i} [label=\"OR\",shape=diamond]"
end

puts "}"
