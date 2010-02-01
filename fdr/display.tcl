
set f      [frame  .frame]
set bbox   [frame  $f.b  ]
set canvas [canvas $f.canvas -width 600 -height 600 -background white]

pack $f -expand true -fill both -side top
pack $bbox -expand false -fill x -side bottom
pack $canvas -expand true -fill both -side top

set next [button $bbox.next -text "Next" -command { 
    global canvas P D E # Fr Fa Ft
    read_points P stdin
    read_points PL stdin
    dims $canvas P D 
    plot $canvas P E D PL 
} ]

set print [button $bbox.print -text "Print" -command { 
    global canvas P D E
    set filename [tk_getSaveFile -filetypes "{{EPS} {*.eps}}" -defaultextension ".eps" ]
    $canvas postscript -file $filename -width $D(w) -height $D(h)
} ]

pack $next -expand false -fill x -side left
pack $print -expand false -fill x -side left

bind $canvas <Configure> {
    global canvas P D E PL 
    set D(w) %w
    set D(h) %h
    dims $canvas P D 
    plot $canvas P E D PL
}

set P(length) 0
set PL(length) 0
set E(length) 0
set Fr(length) 0
set Fa(length) 0
set Ft(length) 0

proc read_points { P_name in } {

    upvar $P_name P

    set i 1
    set P(length) 0
    while { ! [eof $in] } {
        set line [gets $in]
        if { 1==[regexp {([0-9.\-]+) ([0-9.\-]+)} $line dummy x y] } {
            set P($i) $line
            set P(length) $i
            incr i 1
        } else {
            return
        }
    }
}

proc max_of { x y } {
    if { $x > $y } { return $x }
    return $y
}

proc min_of { x y } {
    if { $x < $y } { return $x }
    return $y
}

proc dims { canvas P_name D_name } {
    upvar $P_name P
    upvar $D_name D

    set D(min_x) 0
    set D(max_x) 0
    set D(min_y) 0
    set D(max_y) 0
    for { set i 1 } { $i <= $P(length) } { incr i 1 } {
        set x [lindex $P($i) 0]
        set y [lindex $P($i) 1]
        set D(max_x) [max_of $x $D(max_x)]
        set D(max_y) [max_of $y $D(max_y)]
        set D(min_x) [min_of $x $D(min_x)]
        set D(min_y) [min_of $y $D(min_y)]
    }
    set s1 [expr $D(w) / ($D(max_x) - $D(min_x))]
    set s2 [expr $D(h) / ($D(max_y) - $D(min_y))]
    set D(s) [expr [min_of $s1 $s2] * 0.75]
}

proc plot { canvas P_name E_name D_name PL_name } {
    upvar $P_name P
    upvar $E_name E
    upvar $D_name D
    upvar $PL_name PL

    $canvas delete all
    
    set ox [expr 0.5 * ($D(w) - ($D(s) * ($D(max_x) - $D(min_x))))]
    set oy [expr 0.5 * ($D(h) - ($D(s) * ($D(max_y) - $D(min_y))))]
    set r  [expr 0.01 * [min_of $D(w) $D(h)]]

    for { set i 1 } { $i <= $PL(length) } { incr i 1 } {
        set X($i) [expr ([lindex $PL($i) 0] - $D(min_x)) * $D(s)]
        set Y($i) [expr ([lindex $PL($i) 1] - $D(min_y)) * $D(s)]
        $canvas create oval \
            [expr ( $X($i) + $ox ) - $r] [expr ( $Y($i) + $oy ) - $r] \
            [expr $X($i) + $ox + $r] [expr $Y($i) + $oy + $r] \
            -fill black
    }

    for { set i 1 } { $i <= $P(length) } { incr i 1 } {
        set X($i) [expr ([lindex $P($i) 0] - $D(min_x)) * $D(s)]
        set Y($i) [expr ([lindex $P($i) 1] - $D(min_y)) * $D(s)]
        if { $PL(length) == 0 } {
            $canvas create oval \
                [expr ( $X($i) + $ox ) - $r] [expr ( $Y($i) + $oy ) - $r] \
                [expr $X($i) + $ox + $r] [expr $Y($i) + $oy + $r] \
                -fill black
        }
    }

    for { set i 1 } { $i <= $E(length) } { incr i 1 } {
        set u [lindex $E($i) 0]
        set v [lindex $E($i) 1]
        set ux $X($u)
        set uy $Y($u)
        set vx $X($v)
        set vy $Y($v)
        if { $uy > $vy } { set color red } else { set color black }
        $canvas create line \
            [expr $ux + $ox] [expr $uy + $oy] \
            [expr $vx + $ox] [expr $vy + $oy] \
            -fill $color
    }
}
    
    

read_points E [open [lindex $argv 0] "r"]
read_points P stdin
read_points PL stdin
set D(w) [$canvas cget -width] 
set D(h) [$canvas cget -height]
dims $canvas P D 
plot $canvas P E D PL



            