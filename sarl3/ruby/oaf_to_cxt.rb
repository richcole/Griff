require 'sarl'
require 'lexicon'
require 'cxt_file_format'

include Sarl

object_lex = Lexicon.new
attr_lex = Lexicon.new
context = Context.new

neg = BitSet.new
pos = BitSet.new

if ARGV.size != 3 then
  puts "Usage: oaf_to_cxt.rb <input.oaf> <vars.lex> <output.cxt>"
  exit -1
end

clause_lex = Lexicon.new

File.readlines(ARGV[1]).each {
  |line|
  if line.chomp =~ /\s*([0-9]+)\s*(.*)/ then
    clause_lex.set_word($2,$1.to_i)
  end
}

File.readlines(ARGV[0]).each {
  |line|
  if line.chomp =~ /([^:]+):(.*)/ then
    g_name = $1
    attributes = $2.split(/\s+/)

    g_name = g_name.split(/\s+/).map { 
      |x|
      xval = x.to_i
      if xval < 0 then 
        "NOT (" + clause_lex.word(-xval) + ")" 
      else 
        clause_lex.word(xval)
      end
    }.join(", ")
    g = object_lex.index(g_name)
    for m_name in attributes do
      if m_name.size > 0 then
        m_val = m_name.to_i
        if m_val < 0 then
          m = attr_lex.index("NOT (" + clause_lex.word(-m_val) + ")")
          neg.set(-m_val);
        else
          m = attr_lex.index(clause_lex.word(m_val))
          pos.set(m_val);
        end
        context.insert(m,g)
      end
    end
  else
    puts "Invalid line: " + line
  end
}

both = neg.and(pos)
both_ms = BitSet.new

puts "neg.count=" + neg.count.to_s
puts "pos.count=" + pos.count.to_s
puts "both.count=" + both.count.to_s

count = 0
both.each { 
  |m_val| 
  count += 1
  if count <= 10 then
    both_ms.set(attr_lex.index("NOT (" + clause_lex.word(m_val) + ")"))
    both_ms.set(attr_lex.index(clause_lex.word(m_val)))
  end
}

puts "count=" + count.to_s

st_objects = StringTable.new
st_attributes = StringTable.new

def fill_table(lex, st)
  for x in lex.indexes
    st[x] = lex.word(x)
  end
end

fill_table(object_lex, st_objects)
fill_table(attr_lex, st_attributes)
ic = InterpContext.new(context.object_subcontext(both_ms), st_attributes, st_objects)

interp_context_to_cxt(ic.dual, ARGV[2])
