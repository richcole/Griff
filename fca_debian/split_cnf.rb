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
  File.open(ARGV[2], "w") {
    |file2|
    lines.each {
      |line|
      if line[0] != "c"[0] then
        clauses = line.split(/\s+/)
        # puts "CLAUSE: " + clauses.join(", ")
        if clauses.include?("-1") then
          clauses.delete("-1")
          file2.write clauses.join(" ") + "\n"
        elsif ! clauses.include?("1") then
          file.write clauses.join(" ") + "\n"
        end
      end
    }
  }
}
