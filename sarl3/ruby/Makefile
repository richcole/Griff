
.PHONY: build

all: build_deps build 

build.mak: build.tcl ../build_config.tcl ../build_rules.tcl ../projects.tcl ../build_funcs.tcl
	tclsh build.tcl > build.mak

build_deps.mak: build_deps.tcl ../build_config.tcl ../build_dep_rules.tcl ../projects.tcl ../build_funcs.tcl
	tclsh build_deps.tcl > build_deps.mak

build_deps: build_deps.mak
	$(MAKE) -fbuild_deps.mak

build: build.mak
	$(MAKE) -fbuild.mak

install: build.mak
	$(MAKE) -fbuild.mak install

dep-clean: build.mak
	$(MAKE) -fbuild.mak dep-clean

clean:
	rm -f *.o *.exe *.so *.dep


