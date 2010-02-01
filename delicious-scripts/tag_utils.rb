require 'set'

def tag_parents(categories, alltags)

  parent = {}
  for k,v in r.tags do
    tagarray = k.split("::")
    prev_tag = nil
    for i in 0..(tagarray.size-1) do
      tag = tagarray[0..i].join("::")
      alltags.add(tag)
      parent[tag] = prev_tag
      if prev_tag then 
        categories.add(prev_tag)
      end
      prev_tag = tag
    end
  end
  
  return parent
end

def tag_children(parent)
  children = Hash.new { |h,k| h[k] = Set.new }
  for c, p in parent do
    children[p].add(c)
  end
  return children
end

def parent_closure(t, parent, tags)
  tags << t
  while parent.keys.include?(t) do
    t = parent[t]
    tags << t
  end
  return tags
end

