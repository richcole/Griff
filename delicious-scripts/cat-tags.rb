require 'set'
require 'rubygems'
require_gem 'Rubilicious'
require 'rubilicious_extensions'
require 'tag_utils'

auth = File.read("#{ENV["HOME"]}/.delicious/auth").chomp.split(":")
r    = Rubilicious.new(auth[0], auth[1])

alltags = Set.new
categories = Set.new
parent = tag_parents(r, categories, alltags)
children = tag_children(parent)

# construct bundles

if false then
  for x in categories
    puts "SetBundle: #{x} Tags: #{children[x].to_a.join(" ")}"
    r.set_bundle(x, children[x].to_a.join(" "))
  end
end

for t in alltags do
  for p in r.full_posts(t) do
    url = p['href']
    desc = p['description']
    puts "#{url}"
    puts "  tag: #{t}"
    puts "  tags: #{p['tag'].join(" ")}"
    puts "  closure: #{parent_closure(t, parent, p['tag']).join(" ")}"
    tags = (parent_closure(t, parent, p['tag']) << "all").join(" ")
    r.add(url, desc, nil, tags)
    sleep(1)
  end
end
  




