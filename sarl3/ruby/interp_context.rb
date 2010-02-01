require 'sarl'

module Sarl
  class InterpContext
    attr_accessor :context, :st_objects, :st_attributes, :title
    
    def initialize(context, st_objects, st_attributes, title="")
      @context = context
      @st_objects = st_objects
      @st_attributes = st_attributes
      @title = title
    end

    def dual
      new_cxt = Context.new
      for g in @context.objects do
        new_cxt.insert_attribute(g)
        for m in @context.object_intent(g) do
          new_cxt.insert(m,g)
        end
      end
      for m in @context.attributes do
        new_cxt.insert_object(m)
      end
      return InterpContext.new(new_cxt, st_attributes, st_objects)
    end
  end
end
