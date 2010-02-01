require 'sarl'
require 'lexicon'
require 'cxt_file_format'

include Sarl

clause_lex = Lexicon.new

File.readlines("packages.lex").each {
  |line|
  if line.chomp =~ /\s*([0-9]+)\s*(.*)/ then
    clause_lex.set_word($2,$1.to_i)
  end
}

context = Context.new
g = 1

File.readlines("packages-conflict.txt").each {
  |line|
  attributes = line.gsub!(/[^0-9 ]+/,'').split
  for m in attributes do
    context.insert(g,m.to_i)
  end
  g+=1
}

def fill_table(lex, st)
  for x in lex.indexes
    puts "x=#{x}, word=#{lex.word(x)}"
    st[x] = lex.word(x)
  end
end

st_objects = StringTable.new
st_attributes = StringTable.new
st_objects.populate(1, g)
fill_table(clause_lex, st_attributes)
ic = InterpContext.new(context, st_objects, st_attributes)
interp_context_to_cxt(ic, "packages-conflict.cxt")



  
