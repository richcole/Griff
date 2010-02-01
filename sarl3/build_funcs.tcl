# Build Rules

proc get { n y } {
    upvar $n A
    return [lindex [array get A $y] 1]
}

proc suffix { l s } {
    set r ""
    foreach x $l { lappend r $x$s }
    return $r
}

proc prefix { l s } {
    set r ""
    foreach x $l { lappend r $s$x }
    return $r
}

proc remove { l s } {
    set r ""
    foreach x $l {
        puts stderr "s=$s, x=$x"
        if { ! [string match $s $x] } {
            puts stderr "include $x"
            lappend r $x
        }
    }
    return $r
}


proc cpp_compile { s x } {
    global ccc global
    upvar $x $x

    puts "$s.o: $s.cpp"
    puts "\t$ccc -c [get $x cppflags] [get global cppflags] -o $s.o $s.cpp"
    puts ""
    
}

proc cpp_deps { s x } {
    global ccc global
    upvar $x $x

    puts "$s.dep: $s.cpp"
    puts "\t$ccc -c [get $x cppflags] [get global cppflags] -o $s.dep -M $s.cpp"
    puts ""
}

proc c_compile { s x } {
    global cc global
    upvar $x $x

    puts "$s.o: $s.c"
    puts "\t$cc -c [get $x cppflags] [get global cppflags] -o $s.o $s.c"
    puts ""
    
}

proc c_deps { s x } {
    global cc global
    upvar $x $x

    puts "$s.dep: $s.c"
    puts "\t$cc -c $s.c [get $x cppflags] [get global cppflags] -o $s.dep -M"
    puts ""
}

proc gen_marshal { s x } {
    global gm
    upvar $x $x
    
    puts "$s.h: $s.list"
    puts "\t$gm --header $s.list > $s.h"
    puts ""

    puts "$s.c: $s.list"
    puts "\t$gm --body $s.list > $s.c"
    puts ""
}
    
proc marshal_deps { x } {
    global gm
    upvar $x $x

    foreach m [get $x marshallers] { 
        puts "\# Marshaller Dependencies"
        set cmd [concat exec grep $m [suffix [get $x cppsources] .cpp]]
        set tmp [eval $cmd]
        set tmp [split $tmp \n]
        set gen-deps ""
        foreach s $tmp {
            set gen_dep [string range $s 0 [string first : $s]]
            set gen_dep [string range $gen_dep 0 [string last . $gen_dep]]
            set dep dep
            puts "$gen_dep$dep: $m.h" 
        }
    }
}

