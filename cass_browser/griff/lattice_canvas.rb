
module Griff

  class LatticeCanvas < Canvas

    def interp_context=(interp_cxt)
      @interp_context = interp_cxt
      @lattice = ConceptLattice.new(@interp_context.context)
      @rep = MeetIrrRep.new(@lattice)
      @pos = AdditiveDiagram.new(@rep, @lattice)
      clear
      create_concept_items
      create_edges
      create_concept_labels
    end

    def create_concept_items
      for x in lattice do
        @concept_item_map[x] = CanvasConcept.new(self, x, @pos[x])
      end
    end

    def create_edges
      for x in lattice do
        for y in lattice.lower_covers[x] do
          CanvasLine.new(@canvas_item_map[x], @concept_item_map.[y])
        end
      end
    end

    def create_concept_labels
    end

  end

end
