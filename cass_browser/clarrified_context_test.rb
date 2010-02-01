require 'formal_context_formats'
require 'clarrified_context'


interp_context = nil
File.open('b3.cxt') { |file| 
  interp_context = load_formal_context_from_cxt_file(file)
}

clarrified_interp_context = calc_clarrified_context(interp_context)
write_clarrified_context_as_ds("2/b3", clarrified_interp_context)
clarrified_interp_context = 
  load_clarrified_context_from_ds("2/b3_clarrified.context")
write_clarrified_context_as_ds("3/b3", clarrified_interp_context)
