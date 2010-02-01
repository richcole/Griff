distrib = {}
half_distrib = {}
stats = Hash.new { |h,k| h[k] = Hash.new }

File.read("are_contexts_distributive.txt").each {
  |line|
  line.chomp!
  line.sub!('.cxt','')
  line_array = line.split(/ +/)
  distrib[line_array[0]] = line_array[1]
  half_distrib[line_array[0]] = line_array[2]
}

File.read("diagram_lattice_stats.txt").each {
  |line|
  line.chomp!
  line_array = line.split(/ +/)
  basename = line_array[0].sub('-diagrams.xml','')
  field = line_array[1]
  value = line_array[2]
  stats[basename][field] = value
}

File.readlines("basenames.txt").each { 
  |basename|
  if ! (basename =~ /-distrib/) then
    basename.chomp!
    good = File.readlines(basename + "-good_diagrams.txt").size
    best = File.readlines(basename + "-best_diagrams.txt").size
    puts [basename, distrib[basename], best, stats[basename]["classes"], good].join(" ")
  end
}

File.readlines("basenames.txt").each { 
  |basename|
  if (basename =~ /-distrib/) then
    basename.chomp!
    good = File.readlines(basename + "-good_diagrams.txt").size
    best = File.readlines(basename + "-best_diagrams.txt").size
    puts [basename, distrib[basename], best, stats[basename]["classes"], good].join(" ")
  end
}
