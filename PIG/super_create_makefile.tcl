 
# Copyright 2004, Richard Cole
#
# This file is part of Griff.
# 
# Griff is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 2 of the License, or (at your
# option) any later version.
# 
# Griff is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
# 
# You should have received a copy of the GNU General Public License
# along with Griff; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

proc suffix { A s } {
    set result ""
    foreach x $A { lappend result $x$s }
    return $result
}

proc is_member { x A } {
    if { [lsearch $A x] == -1 } {
	return 0
    } else {
	return 1
    }
}

proc fl { xs } {
    set r ""
    foreach x $xs {
	set r [concat $r $x]
    }
    return $r
}

proc remove_string { s S } {
    set l [string first $s $S]
    if { $l != -1 } {
        return string replace $S $l [string length $s]
    } else {
        return $S
    }
}

proc val { x y } {
    upvar $y Y
    if { [array names Y $x] != "" } {
	return "$Y($x)"
    } else { return "" }
}

proc cond { x B s } {
    if { [is_member x B] == 0 } { return "" } else { return $s }
}

proc wrap_packages { x } {
    if { $x == "" } { return "" } else { return "-package \"$x\"" }
}


  

set sort_objects_txt {
    proc is_member { v vs } {
        return [expr [lsearch -exact $vs $v] != -1]
    }
    
    proc topo_sort { vs es } {
        
        foreach src $vs { set image($src) {} }
        foreach x [split $es \n] {
            if { [llength $x] == 2 } {
                set src [lindex $x 0]
                set dst [lindex $x 1]
                lappend image($src) $dst
            }
        }
        set result  {}
        foreach src $vs {
            if { [is_member $src $result] } { continue }
            set visited $src
            while { [llength $visited] != 0 } {
                set v [lindex $visited end]
                foreach dst $image($v) {
                    if { ! [is_member $dst $result] } {
                        if { [is_member $dst $visited] } {
                            puts stderr "Circular depend: $dst $visited"
			    lappend result $dst
                        } else {
                            lappend visited $dst
                        }
                    }
                }
                if { [lindex $visited end] == $v } {
		    if { ! [is_member $v result] } {
			lappend result $v
		    }
                    set visited \
                        [lrange $visited 0 [expr [llength $visited] - 2]]
                }
            }
        }
        return $result
    }    

    set es ""
    set vs ""
    while { ! [eof stdin] } {
        gets stdin line
        while { [string index $line end] == "\\" } {
            set line "[regsub "\$" $line ""][gets stdin]"
        }
        set ext [string range $line [string first . $line] [expr [string first : $line] - 1]]
        set line [regsub ":" $line ""]
        set hd [lindex $line 0]
        set tl [lrange $line 1 end]
        if { [string trim $hd] == "" } { continue }
        foreach t $tl {
            set t [regsub ".cmi" $t $ext]
            if { [string trim $t] != "" && $t != $hd } {
                set es "$es$hd $t\n"
                if { ! [is_member $t $vs] } { lappend vs $t }
            }
        }
        if { ! [is_member $hd $vs] } { lappend vs $hd }
    }
    set slist [lrange $argv 0 end]
    set tlist [topo_sort $vs $es]
    
    foreach t $tlist {
        if { [is_member $t $slist] } { 
            puts $t
        }
    }
    foreach s $slist {
        if { ! [is_member $s $tlist] } { 
            puts $s
        }
    }
}

set ofid [open sort_objects.tcl "w"]
puts $ofid $sort_objects_txt
close $ofid

proc sort_ml_sources { sources ext } {
    set deps [suffix $sources .dep]
    set objs [suffix $sources $ext]
    puts "\tcat $deps | tclsh sort_objects.tcl $objs > /tmp/source_list"
}

#
# Targets
#

puts -nonewline "all-projects: "
foreach y $contexts {
    foreach lib [val $y CPP_LIBRARY] {
	puts -nonewline "lib$lib.so "
    }
    foreach test [val $y CPP_TESTS] {
	puts -nonewline "$test.exe "
    }
    foreach test [val $y CPP_TARGETS] {
	puts -nonewline "$test.exe "
    }
    foreach test [val $y ML_TESTS] {
	puts -nonewline "$test.exe $test.byte "
    }
    foreach test [val $y ML_TARGETS] {
	puts -nonewline "$test.exe $test.byte "
    }
    foreach lib [val $y ML_LIBRARY] {
	puts -nonewline "$lib.cmxa $lib.cma "
    }
}
puts ""; puts ""

puts "all-test: [suffix $contexts -test]"
puts ""

foreach y $contexts {

    #
    # CPP Section
    #

    # .cpp file -> .o and .dep
    set cpp_opts "[fl [val $y CPP_INCLUDES]] [fl [val $y CPP_OPTS]]"
    foreach src [concat [val $y CPP_SOURCES] [val $y CPP_TESTS] [val $y CPP_TARGETS]] {
	puts "$src.o: $src.cpp"
	puts "\t\$(CCC) \$(CPP_FLAGS) -c -o $src.o $src.cpp $cpp_opts"
        puts ""
        puts "$src.dep: $src.cpp"
	puts "\t\$(CCC) \$(CPP_FLAGS) -M $src.cpp $cpp_opts > $src.dep"
	puts ""
	puts "include $src.dep"
	puts ""
    }

    # library -> .so
    foreach lib [val $y CPP_LIBRARY] {
	set objs "[suffix [val $y CPP_SOURCES] .o]"
	puts "lib$lib.so: $objs"
	puts "\t\$(CCC) \$(CPP_FLAGS) -shared -o \$@ $objs"
        puts ""
    }

    # tests -> .exe
    foreach test [val $y CPP_TESTS] {
	foreach lib [val $y CPP_LIBRARY] {
	    puts "$test.exe: $test.o lib$lib.so"
	    puts "\t\$(CCC) \$(CPP_FLAGS) -o \$@ $test.o -L. -l$lib"
	    puts ""
	}
    }

    foreach test [val $y CPP_TARGETS] {
	set cpp_libs [fl [val $y CPP_LIBS]]
	if { [set lib [fl [val $y CPP_LIBRARY]]] != "" } {
	    puts "$test.exe: $test.o lib$lib.so"
	    puts "\t\$(CCC) \$(CPP_FLAGS) -o \$@ $test.o -L. -l$lib $cpp_opts $cpp_libs"
	    puts ""
	} else {
	    puts "$test.exe: $test.o"
	    puts "\t\$(CCC) \$(CPP_FLAGS) -o \$@ $test.o -L. $cpp_opts $cpp_libs"
	    puts ""
	}
    }

    # test target
    puts ".PHONY: $y-test"
    puts ""
    puts "$y-test: [suffix [val $y CPP_TESTS] .exe]"
    foreach test [val $y CPP_TESTS] {
	puts "\tLD_LIBRARY_PATH=.; ./$test.exe > $test.log"
    }
    puts ""

    #
    # OCAML Section
    #

    foreach src [val $y MLY_SOURCES] {
	set MLI_SOURCES($y) [concat "$src" $MLI_SOURCES($y)]
	set ML_SOURCES($y) [concat "$src" $ML_SOURCES($y)]
    }

    foreach src [val $y MLL_SOURCES] {
	set ML_SOURCES($y) [concat "$src" $ML_SOURCES($y)]
    }

    foreach src [val $y ML_MLL_SOURCES] {
	set ML_SOURCES($y) [concat "$src" $ML_SOURCES($y)]
    }

    set ML_SOURCES($y) [concat [val $y ML_BEFORE_SOURCES] [val $y ML_SOURCES]]

    # .ml -> .cmx
    set packages [fl [val $y ML_PACKAGES]]
    set opts [fl [val $y ML_CPP_OPTS]]
    set ml_opts [fl [val $y ML_OPTS]]
    set cmo_opts [fl [val $y ML_CMO_OPTS]]
    set srcs \
	[concat \
	     [val $y ML_SOURCES] [val $y ML_TESTS] [val $y ML_TARGETS] \
	    ]
    foreach src $srcs {

	puts "$src.cmo: $src.ml"
	puts "\t\$(OCAMLFIND) ocamlc [wrap_packages $packages] -c $src.ml -ccopt \"$opts\" $ml_opts $cmo_opts"
	puts ""

	puts "$src.cmx: $src.ml"
	puts "\t\$(OCAMLFIND) ocamlopt [wrap_packages $packages] -c $src.ml -ccopt \"$opts\" $ml_opts" 
	puts ""

	puts "$src.dep: $src.ml"
	puts "\t\$(OCAMLFIND) ocamldep $src.ml > $src.dep"
	puts ""
	puts "include $src.dep"
	puts ""
    }

    foreach src [val $y MLL_SOURCES] {
	puts "$src.ml: $src.mll"
	puts "\tocamllex $src.mll"
	puts ""
    }

    foreach src [val $y MLY_SOURCES] {
	puts "$src.ml: $src.mly"
	puts "\tocamlyacc $src.mly"
	puts ""

	puts "$src.mli: $src.mly"
	puts "\tocamlyacc $src.mly"
	puts ""
    }

    foreach src [val $y MLY_SOURCES] {
	puts "$src.mli: $src.ml"
	puts ""
    }

    set ml_opts [fl [val $y ML_OPTS]]
    foreach src [val $y MLI_SOURCES] {
	puts "$src.cmi: $src.mli"
	puts "\t\$(OCAMLFIND) ocamlc [wrap_packages $packages] -c $src.mli -ccopt \"$opts\" $ml_opts"
	puts ""

	puts "$src\_mli.dep: $src.mli"
	puts "\t\$(OCAMLFIND) ocamldep [wrap_packages $packages] $src.mli > $src\_mli.dep"
	puts ""
	puts "include $src\_mli.dep"
	puts ""
    }

    # .cmxa
    set opts [fl [val $y ML_CPP_OPTS]]
    set cpp_libs [fl [val $y ML_CPP_LIBS]] 
    set mklib_opts [fl [val $y ML_MKLIB_OPTS]]
    set packages [fl [val $y ML_PACKAGES]]
    foreach lib [concat [val $y ML_LIBRARY]] {

	set ml_sources [val $y ML_SOURCES]

	set cpp_objs [concat \
			  [suffix [val $y CPP_SOURCES] .o]    \
			  [suffix [val $y CPP_OBJECTS] .o]]
	set ml_objs [suffix $ml_sources .cmx]
	set objs "$cpp_objs $ml_objs"

	set other_deps [suffix [val $y MLI_SOURCES] .cmi] 

	if { $cpp_objs == "" } {

	    puts "$lib.cmxa: $objs $other_deps"

	    sort_ml_sources $ml_sources .cmx
	    puts -nonewline "\t\$(OCAMLFIND) ocamlopt [wrap_packages $packages] -a -o $lib.cmxa "
	    puts "`cat /tmp/source_list` -ccopt \"$opts\""
	    puts ""
	    
	    set ml_objs [suffix $ml_sources .cmo]
	    set objs "$cpp_objs $ml_objs" 
	    set other_deps [suffix [val $y MLI_SOURCES] .cmi] 
	    
	    puts "$lib.cma: $objs $other_deps"

	    sort_ml_sources $ml_sources .cmo
	    puts -nonewline "\t\$(OCAMLFIND) ocamlc [wrap_packages $packages] -a -o $lib.cma" 
	    puts " -ccopt \"$opts\" `cat /tmp/source_list` "
	    puts ""
	} else {
	    puts "$lib.cmxa: $objs $other_deps"
	    sort_ml_sources $ml_sources .cmx
	    puts "\tar rc lib$lib.cma.a $cpp_objs"
	    puts -nonewline "\t\$(OCAMLFIND) ocamlopt [wrap_packages $packages] -a -o $lib.cmxa "
	    puts "$cpp_objs `cat /tmp/source_list` -ccopt $opts -cclib \"$cpp_libs\" "
	    puts ""
	    
	    set ml_objs [suffix $ml_sources .cmo]
	    set objs "$cpp_objs $ml_objs" 
	    set other_deps [suffix [val $y MLI_SOURCES] .cmi] 
	    
	    puts "$lib.cma: $objs $other_deps"
	    
	    sort_ml_sources $ml_sources .cmo
	    puts "\tgcc -shared -o dll$lib.cma.so $cpp_objs $opts $cpp_libs"
	    
	    puts -nonewline "\t\$(OCAMLFIND) ocamlc [wrap_packages $packages] -a -o $lib.cma" 
	    puts " -ccopt \"$opts\" -cclib \"$cpp_libs\" `cat /tmp/source_list` -dllib -l$lib.cma "
	    puts ""
	}
    }
	
    # tests -> .exe
    set opts [fl [val $y ML_CPP_OPTS]]
    set ml_opts [fl [val $y ML_OPTS]]
    set packages [fl [val $y ML_PACKAGES]]
    set libs [concat \
		  [suffix [fl [val $y ML_LIBS]] .cmxa] \
		  [suffix [fl [val $y ML_OBJS]] .cmx] ] 
    set lib_deps [suffix [fl [val $y ML_LIB_DEPS]] .cmxa]
    foreach test [val $y ML_TESTS] {
	foreach lib [val $y ML_LIBRARY] {
	    puts "$test.exe: $test.cmx $lib.cmxa $lib_deps"
	    puts "\t\$(OCAMLFIND) ocamlopt [wrap_packages $packages] -o $test.exe -I . $ml_opts $libs $lib.cmxa $test.cmx -ccopt \"$opts\""
	    puts ""
	}
    }

    set libs [concat \
		  [suffix [fl [val $y ML_LIBS]] .cma] \
		  [suffix [fl [val $y ML_OBJS]] .cmo] ]
    set ml_opts [fl [val $y ML_OPTS]]
    set cmo_opts [fl [val $y ML_CMO_OPTS]]
    set lib_deps [suffix [fl [val $y ML_LIB_DEPS]] .cma]
    foreach test [val $y ML_TESTS] {
	foreach lib [val $y ML_LIBRARY] {
	    puts "$test.byte: $test.cmo $lib.cma $lib_deps"
	    puts "\t\$(OCAMLFIND) ocamlc [wrap_packages $packages] -o $test.byte -I . $ml_opts $libs $lib.cma $test.cmo -ccopt \"$opts\" $cmo_opts"
	    puts ""
	}
    }

    set opts [fl [val $y ML_CPP_OPTS]]
    set ml_opts [fl [val $y ML_OPTS]]
    set packages [fl [val $y ML_PACKAGES]]
    set lib_deps [suffix [fl [val $y ML_LIB_DEPS]] .cmxa]
    set libs [concat \
		  [suffix [fl [val $y ML_LIBS]] .cmxa] \
		  [suffix [fl [val $y ML_OBJS]] .cmx] ]
    foreach target [val $y ML_TARGETS] {
	if { [set lib [fl [val $y ML_LIBRARY]]] != "" } {
	    puts "$target.exe: $target.cmx $lib.cmxa $lib_deps"
	    puts "\t\$(OCAMLFIND) ocamlopt [wrap_packages $packages] -o $target.exe -I . $ml_opts $libs $lib.cmxa $target.cmx -ccopt \"$opts\""
	    puts ""
	} else {
	    puts "$target.exe: $target.cmx $lib_deps"
	    puts "\t\$(OCAMLFIND) ocamlopt [wrap_packages $packages] -o $target.exe -I . $ml_opts $libs $target.cmx -ccopt \"$opts\""
	    puts ""
	}
    }

    set libs [concat \
		  [suffix [fl [val $y ML_LIBS]] .cma] \
		  [suffix [fl [val $y ML_OBJS]] .cmo] ]
    set cmo_opts [fl [val $y ML_CMO_OPTS]]
    set ml_opts [fl [val $y ML_OPTS]]
    set lib_deps [suffix [fl [val $y ML_LIB_DEPS]] .cma]
    foreach target [val $y ML_TARGETS] {
	if { [set lib [fl [val $y ML_LIBRARY]]] != "" } {
	    puts "$target.byte: $target.cmo $lib.cma $lib_deps"
	    puts "\t\$(OCAMLFIND) ocamlc [wrap_packages $packages] -o $target.byte -I . $ml_opts $libs $lib.cma $target.cmo -ccopt \"$opts\" $cmo_opts"
	    puts ""
	} else {
	    puts "$target.byte: $target.cmo $lib_deps"
	    puts "\t\$(OCAMLFIND) ocamlc [wrap_packages $packages] -o $target.byte -I . $ml_opts $libs $target.cmo -ccopt \"$opts\" $cmo_opts"
	    puts ""
	}
	
    }
}


  
  
  


  
  
