require 'rjb'

class Solver
  include Rjb

  def initialize(jar_path = '/home/rcole/install/sat4j.jar')
    load(jar_path)
    @solverFactoryClass = import('org.sat4j.minisat.SolverFactory')
    @instanceReaderClass = import('org.sat4j.reader.InstanceReader')
  end

  def solve(filename)
    result = false
    begin
      solver = @solverFactoryClass.newMiniSAT();
      solver.setTimeout(3600)
      reader = @instanceReaderClass.new(solver)
      reader.parseInstance(filename)
      result = solver.isSatisfiable
    rescue Exception => e
      puts "Exception Raised: " + e.class.to_s + " : " + e.to_s
    end
    return result
  end
end
