lines = File.readlines(ARGV[0])
output = []

n = ARGV[1].to_i

def swap(a, i, j) 
  tmp = a[i]
  a[i] = a[j]
  a[j] = tmp
end

def take(a, i)
  result = a[i]
  a[i] = a[a.length-1]
  a.pop
  return result
end

for i in 1..n do
  idx = rand * lines.length
  output.push(take(lines, idx))
end

puts output
