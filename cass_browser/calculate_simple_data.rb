require 'scanf'
require 'lexicon'
require 'ds'

def read_judgements(judgements_filename, loader, lexicon)

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
        
        output_line = ""
        output_line << "#{base_name}-#{diagram_number} "
        output_line << score.to_s + " "

        for stat, value in stats.fields do
          lexicon.index(stat)
        end

        for x in lexicon.indexes do
          output_line << stats.fields[lexicon.word(x)] << " "
        end
        puts output_line
      end
    end
  end

end

loader = DS::create_cache
lexicon = Lexicon.new

# puts "# Starting"

ARGV.each do
  |judgements_filename|
  # puts "# Filename=#{judgements_filename}"
  read_judgements(judgements_filename, loader, lexicon)
end

# puts "# Finished"

    
    
