include config.mak
  
  
.PHONY: all clean doc projects test
  
all: projects.mak all-projects
  
clean:
	rm -f *.cm* *.dep projects.mak *.a *.o
  
# Targets
  
projects.mak: create_makefile.tcl
	tclsh create_makefile.tcl > projects.mak
 
# Include the generated file
 
# Disable default rules
  
include projects.mak
include extra.mak
include install.mak
  
%: %.o
%: %.cpp

