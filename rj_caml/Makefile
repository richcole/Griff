OCAMLOPT   = ocamlopt
OCAMLC     = ocamlc
OCAMLMKLIB = ocamlmklib
OCAMLFIND  = ocamlfind

SOURCES = \
  rb_set.ml lexicon.ml file.ml smatrix.ml mmatrix.ml \
  broker.ml cache.ml usual.ml bit_vect.ml bit_set.ml \
  power_map.ml relation.ml trie.ml stemmer.ml

INTERFACES = $(SOURCES:%.ml=%.cmi)

CFILES  = \
  ml_file.c \
  ml_bit_vect.c \
  ml_stemmer.c

TESTS   = \
  rb_set \
  trie \
  bit_vect 

CMX_FILES = $(SOURCES:%.ml=%.cmx)
CMO_FILES = $(SOURCES:%.ml=%.cmo)
DEP_FILES = $(SOURCES:%.ml=%.dep)
OBJ_FILES = $(CFILES:%.c=%.o)

CCOPT    = -ccopt "-ggdb"

.PHONY: clean

all: rj_caml.cmxa rj_caml.cma

test: $(TESTS:%=test_%.exe)


clean:
	rm -f *.cm* *.exe *.o *.dep

%.ml: %.mll
	ocamllex $<

%.cmx: %.ml
	$(OCAMLOPT) $(CCOPT) $(INCLUDES) -c $<

%.cmi: %.mli
	$(OCAMLOPT) $(CCOPT) $(INCLUDES) -c $<

%.cmo: %.ml
	$(OCAMLC) $(CCOPT) $(INCLUDES) -g -c $<

%.o: %.c
	$(OCAMLC) -ccopt "-I/usr/lib/ocaml -ggdb" $<

%.dep: %.ml
	ocamldep $(INCLUDES) $< > $@

%.exe: %.cmx rj_caml.cmxa
	$(OCAMLOPT) $(CCOPT) $(INCLUDES) -I . -o $@ str.cmxa rj_caml.cmxa $< 

rj_caml.cmxa: $(CMX_FILES) $(OBJ_FILES)
	$(OCAMLMKLIB) -verbose -o $(@:%.cmxa=%) $(CMX_FILES) $(OBJ_FILES)

rj_caml.cma: $(CMO_FILES) $(OBJ_FILES)
	$(OCAMLMKLIB) -verbose -o $(@:%.cma=%) $(CMO_FILES) $(OBJ_FILES)

include $(DEP_FILES)


install:
	$(OCAMLFIND) install rj_caml \
           librj_caml.a rj_caml.a rj_caml.cmxa rj_caml.cma dllrj_caml.so \
	   $(INTERFACES) META

uninstall:
	$(OCAMLFIND) remove rj_caml
