
require 'formal_context'
require 'diagram'
require 'concept_lattice'

class ContraOrdinalScale

  attr_reader :attr_order, :context, :lattice

  def initialize attr_order
    @attr_order = attr_order
    @context = FormalContext.new
    for m in @attr_order do
      @context.insert_object(m)
      @context.insert_attribute(m)
      for n in @attr_order do
        if m != n && ! @attr_order.upset[m].include?(n) then
          @context.insert_pair(n, m)
        end
      end
    end
  end

end


