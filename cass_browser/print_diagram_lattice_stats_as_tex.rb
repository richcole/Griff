require 'set'

# fields = Set.new
hash = Hash.new { |h,k| h[k] = Hash.new { |h,k| h[k] = Hash.new } }
fields = 'num_diagrams', 'num_distinct_diagram', 'classes'

readlines.each {
  |line|
  values = line.split
  
  hash[values[0]][values[1]] = values[2]
#  fields.add(values[1])
}

puts "Lattice Name & " + fields.to_a.join(" & ") + " \\\\ \\hline"
for k,v in hash do

  a = []
  for f in fields do
    a.push(v[f])
  end
  puts "#{k} & #{a.join(" & ")} \\\\"

end
