 
# Copyright 2004, Richard Cole
#
# This file is part of Griff.
# 
# Griff is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 2 of the License, or (at your
# option) any later version.
# 
# Griff is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
# 
# You should have received a copy of the GNU General Public License
# along with Griff; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


require 'ripper'

class RipperTest < Ripper
   
  def on__alias(a, b)
    a
  end

  def on__alias_error(a)
    a
  end

  def on__aref(a, b)
    a
  end

  def on__aref_field(a, b)
    a
  end

  def on__arg_ambiguous()
    nil
  end

  def on__arg_paren(a)
    a
  end

  def on__arglist_add(a, b)
    a
  end

  def on__arglist_add_block(a, b)
    a
  end

  def on__arglist_add_star(a, b)
    a
  end

  def on__arglist_new()
    nil
  end

  def on__arglist_prepend(a, b)
    a
  end

  def on__array(a)
    a
  end

  def on__assign(a, b)
    a
  end

  def on__assign_error(a)
    a
  end

  def on__assoc_new(a, b)
    a
  end

  def on__assoclist_from_args(a)
    a
  end

  def on__bare_assoc_hash(a)
    a
  end

  def on__begin(a)
    a
  end

  def on__binary(a, b, c)
    a
  end

  def on__block_var(a)
    a
  end

  def on__bodystmt(a, b, c, d)
    a
  end

  def on__brace_block(a, b)
    a
  end

  def on__break(a)
    a
  end

  def on__call(object, method, args)
    puts "Call: object=[#{object}], method=[#{method}], args=[#{args}]"
    object
  end

  def on__case(a, b)
    a
  end

  def on__class(name, method, parents)
    puts "Class: namet=[#{name}], dunno=[#{method}], parents=[#{parents}]"
    name
  end

  def on__class_name_error(a)
    a
  end

  def on__command(a, b)
    a
  end

  def on__command_call(a, b, c, d)
    a
  end

  def on__const_ref(a)
    puts "Const Ref: #{a}"
  end

  def on__constpath_field(a, b)
    a
  end

  def on__constpath_ref(a, b)
    a
  end

  def on__def(a, b, c, d, e)
    puts "Def: [#{a}], [#{b}], [#{c}]"
    a
  end

  def on__defined(a)
    puts "Defined: #{a}"
    a
  end

  def on__defs(a, b, c, d, e)
    a
  end

  def on__do_block(a, b)
    a
  end

  def on__dot2(a, b)
    a
  end

  def on__dot3(a, b)
    a
  end

  def on__dyna_symbol(a)
    a
  end

  def on__else(a)
    a
  end

  def on__elsif(a, b, c)
    a
  end

  def on__ensure(a, b)
    a
  end

  def on__fcall(func, args)
    puts "Function Call: #{func}"
    func
  end

  def on__field(a, b, c)
    a
  end

  def on__for(a, b, c)
    a
  end

  def on__hash(a)
    a
  end

  def on__if(a, b, c)
    a
  end

  def on__if_mod(a, b)
    a
  end

  def on__ifop(a, b, c)
    a
  end

  def on__iter_block(a, b)
    a
  end

  def on__massign(a, b)
    puts "Assign: #{a}, #{b}"
    a
  end

  def on__method_add_arg(a, b)
    a
  end

  def on__mlhs_add(a, b)
    a
  end

  def on__mlhs_add_star(a, b)
    a
  end

  def on__mlhs_new()
    nil
  end

  def on__mlhs_paren(a)
    a
  end

  def on__module(a, b)
    a
  end

  def on__mrhs_add(a, b)
    a
  end

  def on__mrhs_add_star(a, b)
    a
  end

  def on__mrhs_new()
    nil
  end

  def on__mrhs_new_from_arglist(a)
    a
  end

  def on__next(a)
    a
  end

  def on__opassign(a, b, c)
    a
  end

  def on__param_error(a)
    a
  end

  def on__params(a, b, c, d)
    a
  end

  def on__paren(a)
    a
  end

  def on__parse_error(a)
    a
  end

  def on__program(a)
    a
  end

  def on__qwords_add(a, b)
    a
  end

  def on__qwords_new()
    nil
  end

  def on__redo()
    nil
  end

  def on__regexp_literal(a)
    a
  end

  def on__rescue(a, b)
    a
  end

  def on__rescue_mod(a, b)
    a
  end

  def on__restparam(a)
    a
  end

  def on__retry()
    nil
  end

  def on__return(a)
    a
  end

  def on__return0()
    nil
  end

  def on__sclass(a, b)
    a
  end

  def on__space(a)
    a
  end

  def on__stmts_add(a, b)
    a
  end

  def on__stmts_new()
    nil
  end

  def on__string_add(a, b)
    a
  end

  def on__string_concat(a, b)
    a
  end

  def on__string_content()
    nil
  end

  def on__string_dvar(a)
    a
  end

  def on__string_embexpr(a)
    a
  end

  def on__string_literal(a)
    a
  end

  def on__super(a)
    a
  end

  def on__symbol(a)
    a
  end

  def on__symbol_literal(a)
    a
  end

  def on__topconst_field(a)
    a
  end

  def on__topconst_ref(a)
    a
  end

  def on__unary(a, b)
    a
  end

  def on__undef(a)
    a
  end

  def on__unless(a, b, c)
    a
  end

  def on__unless_mod(a, b)
    a
  end

  def on__until_mod(a, b)
    a
  end

  def on__var_alias(a, b)
    a
  end

  def on__var_field(a)
    a
  end

  def on__var_ref(a)
    a
  end

  def on__void_stmt()
    nil
  end

  def on__when(a, b, c)
    a
  end

  def on__while(a, b)
    a
  end

  def on__while_mod(a, b)
    a
  end

  def on__word_add(a, b)
    a
  end

  def on__word_new()
    nil
  end

  def on__words_add(a, b)
    a
  end

  def on__words_new()
    nil
  end

  def on__xstring_add(a, b)
    a
  end

  def on__xstring_literal(a)
    a
  end

  def on__xstring_new()
    nil
  end

  def on__yield(a)
    a
  end

  def on__yield0()
    nil
  end

  def on__zsuper()
    nil
  end

end

for fname in ARGV do 
  file = File.new fname; 
  file_contents = file.read()
  begin
    puts "Parsing Ripper: #{fname}"
    Ripper.parse(file_contents)
    puts "Parsing RipperTest: #{fname}"
    RipperTest.parse(file_contents)
    puts "Finished: #{fname}"
  rescue
    StandardError => err
      puts "Raise Argument Error"
  end
end
