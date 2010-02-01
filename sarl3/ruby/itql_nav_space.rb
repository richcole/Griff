 
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


require 'sarl'
require 'itql_client'
require 'lexicon'

# require 'util'

class ItqlNavSpace

  attr_reader :pig

  def set_relation(relation)
    @graph = "$g <gr:#{relation}> $m"
  end

  def initialize 
    @itql = ItqlClient.new
    @h = "<gr:h1>"
    @n = "<gr:n1>"
    @graph = "$g <gr:inherits-in> $m"
    @object_relation = "<gr:in>"
    @attribute_relation = "<gr:in>"
    prime
  end

  def context
    relation_query = <<-EOF
      select 
        $g $m  
      from  
        <rmi://localhost/server1#toscanaj> 
      where 
        #{@graph}  and
        $g <gr:is-member> <gr:h> and
        $m <gr:is-member> <gr:n>
      ;
    EOF

    object_query = <<-EOF
      select 
        $g  
      from  
        <rmi://localhost/server1#toscanaj> 
      where 
        $g <gr:is-member> <gr:h>
      ;
    EOF

    attribute_query = <<-EOF
      select 
        $m  
      from  
        <rmi://localhost/server1#toscanaj> 
      where #{@graph} 
        and $m <gr:is-member> <gr:n>
      ;
    EOF

    context = Context.new
    object_lex = Lexicon.new
    attr_lex = Lexicon.new

    object_st = StringTable.new
    attribute_st = StringTable.new
    
    @itql.query(relation_query).each { |hash| 
      context.insert(object_lex.index(hash['g']), attr_lex.index(hash['m']))
    }
    @itql.query(object_query).each { |hash| 
      context.insert_object(object_lex.index(hash['g']))
    }
    @itql.query(attribute_query).each { |hash| 
      context.insert_attribute(attr_lex.index(hash['m']))
    }

    for x in object_lex.indexes().to_a().sort() do
      object_st.push(object_lex.word(x))
    end
    for x in attr_lex.indexes().to_a().sort() do
      attribute_st.push(attr_lex.word(x))
    end

    return InterpContext.new(context, object_st, attribute_st)
  end

  def remove_objects
    query = <<-EOF
      delete
        select
          $g <gr:is-member> <gr:h> 
        from  
          <rmi://localhost/server1#toscanaj> 
        where 
          $g <gr:is-member> <gr:h>
      from  
        <rmi://localhost/server1#toscanaj> 
      ;
    EOF
    @itql.execute(query)
  end

  def remove_attributes
    query = <<-EOF
      delete
        select
          $m <gr:is-member> <gr:n> 
        from  
          <rmi://localhost/server1#toscanaj> 
        where 
          $m <gr:is-member> <gr:n>
      from  
        <rmi://localhost/server1#toscanaj> 
      ;
    EOF
    @itql.execute(query)
  end

  def add_relations
    query = <<-EOF
      insert <gr:is-member> <gr:is-a> <gr:relation> into
        <rmi://localhost/server1#toscanaj> 
      ;
    EOF
    @itql.execute(query)
  end

  def add_top_objects
    query = <<-EOF
     insert
        select
          $g <gr:is-member> <gr:h>
        from  
          <rmi://localhost/server1#toscanaj> 
        where 
          $g <gr:is-a> <gr:root-package>  
      into
        <rmi://localhost/server1#toscanaj> 
      ;
    EOF
    @itql.execute(query)
  end

  def add_top_attributes
    query = <<-EOF
      insert
        select
          $m <gr:is-member> <gr:n>
        from  
          <rmi://localhost/server1#toscanaj> 
        where 
          $m <gr:is-a> <gr:root-package>  
      into
        <rmi://localhost/server1#toscanaj> 
      ;
    EOF
    @itql.execute(query)
  end

  def focus_on_attribute m
    query = <<-EOF
      delete
        select
          $m <gr:is-member> <gr:n> 
        from  
          <rmi://localhost/server1#toscanaj> 
        where 
          $m <gr:is-member> <gr:n>
      from  
        <rmi://localhost/server1#toscanaj> 
      ;
      insert
        <gr:#{m}> <gr:is-member> <gr:n>
      into
        <rmi://localhost/server1#toscanaj> 
      ;
    EOF
    @itql.execute(query)
  end

  def focus_on_object g
    query = <<-EOF
      delete
        select
          $g <gr:is-member> <gr:h> 
        from  
          <rmi://localhost/server1#toscanaj> 
        where 
          $g <gr:is-member> <gr:h>
      from  
        <rmi://localhost/server1#toscanaj> 
      ;
      insert
        <gr:#{g}> <gr:is-member> <gr:h>
      into
        <rmi://localhost/server1#toscanaj> 
      ;
    EOF
    @itql.execute(query)
  end

  def unfold_object g
    query = <<-EOF
      insert
        select
          $h <gr:is-member> <gr:h>
        from  
          <rmi://localhost/server1#toscanaj> 
        where 
          $h #{@object_relation} <gr:#{g}>
      into
        <rmi://localhost/server1#toscanaj> 
      ;
    EOF
    @itql.execute(query)
  end

  def unfold_attribute m
    query = <<-EOF
      insert
        select
          $n <gr:is-member> <gr:n>
        from  
          <rmi://localhost/server1#toscanaj> 
        where 
          $n #{@attribute_relation} <gr:#{m}>
      into
        <rmi://localhost/server1#toscanaj> 
      ;
    EOF
    @itql.execute(query)
  end

  def fold_object g
    query = <<-EOF
      delete
        select
          $h <gr:is-member> <gr:h>
        from  
          <rmi://localhost/server1#toscanaj> 
        where 
          $h #{@object_relation} <gr:#{g}>
      from
        <rmi://localhost/server1#toscanaj> 
      ;
    EOF
    @itql.execute(query)
  end

  def fold_attribute m
    query = <<-EOF
      delete
        select
          $n <gr:is-member> <gr:n>
        from  
          <rmi://localhost/server1#toscanaj> 
        where 
          $n #{@attribute_relation} <gr:#{m}>
      from
        <rmi://localhost/server1#toscanaj> 
      ;
    EOF
    @itql.execute(query)
  end

  def focus_on_attributes ms
    query = <<-EOF
      delete
        select
          $n <gr:is-member> <gr:n>
        from  
          <rmi://localhost/server1#toscanaj> 
        where 
          $n <gr:is-member> <gr:n>
      from
        <rmi://localhost/server1#toscanaj> 
      ;
      insert 
         XXXX
      into
        <rmi://localhost/server1#toscanaj> 
      ;
    EOF
    clause = ""
    ms.each { 
      |m|
      clause << "<gr:#{m}> <gr:is-member> <gr:n>\n         "
      puts "Attribute: " + m.to_s
      puts "Clause: " + clause
    }

    query.sub!(/XXXX/,clause)
    @itql.execute(query)
  end

  def prime
    add_relations
    remove_objects
    remove_attributes
    add_top_attributes
    add_top_objects
  end

end

    
