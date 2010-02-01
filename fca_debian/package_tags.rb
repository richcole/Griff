#!/bin/ruby
#
# Package Tags
#
#   - parse the available file to extract package information
#   - place the information in a tree view
#

require 'sarl'
require 'sarlgtk'
require 'lexicon'
require 'gtk2'
require 'find'

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
    node = nil
    if tag =~ /([^:]+)::(.+)/ then
      facet = $1
      value = $2
      if value =~ /([^:]+):(.+)/ then
        v1 = $1
        v2 = $2
        node = facets[facet].children[v1].children[v2]
      else
        node = facets[facet].children[value]
        puts "add_tag | Node: #{node.name}"
      end
    else
      node = facets[tag]
    end
    puts "add_tag | Tag: #{tag}, count=#{node.packages.size}"
    m = @tag_lexicon.index(tag)
    @tag_st[m] = tag
    for p in node.packages do
      g = @package_lexicon.index(p)
      @package_st[g] = p
      @context.insert(g, m)
      for pt in packages[p] do
        ptm = @tag_lexicon.index(pt)
        if @context.attributes.is_set(ptm) then
          puts "add_tag | Attribute: #{ptm} text=#{pt}"
          @tag_st[ptm] = pt
          @context.insert(g, ptm)
        end
      end
    end
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
  attr_accessor :children, :name, :packages
  def initialize
    @children = Hash.new { |h,k| h[k] = Node.new }
    @name = ""
    @packages = []
  end
end

def parse_available(file)
  i = 1
  facets = Hash.new { |h,k| h[k] = Node.new }
  packages = { }
  while ! file.eof do
    line = file.readline.chop
    if line =~ /\APackage:\s(.*)/ then
      package = $1
    end
    if line == "" then
      package = ""
      version = ""
    end
    if line =~ /\AVersion:\s(.*)/ then
      version = $1
    end
    if line =~ /\ATag:\s+(.*)/ then
      fullname = package + "-" + version
      tags = $1
      for tag in tags.split(/,\s+/) do
        tg = tag.split("::")
        facet = tg[0]
        value = tg[1]
        if value =~ /([^:]+):([^:]+)/ then
          pv = $1
          sv = $2
          node = facets[facet].children[pv].children[sv]
          node.name = tag
          node.packages << fullname
          facets[facet].packages << fullname
          facets[facet].children[pv].packages << fullname
          packages[fullname] = tags.split(/,\s+/)
        else
          node = facets[facet].children[value]
          node.name = tag
          node.packages << fullname
          facets[facet].packages << fullname
          packages[fullname] = tags.split(/,\s+/)
        end
      end
    end
    i += 1
  end

  return [facets, packages]
end

facets = nil
packages = nil
File.open("/var/lib/dpkg/available") { # /mnt/debian-mount
  |file| facets, packages = parse_available(file)
}

puts "... Creating Tree Items ..."

for facet, node in facets do
  facet_item = treestore.append(nil)
  facet_item[0] = facet
  facet_item[1] = facet
  facet_item[2] = false
  for v1, node in node.children do
    v1item = treestore.append(facet_item)
    v1item[0] = v1
    v1item[1] = facet + "::" + v1
    v1item[2] = false
    for v2, node in node.children do
      v2item = treestore.append(v1item)
      v2item[0] = v2
      v2item[1] = facet + "::" + v1 + ":" + v2
      v2item[2] = false
    end
  end
end

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



#tag_view.add_tag("game", facets, packages)

Gtk.main



