require 'rjb'
require 'uri/common.rb'

trap('SIGHUP') {}

class ItqlClient
  include Rjb

  def initialize(itql_jar_path = '/home/rcole/kowari/itql-1.1.0.jar')
    load(itql_jar_path)
    iTQLInterpreterBeanClass = import('org.kowari.itql.ItqlInterpreterBean')
    @itql_interp = iTQLInterpreterBeanClass.new
  end

  def query(queryString)
    puts "Query: " + queryString
    return ResultSet.new(@itql_interp.executeQuery(queryString))
  end

  def execute(queryString)
    puts "Query: " + queryString
    @itql_interp.executeQuery(queryString)
  end
end

class ResultSet
  attr_reader :at_end

  def initialize(answer)
    @answer = answer
    collect_variables
    self.next
  end

  def collect_variables
    vars = @answer.getVariables
    @var_index = Hash.new
    for i in 1..vars.length do
      @var_index[vars[i-1].getName()] = i-1
    end
  end

  def next
    @at_end = ! @answer.next
  end

  def reset
    @answer.beforeFirst
    self.next
  end

  def val_of_index(index)
    return URI.unescape(@answer.getObject(index).toString).gsub(/^gr:/,'')
  end

  def val(variable_name)
    val_of_index(@var_index[variable_name])
  end

  def each
    reset
    while ! self.at_end do
      h = Hash.new
      for name,index in @var_index do
        h[name] = val_of_index(index)
      end
      yield(h)
      self.next
    end
  end
    
end
