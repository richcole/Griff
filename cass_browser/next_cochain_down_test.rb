
require 'next_cochain_down'
require 'formal_context_formats'

context_filename = ARGV[0..-2]
intent_strings = ARGV[-1].split(" ")

ARGV[0..-2].each {
  |context_filename|
  File.open(context_filename) {
    |file|
    interp_context = load_formal_context_from_cxt_file(file)
    intent = make_default_set(interp_context.context.attributes.lex)
    for x in interp_context.context.attributes do
      for s in intent_strings do
        xn = interp_context.attribute_interp.full_name(x)
        if s.match(xn) then
          intent.add(x)
        end
      end
    end
    ia = implied_attributes(interp_context.context, intent)
    nc = next_cochain_down(interp_context.context, ia)
    ia_s = ia.to_a.map { |m| interp_context.attribute_interp.full_name(m) }.join(",")
    puts "Intent = #{ia} = " + ia_s
    puts "Number of intents: #{nc}"
    nc.each do 
      |intent|
      puts(intent.to_a.map { |m| interp_context.attribute_interp.full_name(m) }.join(","))
    end
  }
}



  
  
