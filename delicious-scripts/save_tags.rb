require 'rubygems'
require_gem 'Rubilicious'
require 'rubilicious_extensions'
require 'tag_utils'
require 'yaml'

auth = File.read("#{ENV["HOME"]}/.delicious/auth").chomp.split(":")
r    = Rubilicious.new(auth[0], auth[1])

File.open("tags.txt", "w") do
  |file| file << r.all_posts.to_yaml
end



  
      
