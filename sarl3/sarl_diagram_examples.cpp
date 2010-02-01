#include "sarl_diagram_examples.h"
#include "sarl_diagram.h"
#include "sarl_context_examples.h"
#include "sarl_lattice.h"

sarl_diagram_t* sarl_diagram_create_b3()
{
  sarl_context_t* context = sarl_context_create_b3();
  sarl_lattice_t* lattice = sarl_lattice_new(context);
  sarl_diagram_t* diagram = sarl_diagram_new_from_lattice(lattice);
  return diagram;
};
