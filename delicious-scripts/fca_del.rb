#!/bin/ruby
#
# Package Tags
#
#   - parse the available file to extract package information
#   - place the information in a tree view
#

require 'yaml'
require 'set'
require 'sarl'
require 'sarlgtk'
require 'lexicon'
require 'gtk2'
require 'tag_utils'

auth = File.read("#{ENV["HOME"]}/.delicious/auth").chomp.split(":")

Gtk.init

hpane = Gtk::HPaned.new
vpane = Gtk::VPaned.new
swindow = Gtk::ScrolledWindow.new
window = Gtk::Window.new
treestore = Gtk::TreeStore.new(String, String, FalseClass)
tree = Gtk::TreeView.new(treestore)

class TagView 
  attr_reader :context, :diagram, :tag_st, :package_st, :lattice_view, :lexicon
  
  def initialize 
    @context = Sarl::Context.new
    @tag_st = Sarl::StringTable.new
    @package_st = Sarl::StringTable.new
    @diagram = Sarl::Diagram.layout(@context)
    @lattice_view = SarlGtk::LatticeView.new(@diagram, @package_st, @tag_st)
    @tag_lexicon = Lexicon.new
    @package_lexicon = Lexicon.new
  end

  # tag is an array of parts
  def add_tag(tag, facets, packages)
    m = @tag_lexicon.index(tag)
    @context.insert_attribute(m)
    @tag_st[m] = tag
    for p in facets[tag].packages do
      g = @package_lexicon.index(p['description'])
      @package_st[g] = p['description']
      for t in p['tag'] do
        m = @tag_lexicon.index(t)
        if @context.attributes.is_set(m) then
          @context.insert(g, m)
        end
      end
    end
    facets[tag].item[2] = true
    @diagram = Sarl::Diagram.layout(@context)
    @lattice_view.set_diagram(@diagram, @package_st, @tag_st)
  end

  def clear
    for g in @context.objects do
      @context.remove_object(g)
      puts "removing object #{g} from context"
    end 
    for m in @context.attributes do
      @context.remove_attribute(m)
      puts "removing attribute #{m} from context"
    end
    @diagram = Sarl::Diagram.layout(@context)
    @lattice_view.set_diagram(@diagram, @package_st, @tag_st)
  end


  def remove_tag(tag, facets, packages)
    m = @tag_lexicon.index(tag)
    @context.remove_attribute(m)
    
    for g in @context.objects do
      if @context.object_intent(g).count == 0 then
        @context.remove_object(g)
      end
    end

    @diagram = Sarl::Diagram.layout(@context)
    @lattice_view.set_diagram(@diagram, @package_st, @tag_st)
  end

end

class Node
  attr_accessor :children, :name, :packages, :item
  def initialize
    @children = Hash.new { |h,k| h[k] = Node.new }
    @name = ""
    @packages = []
    @item = nil
  end
end

pages = YAML::load(File.read("tags.txt"))
parent = {}

tags = Set.new
for p in pages do
  for t in p['tag'] do
    tags.add(t)
  end
end
  
for t in tags
  tagarray = t.split("::")
  prev_tag = nil
  for i in 0..(tagarray.size-1) do
    tag = tagarray[0..i].join("::")
    if prev_tag != nil then
      parent[tag] = prev_tag
    end
    prev_tag = tag
  end
end

for p in pages do
  cl = Set.new
  for t in p['tag']
    parent_closure(t, parent, cl)
  end
  p['tag'] = cl.to_a
end

children = tag_children(parent)  

def parse_tags(pages, parent, children)
  i = 1
  facets = Hash.new { |h,k| h[k] = Node.new }
  packages = { }

  for p in pages do
    for t in p['tag'] do
      facets[t].packages << p
    end
  end

  for p, cs in children do
    facets[p].children = cs.map { |c| facets[c] }
  end

  return [facets, packages]
end

facets, packages = parse_tags(pages, parent, children)

puts "... Creating Tree Items ..."

def create_node(t, parent, facets, treestore)
  item = nil
  if facets[t].item != nil then
    return facets[t].item
  end
  if parent.keys.include?(t) then
    parent_item = facets[parent[t]].item
    if parent_item == nil then
      parent_item = create_node(parent[t], parent, facets, treestore)
    end
    item = treestore.append(parent_item)
    item[0] = t
    item[1] = t
    item[2] = false
  else
    item = treestore.append(nil)
    item[0] = t
    item[1] = t
    item[2] = false
    facets[t].item = item
  end
  facets[t].item = item
  return item
end

for facet, node in facets do
  if node.item == nil then
    create_node(facet, parent, facets, treestore)
  end
end

treestore.set_sort_column_id(1)

renderer = Gtk::CellRendererText.new
renderer.weight = Pango::FontDescription::WEIGHT_BOLD
column = Gtk::TreeViewColumn.new(
  "Facet", renderer, { :text => 0, :weight_set => 2})
tree.append_column(column)

puts "... Running Main ..."

tag_view = TagView.new 

# a button to clear everything
clearbutton = Gtk::Button.new("Clear")

# connect signal
clearbutton.signal_connect("clicked") {
  # iterate through tree and set third column to false
  treestore.each do |model, path, iter|
    if iter[2] then
      puts "removing #{path.to_str}"
      iter[2] = false
    end
  end
  # clear lattice
  tag_view.clear
}

window.add(hpane)
hpane.add(swindow)
hpane.add(vpane)
vpane.add(clearbutton)
vpane.add(tag_view.lattice_view)
swindow.add(tree)
window.set_default_size(300,300)
window.show_all
window.signal_connect("destroy") { Gtk.main_quit }


# connect to tree view signal
tree.signal_connect("row-activated") do |view, path, column|
  puts "Row #{path.to_str} was clicked!"
  if iter = tree.model.get_iter(path)
    puts "Double-clicked row contains name #{iter[0]}!"
    iter[2] = ! iter[2]
    if iter[2] then 
      puts "Calling Add Tag: #{iter[1]}"
      tag_view.add_tag(iter[1], facets, packages)
    else
      puts "Calling Remove Tag: #{iter[1]}"
      tag_view.remove_tag(iter[1], facets, packages)
    end
  end
end

Gtk.main



