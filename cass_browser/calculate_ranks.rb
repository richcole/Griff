require 'ds'
require 'formal_context'
require 'formal_context_formats'
require 'interps'

if ARGV.length != 3 then
  $stderr.puts "Usage: calculate_ranks <stats_xml_filename> <context_filename> <rank_filename>"
  exit
end

stats_xml_filename = ARGV[0]
context_filename = ARGV[1]
rank_xml_filename = ARGV[2]

class RankScores

  attr_reader :scores

  def initialize
    @scores = Hash.new { |h,k| h[k] = [nil,nil] }
    @op = {
      'reflections' => :gte,
      'edgeLength' => :leq,
      'pathWidth' => :leq,
      'vectors' => :leq,
      'meetVectors' => :leq,
      'chains' => :gte,
      'edgeCrossing' => :leq,
      'symChildren' => :gte,
      'symChildrenNZ' => :gte,
      'wellPlaced' => :gte,
      'okPlaced' => :gte,
      'ballance' => :leq,
      'numThreeChains' => :gte,
      'planar' => :gte,
      'numThreeChains' => :gte,
      'horizontalShift' => :leq,
      'num_unique_gradients' => :leq,
      'num_unique_gradients_abs' => :leq
   }
  end

  def is_better_or_equal(value, curr, op)
    case op
    when :gte then 
      return (curr == nil) || (value >= curr)
    when :leq then 
      return (curr == nil) || (value <= curr)
    end
    raise "Program Error: invalid operation op=#{op}"
  end

  def add(stats)
    for field, value in stats.fields do
      value = value.to_f
      if is_better_or_equal(value, @scores[field][0], @op[field]) then 
        if @scores[field][0] != value then
#          puts "field=#{field}, op=#{@op[field]} replace #{@scores[field][0]} with #{value}"
          if scores[field][0] then
#            puts "  result=#{value >= scores[field][0]},#{value == scores[field][0]}"
          end

          @scores[field][1] = @scores[field][0]
          @scores[field][0] = value
        end
      else 
        if is_better_or_equal(value, @scores[field][1], @op[field]) then
          if @scores[field][1] != value then
            @scores[field][1] = value
          end
        end
      end
    end
  end

  def rank_of(field, value)
    value = value.to_f
    if @scores[field][0] == value then 
      return 1 
    end
    if @scores[field][1] == value then 
      return 2 
    end
    return 3
  end
  
end

loader = DS::create_cache
scores = RankScores.new
max_planar = 0

# iterate collecting the scores first
doc = loader.get_document(stats_xml_filename)
doc.elements.each("ds/*") do 
  |element|
  
  if element.name =~ /.stats$/ then
    stats_filename = element.name
    stats = loader[stats_xml_filename + "#" + stats_filename]
    max_planar = max_of(max_planar, stats.fields['planar'].to_i) 
  end
end

doc = loader.get_document(stats_xml_filename)
doc.elements.each("ds/*") do 
  |element|
  
  if element.name =~ /.stats$/ then
    stats_filename = element.name
    stats = loader[stats_xml_filename + "#" + stats_filename]
    if ! (max_planar == 1 && stats.fields['planar'] != '1') then 
      scores.add(stats)
    end
  end
end

context = FormalContext.new

File.open(rank_xml_filename, "w") {
  |rank_xml_file|
  rank_xml_file.puts "<ds>"

  # iterate collecting the ranks
  doc.elements.each("ds/*") do
    |element|
    
    if element.name =~ /.stats$/ then
      stats_filename = element.name
      stats = loader[stats_xml_filename + "#" + stats_filename]
      if ! (max_planar == 1 && stats.fields['planar'] != '1') then 
        rank_xml_file.write "<#{stats_filename}>"
        for field, value in stats.fields
          rank = scores.rank_of(field, value)
          rank_xml_file.puts "#{field}=#{rank}"
          context.insert_object(stats_filename)
          #      printf("%10s  %10s  %6.2f  %4d  %4f  %4f",
          #        stats_filename, field, value.to_f, rank, 
          #        scores.scores[field][0].to_f, scores.scores[field][1].to_f
          #      )
          #      puts "#{scores.scores[0]}\t#{scores.scores[1]}"
          if rank <= 1 then
            context.insert_pair(stats_filename, field + " <= 1")
          end
          if rank <= 2 then
            context.insert_pair(stats_filename, field + " <= 2")
          end
        end
        rank_xml_file.write "</#{stats_filename}>"
      end
    end
  end
  rank_xml_file.puts "</ds>"
}

interp = IdentInterp.new
interp_context = FormalContextAndInterps.new(context, interp, interp)
interp_context.title = context_filename

File.open(context_filename, "w") { 
  |file|
  write_formal_context_to_cxt_file(interp_context, file)
}




