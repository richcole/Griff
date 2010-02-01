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
    clause_lex.set_word("NOT " + $2,- ($1.to_i))
  end
}

File.readlines(ARGV[0]).each {
  |line|
  if line.chomp =~ /([^:]+):(.*)/ then
    my_object = $1
    my_attributes = $2
    attributes = my_attributes.split(/\s+/)
    g_name = clause_lex.word(my_object.to_i)
    puts "attributes = #{attributes}, g_name=#{g_name}"
    g = object_lex.index(g_name)
    for attr in attributes do
      if attr != "" then
        m_name = clause_lex.word(attr.to_i)
        puts "attr=#{attr}, m_name=#{m_name}"
        m = attr_lex.index(m_name)
        context.insert(g, m)
      end
    end
  end
}

def fill_table(lex, st)
  for x in lex.indexes
    puts "x=#{x}, word=#{lex.word(x)}"
    st[x] = lex.word(x)
  end
end

st_objects = StringTable.new
st_attributes = StringTable.new
fill_table(object_lex, st_objects)
fill_table(attr_lex, st_attributes)
ic = InterpContext.new(context, st_objects, st_attributes)
interp_context_to_cxt(ic, ARGV[2])
