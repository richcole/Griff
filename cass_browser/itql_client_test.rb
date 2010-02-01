require 'itql_client'
require 'util'

itql = ItqlClient.new
rs = itql.query(contents_of_file('/home/rcole/kowari/example.itql'))

for hash in rs do
  for var, val in hash do
    puts "var=#{var}, val=#{val}"
  end
end


