require 'scanf'
require 'lexicon'
require 'ds'
require 'formal_context'
require 'formal_context_formats'

def read_judgements(judgements_filename, loader, lexicon, context)

  base_name = nil
  if judgements_filename =~ /(.*?)-judgements.txt/ then
    base_name = $1
  else
    raise "Invalid judgements_filename: '#{judgements_filename}'"
  end

  File.open(judgements_filename) do
    |file|
    file.readlines.each do
      |line|
      line.chomp!
      if line != "" then
        diagram_number, score = line.scanf("%d%d")
        ranks_name = "#{base_name}-rank.xml##{base_name}-#{diagram_number}.stats"
        diagram_name = "#{base_name}-diagrams.xml##{base_name}-#{diagram_number}.diagram"
        ranks = loader[ranks_name]

        if score > 1 then 
          context.insert_pair(diagram_name, "good")
        else 
          context.insert_pair(diagram_name, "bad")
        end

        for stat, value in ranks.fields do
          if value.to_i == 1 then
            context.insert_pair(diagram_name, stat)
          end
        end
      end
    end
  end

end

loader = DS::create_cache
lexicon = Lexicon.new

context = FormalContext.new

ARGV.each do
  |judgements_filename|
  read_judgements(judgements_filename, loader, lexicon, context)
end

interp = IdentInterp.new
interp_context = FormalContextAndInterps.new(context, interp, interp)
write_formal_context_to_cxt_file(interp_context, $stdout)

    
    
