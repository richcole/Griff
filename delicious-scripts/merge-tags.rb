require 'set'
require 'rubygems'
require_gem 'rubilicious'

auth = File.read("#{ENV["HOME"]}/.delicious/auth").chomp.split(":")
r    = Rubilicious.new(auth[0], auth[1])

t1   = "page-type::home-page"
t2   = "page-type::homepage"

# merge t2 into t1
t2_posts = r.full_posts(t2)
for x in t2_posts do
  puts "Merging: url=#{x['href']}"
  url = x['href']
  desc = x['description']
  tags = x['tag'].join(" ") << " " << t1
  r.add(url, desc, nil, tags)
end




