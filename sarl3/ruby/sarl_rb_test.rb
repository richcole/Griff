require 'sarl'
require 'test_assert'

include Sarl

bset = BitSet.new
bset.set(1)

test_assert(bset.is_set(1) == true)
test_assert(bset.is_set(2) == false)

bset.clear(1)

test_assert(bset.is_set(1) == false)
test_assert(bset.is_set(2) == false)

test_assert(bset.hash == 0)

bset.set(1)
bset.set(19)

bset2 = bset.dup
bset3 = bset.and(bset2)

test_assert(bset.count == 2)
test_assert(bset2.count == 2)
test_assert(bset3.count == 2)

bset.set(33)

test_assert(bset.count == 3)
test_assert(bset2.count == 2)
test_assert(bset3.count == 2)

bset3 = bset.and(bset2)
bset4 = bset2.or(bset)

test_assert(bset3.count == 2)
test_assert(bset4.count == 3)

bset4.and_equals(bset2)
test_assert(bset2.count == 2)
test_assert(bset4.count == 2)

bset3.or_equals(bset)
test_assert(bset3.count == 3)

test_assert(bset3.is_subseteq(bset3))
test_assert(bset3.is_subseteq(bset))
test_assert(! bset3.is_subseteq(bset4))
test_assert(bset4.is_subseteq(bset3))
test_assert(! bset3.is_subset(bset3))
test_assert(! bset3.is_subset(bset))
test_assert(! bset3.is_subset(bset4))

context = Context.new
context.insert(1,1)
context.insert(2,2)
context.insert(3,3)

lattice = Lattice.new(context)
test_assert(lattice.count == 5)

st = StringTable.new
st.set(1, "one")
st.set(2, "two")
st.set(3, "three")

test_assert(st.get(1) == "one")
test_assert(st.get(2) == "two")
test_assert(st.get(3) == "three")

diagram = Diagram.create_b3()

st.each { |string| puts "string=#{string}" }

st[4] = "four"

st.each { |string| puts "string=#{string}" }
