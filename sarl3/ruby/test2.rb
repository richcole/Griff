require 'sarl'

include Sarl

def test_assert(x)
  if ! x then
    puts "Assertion Failed. value=#{x}"
    raise AssertionFailed
  end
end

bset = BitSet.new
bset.set(1)


