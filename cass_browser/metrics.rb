require 'lines'
require 'concept_lattice' 
require 'griff/geom'
require 'set'
require 'mathn'
require 'countable'
require 'util'
require 'power_map'


include Griff

def each_covering(lattice, diagram) 
  for a in lattice do
    for b in lattice.lower_covers[a] do
      yield(diagram.points[a], diagram.points[b])
    end
  end
end

def num_edge_crossings(lattice, diagram)
  edges = 0

  for x in lattice do
    for y in lattice.lower_covers[x] do
      for a in lattice do
        if a > x then
          for b in lattice.lower_covers[a] do
            if x != a && x != b && y != a && y != b then
              px = diagram.points[x]
              py = diagram.points[y]
              pa = diagram.points[a]
              pb = diagram.points[b]
              r = crossing(px.x, px.y, py.x, py.y, pa.x, pa.y, pb.x, pb.y)
              if r != nil then
                edges += 1
              end
            end
          end
        end
      end
    end
  end
  return edges
end

class PointSet
  def initialize
    @h = Hash.new do |h,k| h[k] = Set.new end
  end
  def add(p)
    @h[p.y].add(p.x)
  end
  def include?(p)
    return @h[p.y].include?(p.x)
  end

  def each
    for y, xs in @h do
      for x in xs do
        yield(Point.new(x,y))
      end
    end
  end

  def level(y)
    return @h[y]
  end

  def each_level
    for y, xs in @h do
      yield(y, xs)
    end
  end

  include Countable

end

def num_symmetric_children(lattice, diagram)
  sym_count = 0
  for x in lattice do
    points = PointSet.new
    px = diagram.points[x]
    for y in lattice.lower_covers[x] do
      py = diagram.points[y] - px
      points.add(py)
    end
    for p in points do
      if points.include?(Point.new(-p.x, p.y)) then
        sym_count += 1
      end
    end
  end

  return sym_count
end

def num_symmetric_children_nz(lattice, diagram)
  sym_count = 0
  for x in lattice do
    points = PointSet.new
    px = diagram.points[x]
    for y in lattice.lower_covers[x] do
      py = diagram.points[y] - px
      points.add(py) if py.x != 0
    end
    for p in points do
      if points.include?(Point.new(-p.x, p.y)) then
        sym_count += 1
      end
    end
  end

  return sym_count
end

def num_reflections(lattice, diagram)
  
  points = PointSet.new
  plane_count = Hash.new { |h,k| h[k] = 0 }
  planes = Set.new

  # collect all the points
  for a in lattice do
    pa = diagram.points[a]
    points.add(pa)
    planes.add(pa.x)
  end

  # for level wise pairs collect the sym plains
  points.each_level do |y, xs|
    xs.each do |ax|
      xs.each do |bx|
        if bx > ax then
          plane = (ax + bx) / 2
          if planes.include?(plane) then
            plane_count[plane] += 1
          end
        end
      end
    end
  end

  # boose each symetry line by the number of
  # points that are coindicent on it
  for x in lattice do
    plane = diagram.points[x].x    
    if plane_count[plane] > 0 then
      plane_count[plane] += 1
    end
  end

  #counts = []
  #for plane, count in plane_count do
  #  counts.push(count)
  #end
  #puts "counts=" + counts.join(", ")

  # sum logs of the counts
  result = 0
  for plane, count in plane_count do
    if count > 0 then
      result += Math.log(count)
    end
  end

  return result

end

def num_vectors(lattice, diagram)

  points = PointSet.new

  each_covering(lattice, diagram) do |pa, pb|
    points.add(pb - pa)
  end

  return points.count
end

def num_unique_gradients(lattice, diagram)

  grads = Set.new

  each_covering(lattice, diagram) do |pa, pb|
    grads.add((pa.x-pb.x).to_f/(pa.y-pb.y))
  end

  return grads.size

end

def num_unique_gradients_abs(lattice, diagram)

  grads = Set.new

  each_covering(lattice, diagram) do |pa, pb|
    grads.add(((pa.x-pb.x).to_f/(pa.y-pb.y)).abs)
  end

  return grads.size

end

def num_meet_vectors(lattice, diagram)

  points = PointSet.new

  for x in lattice do
    u = lattice.upper_covers[x]
    if u.count == 1 then
      points.add(diagram.points[x] - diagram.points[u.first])
    end
  end

  return points.count
end

#
# Count the number of times that a meet irreducible copies the
# vector of its parent.

def num_chains(lattice, diagram)
  chain_count = 0
  for x in lattice do
    if lattice.upper_covers[x].count == 1 then
      parent_x = lattice.upper_covers[x].first
      for child_x in lattice.lower_covers[x] do
        if lattice.upper_covers[child_x].count == 1 then
          parent_point = diagram.points[parent_x]
          x_point = diagram.points[x]
          child_point = diagram.points[child_x]
          dv = x_point - parent_point
          du = child_point - x_point
          if dv.x == du.x && dv.y == du.y then
            chain_count += 1
          end
        end
      end
    end
  end
  return chain_count
end

def num_three_chains(lattice, diagram)
  chain_count = 0
  for x in lattice do
    if lattice.upper_covers[x].count == 1 then
      parent = lattice.upper_covers[x].first
      if lattice.upper_covers[parent].count == 1 then
        granny = lattice.upper_covers[parent].first
        if lattice.upper_covers[granny].count == 1 then
          great_granny = lattice.upper_covers[granny].first
          dxp = diagram.points[parent] - diagram.points[x]
          dpg = diagram.points[granny] - diagram.points[parent]
          dpgg = diagram.points[great_granny] - diagram.points[granny]
          if dxp.eql?(dpg) && dpgg.eql?(dpg) then
            chain_count += 1
          end
        end
      end
    end
  end
  return chain_count
end

def total_edge_length(lattice, diagram)
  total = 0.0
  each_covering(lattice, diagram) do 
    |pa, pb|
    du = pb - pa
    total += Math.sqrt( 
      (du.x.to_f * du.x.to_f) + (du.y.to_f * du.y.to_f).to_f 
    )
  end
  return total
end

def each_path(lattice, diagram, xs, x, &block)
  if x == nil then
    raise "Argument Error"
  end
  xs.push(x)
  if lattice.lower_covers[x].count == 0 then
    yield(xs)
  else
    for y in lattice.lower_covers[x] do
      each_path(lattice, diagram, xs, y, &block)
    end
  end
  xs.pop
end

def ave_path_width(lattice, diagram)
  num_paths = 0
  total_path_width = 0
  each_path(lattice, diagram, [], lattice.top) {
    |path|
    min_x = nil
    max_x = nil
    for a in path do
      ax = diagram.points[a].x
      min_x = min_of(min_x, ax)
      max_x = max_of(max_x, ax)
    end
    num_paths += 1
    total_path_width += max_x - min_x
  }
  return total_path_width.to_f / num_paths.to_f
end

def is_same_diagram(lattice, d1, d2)
  h = Hash.new { |h,k| h[k]=Set.new }
  each_covering(lattice, d1) { |p,q|
    h[p].add(q)
  }
  each_covering(lattice, d2) { |p,q|
    if ! h[p].include?(q) then
      return false
    end
  }
  return true
end

def num_well_placed_children(lattice, diagram)

  count = 0

  for a in lattice do
    case lattice.lower_covers[a].size
    when 1 then
      mini_count = 0
      b = lattice.lower_covers[a].first
      dx = diagram.points[b] - diagram.points[a]
#      puts "dx=(#{dx.x},#{dx.y})"
      if dx.y == 50 && dx.x == 0 then
        mini_count += 1
      end
      if mini_count == 1 then
        count += 1
      end
    when 2 then
      mini_count = 0
      for b in lattice.lower_covers[a] do
        dx = diagram.points[b] - diagram.points[a]
        if dx.y == 50 && (dx.x == -50 || dx.x == 50) then
          mini_count += 1
        end
      end
      if mini_count == 2 then
        count += 1
      end
    when 3 then
      mini_count = 0
      for b in lattice.lower_covers[a]
        dx = diagram.points[b] - diagram.points[a]
        if dx.y == 50 && (dx.x == -50 || dx.x == 50 || dx.x == 0) then
          mini_count += 1
        end
      end
      if mini_count == 3 then
        count += 1
      end
    end
  end

  return count
end

def num_ok_placed_children(lattice, diagram)

  count = 0

  for a in lattice do
    if lattice.lower_covers[a].size >= 2 then
      mini_count = 0
      for b in lattice.lower_covers[a] do
        dx = diagram.points[b] - diagram.points[a]
        if dx.y == 50 && (dx.x == -50 || dx.x == 50) then
          mini_count += 1
        end
      end
      if mini_count == 2 then
        count += 1
      end
    end
  end

  return count
end

def children_ballance(lattice, diagram)
  result = 0
  for a in lattice do
    ballance = 0
    for b in lattice.lower_covers[a] do
      pa = diagram.points[a]
      pb = diagram.points[b]
      if pb.x > pa.x then
        ballance += 1
      else
        if pb.x < pa.x then
          ballance -= 1
        end
      end
    end
    result += ballance.abs
  end
  return result
end
          
def is_distributive(lattice)
  meet_irr = make_default_set(lattice.index_lex)
  reduced_intent = make_default_power_map(lattice.index_lex)
  for x in lattice do
    if lattice.upper_covers[x].count == 1 then
      meet_irr.add(x)
    end
  end
  for x in lattice do
    reduced_intent[x] = lattice.upset[x].and(meet_irr)
  end
  for x in lattice do
    for px in lattice.upper_covers[x] do
      if reduced_intent[x].minus(reduced_intent[px]).count > 1 then
        return false
      end
    end
  end
  return true
end

def is_distributive_half(lattice)
  meet_irr = make_default_set(lattice.index_lex)
  reduced_intent = make_default_power_map(lattice.index_lex)
  for x in lattice do
    if lattice.upper_covers[x].count == 1 then
      meet_irr.add(x)
    end
  end
  for x in lattice do
    reduced_intent[x] = lattice.upset[x].and(meet_irr)
  end
  for x in lattice do
    if lattice.lower_covers[x].count > 0 then
      for px in lattice.upper_covers[x] do
        if reduced_intent[x].minus(reduced_intent[px]).count > 1 then
          return false
        end
      end
    end
  end
  return true
end

def horizontal_shift(lattice,diagram)
  #(top.x - bottom.x).abs
  return (diagram.points[lattice.top].x - diagram.points[lattice.bottom].x).abs
end      

        
  
