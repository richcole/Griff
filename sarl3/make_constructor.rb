def default_new(basename, fields)
  typename = basename + "_t" 

  arg_list = []
  for f in fields do
    case f[2] 
      when "arg"
        arg_list << "  " + f[0] + "_t* " + f[1]
    end
  end
  
  init_list = []
  for f in fields do
    case f[2] 
    when "arg"
      init_list << "  cc->" + f[1] + " = " + f[1] + ";"
    when "new"
      init_list << "  cc->" + f[1] + " = " + f[0] + "_default_new();"
    end
  end
  
  if arg_list.size > 0 then
    args = "\n" + arg_list.join(",\n") + "\n"
  else
    args = ""
  end

  if init_list.size > 0 then
    inits = "\n" + init_list.join("\n") + "\n"
  else
    inits = ""
  end
  
  <<-EOF
inline #{typename}* #{basename}_new(#{args})
{
  #{typename} *cc = (#{typename} *)
    GC_MALLOC(sizeof(#{typename}));
  sarl_return_zero_error_if_zero(cc);
  memset(cc, sizeof(*cc), 0);
#{inits}
  return cc;
};
EOF
end

basename = "sarl_concept_interp"

fields = [
  [ "sarl_contintent_interp", "object_interp", "nil" ],
  [ "sarl_contintent_interp", "attribute_interp", "nil" ],
  [ "sarl_bit_set",           "realised_concepts", "nil" ]
]

puts default_new(basename, fields)


basename = "sarl_contingent_interp"

fields = [
  [ "sarl_string_table", "names", "nil" ],
  [ "sarl_power_map", "contingents", "nil" ],
]

puts "\n\n"
puts default_new(basename, fields)


basename = "sarl_lattice_view_interp"

fields = [
  [ "sarl_array", "labels", "new" ],
  [ "sarl_concept_interp", "interp", "arg" ],
]

puts "\n\n"
puts default_new(basename, fields)

basename = "sarl_inverted_lists"

fields = [
  [ "sarl_array", "lists", "new" ],
]

puts "\n\n"
puts default_new(basename, fields)

basename = "sarl_implication_set"

fields = [
  [ "sarl_power_set", "premice", "new" ],
  [ "sarl_power_map", "conclusions", "new" ]
]

puts "\n\n"
puts default_new(basename, fields)




