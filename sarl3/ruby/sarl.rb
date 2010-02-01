require 'sarlrb'

module Enumerable
  def range
    return 0 .. (self.size - 1)
  end
end

module Sarl

  module WithJoin
    def to_s
      return "[" + self.to_a.join(", ") + "]"
    end

    def join(*args)
      return self.to_a.join(*args)
    end
  end

  class BitSet
    include Enumerable
    include WithJoin
  end

  class IntArray
    include Enumerable
    include WithJoin
  end

end



