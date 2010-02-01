def min_of(x,y)
  if x != nil && x <= y then return x else return y end
end

def max_of(x,y)
  if x != nil && x >= y then return x else return y end
end

def contents_of_file(filename) 
  result = nil
  File.open(filename) { |file| return file.read() }
  return result
end

def count_in xs
  count = 0
  for x in xs do
    count += 1
  end
  return count
end

def time_block(text)
  before = Time.now
  yield
  after = Time.now
  puts "Time (" + text + "): " + (after - before).to_s
end

