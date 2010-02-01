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

class Pig

  def initialize
    @pig_bin = "/usr/local/bin"
    @db = "/home/rcole/Sandpit/analyse_toscanaj_0_2.wiki/toscanaj"
    @print_cxt_exe = "#{@pig_bin}/pig_print_cxt2"
    @execute_action = "#{@pig_bin}/pig_execute_action"
    @retract_graph = "#{@pig_bin}/pig_retract_graph"
    @assert_graph = "#{@pig_bin}/pig_assert_graph"
    @query_graph = "#{@pig_bin}/pig_query_graph"
  end

  def quote s
    return "\"" + s.gsub(/"/,"\\\"") + "\""
  end

  def sys_command cmd
    puts "Exec: #{cmd}"
    result = ""
    ok = IO.popen(cmd, "r") { |inp| result = inp.read }
    if ! ok then
      puts "Error excuting command: #{cmd}"
    end
    return result
  end

  def sys_command_with_input cmd, input
    result = ""
    ok = IO.popen(cmd, "r+") { |inp| 
      puts input
      inp.write(input)
      inp.close_write
      result = inp.read 
    }
    if ! ok then
      puts "Command execution is not ok: cmd=#{cmd}, input=#{cmd}"
    end
    return result
  end

  def get_context(objs, attrs, incidence)
    cmd = "#{@print_cxt_exe} #{@db} #{quote(objs)} #{quote(attrs)} #{quote(incidence)}"
    puts "CMD: #{cmd}"
    result = nil
    ok = IO.popen(cmd) { |inp| 
      result = load_formal_context_from_cxt_file(inp)
      result.object_interp = 
        NavSpaceArrayInterp.new(
          result.object_interp, 
          lambda {|x| return self.is_object_unfolded(x)} 
        )
      result.attribute_interp = 
        NavSpaceArrayInterp.new(
          result.attribute_interp, 
          lambda {|x| return self.is_attribute_unfolded(x)} 
        )
    }
    if ! ok then
      puts "Error executing cmd, exception=#{exception}"
      puts "CMD="
      puts cmd
    end      
    return result
  end

  def execute_action(action)
    return sys_command_with_input("#{@execute_action} #{@db} -", action)
  end

  def query(graph)
    return sys_command("#{@query_graph} #{@db} #{quote(graph)}")
  end

  def retract(query_graph, retract_graph)
    cmd = "#{@retract_graph} #{@db} #{quote(query_graph)} #{quote(retract_graph)}"
    return sys_command(cmd)
  end

  def assert graph
    return sys_command("#{@assert_graph} #{@db} #{graph}")
  end  

end
    
