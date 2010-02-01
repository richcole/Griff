require 'formal_context_formats'
require 'concept_lattice'
require 'diagram'
require 'transform'

require 'griff/canvas_concept'
require 'griff/canvas_line'
require 'griff/canvas_concept_label'

module Griff

class MoveManipController

  def initialize(lattice_controller)
    @lattice_controller = lattice_controller
  end

  def canvas
    return @lattice_controller.canvas
  end

  def concept_dragged(concept_item, dv)
    pos = @lattice_controller.pos
    pos.move_by(concept_item.concept, dv.x, dv.y)
  end

  def drag_start(item, p)
    item.drag_start(p)
    canvas.update
  end

  def drag_stop(item, p)
    item.drag_stop(p)
    @lattice_controller.update_transform
    canvas.update
  end

  def drag(item, dx)
    if item.kind_of?(CanvasConcept) then
      concept_dragged(item, dx)
    else
      item.drag(dx)
    end
    canvas.update
  end

  def mouse_enter(item)
    item.mouse_enter()
    canvas.update
  end

  def mouse_leave(item)
    item.mouse_leave()
    canvas.update
  end

end

class CanvasLatticeController

  attr_accessor :pos, :transform, :canvas

  def initialize (canvas_view, canvas)
    @canvas_view = canvas_view
    @canvas = canvas
    @move_manip_controller = MoveManipController.new(self)
    @canvas.controller.broker = @move_manip_controller
    @canvas_view.before_paint {
      calculate_concept_positions
    }
  end

  def load_context interp_context
    @canvas.remove_all_items

    @interp_context = interp_context
    @lattice   = ConceptLattice.new(@interp_context.context)
    @rep = MeetIrrRep.new(@lattice)
    @pos = AdditiveDiagram.new(@rep, @lattice)
    
    calculate_concepts(@canvas, @lattice, @pos)
    calculate_lines(@canvas, @lattice, @concept_item_map)
    calculate_attribute_labels(@canvas, @lattice)
    calculate_object_labels(@canvas, @lattice)
    update_transform
  end

  def update_transform
    @canvas.bounding_rect = @canvas.calculate_bounding_rect
    @canvas_view.calculate_transform
  end

  def load_cxt_file filename
    @canvas.remove_all_items

    file = File.open(filename)
    interp_context = load_formal_context_from_cxt_file(file)
    load_context interp_context
  end

  def calculate_concepts (canvas, lattice, pos)
    concept_style = Griff::CanvasConceptStyle.new(2, 10)
    @concept_item_map = Hash.new
    for concept in lattice do
      p = pos[concept]
      concept_item = Griff::CanvasConcept.new(canvas, concept, pos[concept], 10, concept_style)
      @concept_item_map[concept] = concept_item
    end
    return @concept_item_map
  end

  def calculate_lines(canvas, lattice, concept_item_map)
    @line_style = Griff::CanvasLineStyle.new(2)
    for concept in lattice do
      for upper_concept in lattice.upper_covers[concept] do
        Griff::CanvasLine.new(
          canvas, 
          concept_item_map[concept], 
          concept_item_map[upper_concept], 
          5, 
          @line_style
        )
      end
    end
  end

  def calculate_attribute_labels(canvas, lattice)
    @attr_style = Griff::CanvasTextStyle.new(1, "Comic", 14)
    @label_line_style = Griff::CanvasLineStyle.new(1)
    @attr_label_seq = []
    for concept in lattice do
      contingent = lattice.concept(concept).attribute_contingent;
      puts "size=#{contingent.size}"
      if contingent.size() > 0 then
        text = contingent.collect { |m| 
          @interp_context.attribute_interp.short_name(m)
        }
        concept_item = @concept_item_map[concept]
        p = Point.new(0,0)
        label = CanvasConceptLabel.new(canvas, concept_item, text, p, 40, @attr_style)
        label_line = CanvasLine.new(canvas, label, concept_item, 2, @label_line_style)
        @attr_label_seq.push(label)
      end
    end
  end

  def calculate_object_labels(canvas, lattice)
    @object_style = Griff::CanvasTextStyle.new(1, "Comic", 14)
    @label_line_style = Griff::CanvasLineStyle.new(1)
    @object_label_seq = []
    for concept in lattice do
      contingent = lattice.concept(concept).object_contingent;
      puts "size=#{contingent.size}"
      if contingent.size() > 0 then
        text = contingent.collect { |m| 
          @interp_context.object_interp.short_name(m)
        }
        concept_item = @concept_item_map[concept]
        p = Point.new(0,0)
        label = CanvasConceptLabel.new(canvas, concept_item, text, p, 40, @object_style)
        label_line = CanvasLine.new(canvas, label, concept_item, 2, @label_line_style)
        @object_label_seq.push(label)
      end
    end
  end

  def calculate_concept_positions
      for concept in @lattice do
        p = @pos[concept]
        @concept_item_map[concept].center_on(p)
      end
      for label in @attr_label_seq do
        label.update_position
      end
      for label in @object_label_seq do
        label.update_position
      end
  end

end

end
