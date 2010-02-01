#!/bin/ruby
#
# create_makefile.rb: copyright Richard Cole Jan 2006. 
#   See LICENCE for details.


module Enumerable 
  def suffix(suffix)
    self.map { |x| x + suffix }.join(" ")
  end
end

class CreateMakefile

  attr_accessor :outp, :ccc, :cflags, :ldflags
  attr_accessor :module_sources, :binary_sources

  def initialize
    @outp = File.open("build.mak", "w")

    @ccc     = "g++"
    @cflags  = "-ggdb"
    @ldflags = ""

    @coco    = "coco"

    @module_sources = %w(scanner parser Scanner Parser)
    @binary_sources = %w(test_scanner test_parser RJLC)
    @coco_sources   = %w(RJL)
  
    @sources = @module_sources + @binary_sources
    @namespace = "RJL"
  end

  def puts(*args)
    outp.puts(*args)
  end

  def coco_atg(source, ns)
    puts "Scanner.cpp Scanner.h Parser.cpp Parser.h: #{source}.atg"
    puts "\t#{@coco} -namespace #{ns} #{source}.atg"
    puts ""
  end

  def compile_object(source)
    puts "#{source}.o: #{source}.cpp"
    puts "\t#{ccc} #{cflags} -c -o $@ $<"
    puts ""
  end
  
  def depends_object(source)
    puts "#{source}.dep: #{source}.cpp"
    puts "\t#{ccc} #{cflags} -M -o $@ $<"
    puts ""
    puts "include #{source}.dep"
  end
  
  def link_exe(source)
    puts "#{source}.exe: #{source}.o #{module_sources.suffix(".o")}"
    puts "\t#{ccc} #{cflags} #{ldflags} -o $@ $< #{module_sources.suffix(".o")}"
    puts ""
  end
  
  def do
    puts "all: #{binary_sources.suffix(".exe")}"
    puts ""

    @sources.each { |source| 
      compile_object(source) 
      depends_object(source)
    }

    @binary_sources.each { |source| 
      link_exe(source) 
    }

    @coco_sources.each { |source|
      coco_atg(source, @namespace)
    }
  end
end

cm = CreateMakefile.new
cm.do
