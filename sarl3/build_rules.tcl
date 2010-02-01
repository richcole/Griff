
# ----------------------------------------------------------------------
# Rules
#

foreach x $projects {
    set p [prefix [get global include_dir] -I]
    set xf [get $x cppflags] 
    array set $x [list cppflags [concat $xf $p]]
}

puts "all: libs tests progs"
puts ""

foreach x $projects {  # qt
    foreach y [array names $x] {
        set str [string map {\n " "} [subst \$[subst $x\($y\)]]]
        while { [regsub {\s\s+} $str " " res]>0 } { 
            set str $res
        }
        array set $x [list $y $str]
    }
}

foreach x $projects {
    set $x\(install_dir\) [get global install_dir]
    set $x\(include_include_dir\) [get global install_include_dir]
}

foreach x $projects {

    foreach s [get $x marshallers] {
        gen_marshal $s $x
    }

    foreach s [get $x csources]  {
        c_compile $s $x
        lappend [subst $x\(objects\)] $s.o
    }

    foreach s [get $x ctests] {
        c_compile $s $x
    }

    foreach s [get $x cppsources] {
        cpp_compile $s $x
        lappend [subst $x\(objects\)] $s.o
    }

    foreach s [concat [get $x cpptests] [get $x cppprogs]] {
        cpp_compile $s $x
    }

    foreach t [concat [get $x cpptests] [get $x cppprogs]] {
        puts "$t.exe: $t.o [get $x objects]"
#        puts "\t$ccc -o $t.exe $t.o -L. -l[get $x libname] [get $x ldflags] [get global ldflags]"
        puts "\t$ccc -o $t.exe $t.o [get $x objects] [get $x ldflags] [get global ldflags]"
        puts ""
    }

    foreach t [get $x cpptests] {
        lappend [subst $x\(tests\)] $t.exe
    }

    foreach t [get $x cppprogs] {
        lappend [subst $x\(progs\)] $t.exe
    }

    foreach t [get $x ctests] {
        puts "$t.exe: $t.o [get $x objects]"
        puts "\t$cc -o $s.exe $t.o -L. -l[get $x libname] [get $x ldflags] [get global ldflags]"
        puts ""

        lappend [subst $x\(tests\)] $t.exe
    }

    foreach l [get $x libname] {
        puts "lib$l.so: [get $x objects]"
        puts "\t$ccc -shared -o lib$l.so [get $x objects] [get $x ldflags] [get global ldflags]"
        puts ""

        lappend libs lib$l.so
    }

    foreach l [get $x rubylib] {
        puts "$l.so: [get $x objects]"
        puts "\t$ccc -shared -o $l.so [get $x objects] [get $x ldflags] [get global ldflags]"
        puts ""

        lappend libs $l.so
    }

    puts ".PHONY: tests-$x run-tests-$x"
    puts ""
    puts "tests-$x: [get $x tests]"
    puts ""
    puts "run-tests-$x: tests-$x"
    puts "\tset -e \# allow error propogration"
    foreach y [get $x tests] {
        puts "\t./$y"
    }
    puts ""
    puts "run-tests: run-tests-$x"
    puts "tests: tests-$x"
    puts ""

    puts ".PHONY: progs-$x"
    puts ""
    puts "progs-$x: [get $x progs]"
    puts "progs: progs-$x"

    puts ".PHONY: install-$x"
    puts ""
    puts "install-$x: lib$l.so"
    puts "\tinstall lib$l.so [get $x install_dir]/lib/lib$l.so.[get global version3]"
    puts "\trm -f [get $x install_dir]/lib/lib$l.so.[get global version2]"
    puts "\tln -s lib$l.so.[get global version3] [get $x install_dir]/lib/lib$l.so.[get global version2]"
    puts "\trm -f [get $x install_dir]/lib/lib$l.so.[get global version1]"
    puts "\tln -s lib$l.so.[get global version3] [get $x install_dir]/lib/lib$l.so.[get global version1]"
    puts "\trm -f [get $x install_dir]/lib/lib$l.so"
    puts "\tln -s lib$l.so.[get global version3] [get $x install_dir]/lib/lib$l.so"
    puts "\tinstall [suffix [get $x headers] .h] [get $x install_include_dir]/include"
    puts ""

    lappend install install-$x
}

puts "install: $install"
puts ""
puts "libs: $libs"
puts ""


