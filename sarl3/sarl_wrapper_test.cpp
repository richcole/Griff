#include "sarl_wrapper.h"
#include "sarl_assert.h"

int main(int argc, char **argv) 
{
  SarlContext context;
  sarl_unsigned_int i;
  for(i=1;i<=3;++i) {
    context.insert(i,i);
  }
  
  SarlStringTable table;
  table.set(1, "one");
  table.set(2, "two");
  table.set(3, "three");

  SarlLattice lattice(context);
  SarlDiagram diagram(lattice);

  sarl_test_end;
};
