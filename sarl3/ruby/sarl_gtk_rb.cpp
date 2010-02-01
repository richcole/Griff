#include "sarl_gtk_rb.h"
#include "sarl_lattice_view_gtk_rb.h"

VALUE mSarlGtk = 0;

void
Init_sarlrbgtk()
{
  rb_require("gtk2");

  mSarlGtk = rb_define_module ("SarlGtk");

  Init_sarl_lattice_view_gtk(mSarlGtk);
};
