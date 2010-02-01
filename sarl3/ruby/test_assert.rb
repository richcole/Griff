
def test_assert(x)
  if ! x then
    puts "Assertion Failed. value=#{x}"
    raise AssertionFailed
  end
end

