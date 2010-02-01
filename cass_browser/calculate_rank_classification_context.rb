require 'scanf'
require 'lexicon'
require 'ds'
require 'formal_context'
require 'formal_context_formats'
require 'interps'

def read_judgements(judgements_filename, loader, cxt1, cxt2)

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
        stats = loader["#{base_name}-rank.xml##{base_name}-#{diagram_number}.stats"]
        diagram_name = "#{base_name}-#{diagram_number}"

        if score == 2 then
          cxt1.insert_pair(diagram_name, "Good")
          cxt1.insert_pair(diagram_name, "OK")
        end
        if score == 1 then
          cxt1.insert_pair(diagram_name, "OK")
        end
        if score <= -1 then
          cxt1.insert_pair(diagram_name, "Bad")
        end
        if score <= -2 then
          cxt1.insert_pair(diagram_name, "Ugly")
          cxt1.insert_pair(diagram_name, "Bad")
        end

        for stat, value in stats.fields do
          if value.to_i <= 1 then
            cxt2.insert_pair(diagram_name, "#{stat} <= 1")
          end
          if value.to_i <= 2 then
            cxt2.insert_pair(diagram_name, "#{stat} <= 2")
          end
        end
      end
    end
  end
end

loader = DS::create_cache
cxt1 = FormalContext.new
cxt2 = FormalContext.new
interp = IdentInterp.new

ARGV.each do
  |judgements_filename|
  read_judgements(judgements_filename, loader, cxt1, cxt2)
end

interp_context1 = FormalContextAndInterps.new(cxt1, interp, interp)
interp_context2 = FormalContextAndInterps.new(cxt2, interp, interp)
write_formal_context_to_cxt_filename(interp_context1, "1.cxt")
write_formal_context_to_cxt_filename(interp_context2, "2.cxt")



    
    
