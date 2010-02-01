module Sarl
  class InterpDiagram
    attr_reader :diagram, :st_objects, :st_attributes
    
    def initialize(diagram, st_objects, st_attributes)
      @diagram = diagram
      @st_objects = st_objects
      @st_attributes = st_attributes
    end
  end
end
