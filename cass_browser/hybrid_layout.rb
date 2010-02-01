require 'concept_lattice'
require 'formal_context_formats'
require 'util'

class HybridLayout

  attr_reader :rank, :lattice

  # initialize(lattice: concept_lattice) : void
  def initialize(lattice)
    @lattice = lattice

    # @rank: lattice -> int
    #   rank[x] = max_{p in lattice.upper_covers[x]} rank(p) + 1
    @rank = Hash.new

    @parent_num = Hash.new { |h,k| h[k] = 0 }
    @child_num = Hash.new { |h,k| h[k] = 0 }
    @num_children = Hash.new { |h,k| h[k] = 0 }
    @pos_count = Hash.new { |h,k| h[k] = 0 }

    @pos_x = Hash.new
    @vect_x = Hash.new

    calculate_rank
    calculate_irreducibles
    place_concepts
  end

  def calculate_rank
    for c in @lattice.topo do
      max_parent_rank = 0
      for p in @lattice.upper_covers[c] do
        max_parent_rank = max_of(max_parent_rank, @rank[p])
      end
      @rank[c] = max_parent_rank + 1
    end
  end

  # calculate the irreducibles elements of x
  def calculate_irreducibles
    @irreducibles = make_default_set(@lattice.index_lex)
    for x in @lattice do
      if @lattice.upper_covers[x].count() == 1 then
        @irreducibles.add(x)
      end
    end
  end

  # calculate the number of children per irreducible
  def calculate_num_children
    for x in @lattice do
      for c in @lattice.lower_covers[x] do
        if @irreducibles.contain?(c) then
          @num_children[x] += 1
        end
      end
    end
  end

  def place_concept(c)
    if @irreducibles.include?(c) then
      parent = @lattice.upper_covers[c].first
      @parent_num[parent] += 1
      @child_num[c] = @parent_num[parent]
      @vect_x[c] = vector_of(c, parent)
    end

    @pos_x[c] = pos_x_of(c)

    if record_pos(c) > 1 then
      backtracking_fixup(c)
    end
  end

  def place_concepts
    for x in @lattice.topo do
      place_concept(x)
    end
  end

  def vector_of(c, p)
    if (@num_children[p] % 2) == 0 then
      i = @child_num[c]
    else
      i = @child_num[c] - 1
    end
    if ( i == 0 ) then
      return 0
    else
      if ( i % 2 ) == 1 then
        return (i / 2).to_i
      else
        return - (i / 2).to_i
      end
    end
  end

  def pos_x_of(c)
    result = 0
    for x in @lattice.upset[c] do
      if @irreducibles.include?(x) then
        result += @vect_x[x]
      end
    end
    return result
  end

  def record_pos(c)
    p = [@pos_x[c], @rank[c]]
    @pos_count[p] += 1
    puts "record: c=#{c}, pos_x[c]=#{@pos_x[c]}, rank[c]=#{@rank[c]}, pos_count=#{@pos_count[p]}"
    return @pos_count[p]
  end

  def unrecord_pos(c)
    p = [@pos_x[c], @rank[c]]
    @pos_count[p] -= 1
    puts "unrecord: c=#{c}, pos_x[c]=#{@pos_x[c]}, rank[c]=#{@rank[c]}, pos_count=#{@pos_count[p]}"
    return @pos_count[p]
  end    

  def pos_of(c)
    return [@pos_x[c], @rank[c]]
  end

  def backtracking_fixup(c)
    ms = Array.new

    if @irreducibles.include?(c) then
      ms.push(c)
    end
      
    for m in @irreducibles.and(@lattice.upset[c]) do
      ms.push m
    end

    ms.reverse!

    dist = 1
    num_overlaps = 1

    while num_overlaps > 0 && dist < 8 do
      tmp_ms = ms.dup
      m = tmp_ms.pop
      while num_overlaps > 0 && m != nil do
        old_child_num = @child_num[m]
        num_overlaps = fixup(c, m, dist)
        if num_overlaps > 1 then
          unfixup(c, m, dist, old_child_num)
          m = tmp_ms.pop
        end
      end
      dist += 1
    end

    if ( num_overlaps > 0 ) then
      puts "Number of overlaps: #{num_overlaps}"
    end

    puts "Backtracking fixup finished."
  end

  def fixup(c, m, dist)

    puts "Fixup: c=#{c}, m=#{m}, dist=#{dist}"

    parent = @lattice.upper_covers[m].first
    @parent_num[parent] += dist

    puts "  parent=#{parent}, parent_num=#{@parent_num[parent]}"

    @child_num[m] = @parent_num[parent]
    @vect_x[m] = vector_of(m, parent)

    puts "  vect_x[m]=#{@vect_x[m]}"

    for x in @lattice.topo do
      unrecord_pos(x)
      if x == c then
        break
      end
    end

    num_overlaps = 0
    for x in @lattice.topo do
      @pos_x[x] = pos_x_of(x)
      if record_pos(x) > 1 then
        num_overlaps += 1
      end
      if x == c then
        break
      end
    end

    return num_overlaps
  end
      
  def unfixup(c, m, dist, old_child_num)

    puts "UnFixup: c=#{c}, dist=#{dist}, m=#{m}, child_num=#{old_child_num}"

    parent = @lattice.upper_covers[m].first
    @parent_num[parent] -= dist
    @child_num[m] = old_child_num
    @vect_x[m] = vector_of(m, parent)

    for x in @lattice.topo do
      unrecord_pos(x)
      if x == c then
        break
      end
    end

    num_overlaps = 0
    for x in @lattice.topo do
      @pos_x[x] = pos_x_of(x)
      if record_pos(x) > 1 then
        num_overlaps += 1
      end
      if x == c then
        break
      end
    end

    return num_overlaps
  end

end

def minus_x(set, x)
  return_value = set.dup
  return_value.remove(x)
  return return_value
end

  

  
