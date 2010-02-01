require 'lexicon'


$metric_lexicon = Lexicon.new

$metric_names = [
  'edgeCrossing', 
  'symChildren', 
  'symChildrenNZ', 
  'reflections', 
  'vectors', 
  'chains', 
  'edgeLength', 
  'pathWidth', 
  'meetVectors', 
  'wellPlaced', 
  'okPlaced', 
  'ballance', 
  'numThreeChains', 
  'planar', 
  'horizontalShift', 
  'num_unique_gradients', 
  'num_unique_gradients_abs'
]

$metric_names.each { |name| $metric_lexicon.index(name) }
