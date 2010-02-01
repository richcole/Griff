#ifndef SARL_WRAPPER_H
#define SARL_WRAPPER_H

#include "sarl_context.h"
#include "sarl_lattice.h"
#include "sarl_diagram.h"
#include "sarl_string_table.h"

#define SARL_DEFINE_THIN_WRAPPER(SarlDiagram, typ, init) \
  SarlDiagram() { ptr = init; }; \
  SarlDiagram(typ *p) { ptr = p; }; \
  SarlDiagram(SarlDiagram const& x) { \
    ptr = x.ptr; \
  }; \
\
  SarlDiagram& operator=(SarlDiagram const& x) { \
    ptr = x.ptr; \
    return *this; \
  }; \
  typ * operator->() const { return ptr; }; \
  operator typ * () { return ptr; }

class SarlContext {
public:
  sarl_context_t *ptr;

  SARL_DEFINE_THIN_WRAPPER(SarlContext, sarl_context_t, sarl_context_new());

  void insert(sarl_unsigned_int g, sarl_unsigned_int m) {
    sarl_context_insert(ptr, g, m);
  }
};

class SarlStringTable {
public:
  sarl_string_table_t* ptr;

  SARL_DEFINE_THIN_WRAPPER(
    SarlStringTable, sarl_string_table_t, sarl_string_table_default_new()
  );

  void set(sarl_unsigned_int index, char *str) {
    sarl_string_table_set(ptr, index, str);
  }

  char const* get(sarl_unsigned_int index) {
    return sarl_string_table_get(ptr, index);
  }
};

class SarlLattice {
public:
  sarl_lattice_t *ptr;

  SARL_DEFINE_THIN_WRAPPER(SarlLattice, sarl_lattice_t, 0);

  SarlLattice(SarlContext const& context) {
    ptr = sarl_lattice_new(context.ptr);
  }

  sarl_unsigned_int count() {
    return sarl_lattice_count(ptr);
  }

};

class SarlDiagram {
public:
  sarl_diagram_t *ptr;

  SARL_DEFINE_THIN_WRAPPER(SarlDiagram, sarl_diagram_t, 0);

  SarlDiagram(SarlContext const& context) {
    ptr = sarl_diagram_new_from_context(context.ptr);
  }

  SarlDiagram(SarlLattice const& lattice) {
    ptr = sarl_diagram_new_from_lattice(lattice.ptr);
  }
};

#endif
