OCAMLOPT   = ocamlopt
OCAMLC     = ocamlc
OCAMLMKLIB = ocamlmklib
OCAMLFIND  = ocamlfind
OCAMLDEP   = ocamldep

PROGRAMS  = 

SOURCES   = $(PROGRAMS:%=%.ml) cgi.ml
DEP_FILES = $(SOURCES:%.ml=%.dep)
PACKAGES  = -package "rj_caml"

CGI_INSTALL = $(HOME)/public_html/cgi-bin

%.ml: %.mll
	ocamllex $<

%.cmx: %.ml
	$(OCAMLFIND) $(OCAMLOPT) $(PACKAGES) -c $<

%.cmo: %.ml
	$(OCAMLFIND) $(OCAMLC) $(PACKAGES) -g -c $<

%.o: %.c
	$(OCAMLFIND) $(OCAMLC) \
           -ccopt "-I/usr/lib/ocaml -ggdb" $<

%.dep: %.ml
	$(OCAMLFIND) $(OCAMLDEP) $< > $@

all: $(PROGRAMS:%=%.exe) cgi.cmo cgi.cmx

install: $(PROGRAMS:%=%.exe) cgi.cmx cgi.cmo
	ocamlfind install rj_cgi cgi.cmx cgi.cmo cgi.cmi cgi.o META

uninstall:
	ocamlfind remove rj_cgi

clean:
	rm -f *.cm* *.so *.a *.dll

include $(DEP_FILES)

