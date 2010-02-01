foreach x $projects {
    set p [prefix [get global include_dir] -I]
    set xf [get $x cppflags] 
    array set $x [list cppflags [concat $xf $p]]
}

puts "all: deps"
puts ""

foreach x $projects {
    foreach sx { ctests csources cprogs } {
	foreach s [get $x $sx]  {
	    c_deps $s $x
	    lappend deps $s.dep
	}
    }
    foreach sx { cppsources cpptests cppprogs } {
	foreach s [get $x $sx] {
	    cpp_deps $s $x
	    lappend deps $s.dep
	}
    }
    puts ""

    marshal_deps $x
}

puts ""
puts "deps: $deps"
puts ""

puts ".PHONY: dep-clean"
puts "dep-clean: "
puts "\trm -f $deps"
puts ""
