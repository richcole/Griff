#!/usr/bin/ruby -I /home/rcole/Source/griff/sarl3/ruby

require 'sarl'
require 'lexicon'
require 'cxt_file_format'
require 'sat_solver'

include Sarl

class GenConflicts

  def initialize
    @num_clauses = 0
    @num_vars = 0
    @num_comments = 0
    
    @eol = "\n"            # end of line
    @eoc = " 0" + @eol      # end of clause
    
    @top_targets = []
    @clauses = []
    @sat_regexp = Regexp.new('s SATISFIABLE')
    @context = Context.new
    @attr_lex = Lexicon.new
    @obj_lex = Lexicon.new
    @cnf_lex = Lexicon.new
    @next_object = 1
    @count = 1
    @max_count = 100000

    @solver = Solver.new
  end

  def targets
    return @top_targets
  end

  def parse(filename)

    lines = File.readlines(filename)

    lines.each {
      |line|
      if line[0] == "c"[0] then
        @num_comments += 1
      elsif
        @num_clauses += 1
        clause = line.split(/\s+/).map { |x| x.to_i }
        clause.each { 
          |x| @num_vars = x.abs if x.abs > @num_vars
        }
        if clause.include?(-1) then # implications of the head clause
          @top_targets.push( clause.select { |x| x != -1 } )
        end
        if clause != [1] then # don't push the head clause
          @clauses.push(clause)
        end
      end
    }
  end

  def sat(targets, tmp_fname)
    File.open(tmp_fname, "w") {
      |file|
      nc = @num_clauses + targets.length - 1
      file.write "p cnf " + @num_vars.to_s + " " + nc.to_s + @eol
      targets.each { |t| file.write t.join(" ") + @eoc }
      @clauses.each { |c| file.write c.join(" ") + @eoc }
    }
    
    return @solver.solve(tmp_fname)
  end

  def c_to_s(nt) 
    return nt.map { |x| "[" + x.join(", ") + "]" }.join(",")
  end

  def search(ts, targets, k, tmp_fname)
    bound = [[ts, 1]]
    while (bound.size != 0) && (@max_count == -1 || @count < @max_count) do
      puts "bound.count=" + bound.size.to_s
      ts, k = bound.pop
      puts "TRACE: ts=#{c_to_s(ts)}, k=#{k}"
      for i in k..targets.length
        break if @max_count != -1 && @count > @max_count
        @count += 1
        t = targets[i-1]
        if ! ts.include?(t) then
          nt = ts.dup
          nt.push(t)

          bset = BitSet.new
          nt.each { |x| bset.set(@attr_lex.index(x)) }

          if @context.is_maximal_intent(bset) then
            if sat(nt, tmp_fname) then
              puts "SAT: " + c_to_s(nt)
              bound.push([nt, i+1]) 
            else
              nt.each { 
                |x| 
                @context.insert(@next_object, @attr_lex.index(x))
              }
              @next_object += 1
              puts "UNSAT: " + c_to_s(nt)
            end
          end
        end
      end
    end
  end

  def output_context(filename)
    st_objects = StringTable.new
    st_attributes = StringTable.new
    st_objects.populate(1, @context.objects.count)
    for m in @attr_lex.indexes do
      word = @attr_lex.word(m)
      m_name = word.map { |x| @cnf_lex.word(x) }.join(" | ")
      st_attributes[m] = m_name
    end
    ic = InterpContext.new(@context, st_objects, st_attributes)
    interp_context_to_cxt(ic, filename)
  end

  def parse_lexicon(filename)
    File.readlines(filename).each {
      |line|
      if line =~ /\s*([0-9]+)\s+(.*)/ then
        index = $1.to_i
        word = $2
        @cnf_lex.set_word(word, index)
      else
        puts "Unable to read lex line: '" + line + "'"
      end
    }
  end

end

if ARGV.length != 4 then
  puts "Usage: generate_conflict_lattice.rb " +
    "<input.cnf> <output.cnf> <lexicon> <output_context>"
  exit -3
end

gc = GenConflicts.new()

gc.parse_lexicon(ARGV[2])
gc.parse(ARGV[0])
puts "Up to here!!"
gc.search([], gc.targets, 1, ARGV[1])
gc.output_context(ARGV[3])




