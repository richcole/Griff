require 'redcloth'

r = RedCloth.new($stdin.read)
puts r.to_html
