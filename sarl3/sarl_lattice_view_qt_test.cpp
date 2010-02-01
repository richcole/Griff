#include "sarl_lattice_view.h"
#include "sarl_context_examples.h"
#include "sarl_diagram.h"
#include "sarl_string_table.h"

#include <qapplication.h>

int main( int argc, char **argv )
{
  QApplication app(argc,argv);

  sarl_unsigned_int n = 6;

  sarl_string_table_t* stab = sarl_string_table_default_new();
  sarl_string_table_populate(stab, 1, n);

  SarlLatticeView lattice_view(
    sarl_diagram_new_from_context(sarl_context_create_boolean(n)), 
    stab, stab
  );

  app.setMainWidget( &lattice_view );
  lattice_view.show();

  return app.exec();
}
