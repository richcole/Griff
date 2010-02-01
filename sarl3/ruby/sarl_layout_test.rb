require 'sarl'
require 'cxt_file_format'
require 'cxt_to_xml'

include Sarl

module Enumerable
  def range
    return 0..(size-1)
  end
end

def max_of(x,y)
  return (x != nil && x > y) ? x : y
end

ic = interp_context_from_cxt('../../flashview/contexts/stl.cxt')
metrics = []
diagrams = Diagram.layout_array(ic.context, metrics)

puts "diagrams.count = " + diagrams.size.to_s + " metrics.count = " + metrics.size.to_s

maxes = []
for m in metrics do
  puts "metrics: " + m.join(", ")
  for mi in m.range do
    maxes[mi] = max_of(maxes[mi], m[mi])
  end
end

diags = []
for i in diagrams.range do
  d = diagrams[i]
  m = metrics[i]
  name = "fig8-#{i+1}"
  File.open(name + ".xml", "w") { 
    |file|
    ismax = []
    score = 0
    for im in m.range do
      ismax[im] = (m[im] == maxes[im]) ? 1 : 0
      if ismax[im] == 1 then
        score += 1
      end
    end
    diags << [score, i]
    ic.title = name + ": " + ismax.join(", ")
    lattice_to_xml(ic, d, file)
  }
end

diags.sort! { |x,y| x[0] <=> y[0] }
puts "Diagram Ordering: ", diags.map { |x| x[1]+1 }.join(", ")
