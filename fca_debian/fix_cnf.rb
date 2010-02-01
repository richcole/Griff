lines = File.readlines(ARGV[0])

num_clauses = 0
num_vars = 0
num_comments = 0

eol = "\n"

lines.each {
  |line|
  if line[0] == "c"[0] then
    num_comments += 1
  elsif
    num_clauses += 1
    for x in line.split(/\s+/) do
      y = x.to_i
      y = -y if y < 0 
      num_vars = y if y > num_vars 
    end
  end
}

File.open(ARGV[1], "w") {
  |file|
  file.write "p cnf " + num_vars.to_s + " " + num_clauses.to_s + eol
  lines.each {
    |line|
    if line[0] != "c"[0] then
      file.write line.chomp + ' 0' + eol
    end
  }
}
