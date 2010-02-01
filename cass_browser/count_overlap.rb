require 'set'

class IntSequence
  def initialize(arr)
    @arr = arr
  end

  def hash
    result = 0
    for i in 0..(@arr.size-1) do
      result = result ^ @arr[i]
    end
    return result
  end

  def eql?(other)
    return false if @arr.size != other.size
    for i in 0..(@arr.size-1) do
      if ! @arr[i].eql?(other[i]) then
        return false
      end
    end
    return true
  end

  def [](i)
    return @arr[i]
  end

  def size
    @arr.size
  end

  def to_s
    return '[' + @arr.join(",") + ']'
  end
    
end

class Set
  def to_s
    return '{' + self.to_a.join(",") + '}'
  end
end

label_hash = Hash.new { |h,k| h[k] = Set.new }
result_hash = Hash.new { |h,k| h[k] = Array.new }

ARGV.each {
  |filename|
  File.readlines(filename).each { 
    |line|
    nums = line.split(" ")
    puts "nums=#{nums.join(", ")}"
    name = nums[0]
    label = nums[1].to_i
    features = IntSequence.new(nums[2..-1].map { |x| x.to_i })
    label_hash[features].add(label)
    result_hash[features].push([name, label])
  }
}

for h,v in label_hash do
  if v.size > 1 && v.include?(2) then
    puts "Collision: #{h}, #{v}"
    for name, label in result_hash[h] do
      puts "  name=#{name}, label=#{label}"
    end
  end
end
  
