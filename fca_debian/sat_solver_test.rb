require 'sat_solver'

solver = Solver.new
b = Time.new
result = solver.solve("packages_fixed.cnf")
e = Time.new

puts "Time: " + (e - b).to_s + " Result: " + result.to_s
