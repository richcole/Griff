require 'rexml/document'
require 'stringio'
require 'griff/geom'

class Class
  def attr_filename( *fnames )
    for fname in fnames do 
      class_eval %{
        def #{fname}
          return path + @fields['#{fname}']
        end
      }
    end
  end
end

module DS

  def create_cache
    return Fetcher.new
  end

  module_function :create_cache

  class Fetcher < Hash

    def initialize
      super() { |h,k| h[k] = h.load(k) }
      @xml_hash = Hash.new { |h,k| h[k] = load_document(k) }
      @store = Hash.new { |h,k| h[k] = Hash.new }
    end

    def get_document(k)
      return @xml_hash[k]
    end

    def load(k)
      case k
      when /\.rep$/ then 
        open(k) { |file| return Rep.new(k, file) }
      when /\.set$/ then 
        open(k) { |file| return Set.new(k, file) }
      when /\.lattice$/ then 
        open(k) { |file| return Lattice.new(k, file) }
      when /\.diagram$/ then 
        open(k) { |file| return Diagram.new(k, file) }
      when /\.context$/ then 
        open(k) { |file| return Context.new(k, file) }
      when /\.stats$/ then 
        open(k) { |file| return Stats.new(k, file) }
      else 
        throw "Unknown File Type #{k}"
      end
      puts "CANNOT GET TO HERE"
    end

    def open(filename)
      if filename =~ /(.*?)#(.*)/ then
        xml_filename = $1
        xml_path = "ds/" + $2
        xml_name = $2
        get_document(xml_filename)
        result = @store[xml_filename][xml_name] 
        if result
          yield(StringIO.new(result) )
        else
          raise "Element not found: #{filename}"
        end
      else
        File.open(filename) { |file| yield(file) }
      end
    end

    private
    def load_document(xml_filename)
      File.open(xml_filename) { |file|
        doc = REXML::Document.new(file)
        doc.elements.each('ds/*') { 
          |element|
          @store[xml_filename][element.name] = element.text
        }
        return doc
      }
      return nil
    end

  end

  class OneBasedArrayWrapper
    def initialize arr
      @arr = arr
    end

    def [](index)
      return @arr[index-1]
    end

    def size
      return @arr.size
    end

    def count
      return @arr.size
    end

    def each(&block)
      @arr.each(&block)
    end

    include Enumerable
  end

  class PathHolder
    def initialize(filename)
      @filename = filename
    end

    def path
      if @filename =~ /(.*[\/#])/ then
        return $1
      end
      return ""
    end
  end

  class Base < PathHolder

    attr_reader :map, :fields
    
    def path
      if @filename =~ /(.*[\/#])/ then
        return $1
      end
      return ""
    end

    def initialize(filename, file=nil)
      super(filename)

      @map_arr = []
      @map = OneBasedArrayWrapper.new(@map_arr)
      @fields = {}

      load_from_file(file) if file
    end

    def load_from_file(file)
      is_attr_value = true
      for line in file.readlines do
        if is_attr_value then
          if line =~ /(.+)=(.+)/ then
            add_field($1, $2)
          else
            is_attr_value = false
            @count = 1
            add_mapping(@count, line.chomp)
          end
        else
          @count += 1
          add_mapping(@count, line.chomp)
        end
      end
    end

    def devolve(str)
      if str == "-" then
        return []
      else
        return str.split(" ").map { |x| x.to_i }
      end
    end

    def add_field(name, value)
      eval "@#{name}=value"
      @fields[name] = value
    end

    def add_mapping(index, line)
      xs = devolve(line)
      @map_arr.push(xs)
    end

  end

  class Stats < Base
    def add_mapping(index, line)
    end
  end

  class Set < PathHolder
    attr_reader :members

    def initialize(filename, file=nil)
      super(filename)
      members = nil
      if file then
        members = file.readlines.map { |x| x.chomp }
        @members = OneBasedArrayWrapper.new(members)
      else
        @members = OneBasedArrayWrapper.new([])
      end
    end
  end

  class Rep < Base
    attr_filename :domain, :range
  end

  class Context < Base
    attr_filename :objects, :attributes
  end

  class Diagram < Base
    attr_filename :lattice
    attr_reader :points

    def initialize(*args)
      @points = [nil]
      super(*args)
    end

    def add_mapping(index, line)
      ps = line.split(" ").map { |x| x.to_f }
      points.push(Griff::Point.new(ps[0], ps[1]))
    end
  end

  class Lattice < Base
    attr_filename :context, :extent, :intent, :upset, :downset 
    attr_filename :uppercovers, :lowercovers
  end

end 

