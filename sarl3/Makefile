
.PHONY: build

all: build_deps build

build.mak: build.tcl build_config.tcl build_rules.tcl projects.tcl build_funcs.tcl
	tclsh build.tcl > build.mak

build_deps.mak: build_deps.tcl build_config.tcl build_rules.tcl projects.tcl build_funcs.tcl
	tclsh build_deps.tcl > build_deps.mak

sarl_lattice_view_gtk_marshaller.c: build.mak
	$(MAKE) -fbuild.mak sarl_lattice_view_gtk_marshaller.c

build_deps: build_deps.mak build.mak sarl_lattice_view_gtk_marshaller.c
	$(MAKE) -fbuild_deps.mak
	cd ruby && $(MAKE) build_deps

build: build.mak 
	$(MAKE) -fbuild.mak
	cd ruby && $(MAKE) build

install: build.mak
	$(MAKE) -fbuild.mak install

tests: build.mak
	$(MAKE) -fbuild.mak tests

test: build.mak
	$(MAKE) -fbuild.mak run-tests

dep-clean: build.mak
	$(MAKE) -fbuild.mak dep-clean

clean:
	rm -f build_deps.mak build.mak
	rm -f ruby/build_deps.mak ruby/build.mak
	rm -f *.o *.exe *.so *.dep
	rm -f sarl_lattice_view_gtk_marshaller.c
	rm -rf tmp dist
	cd ruby && $(MAKE) clean



