require 'sarl'
include Sarl

context = Context.new

for m in context.methods.to_a.sort do
  puts m.to_s
end

if context.objects.count != 0 then
  puts "Error, execpeted a count of 0"
end
