set cppflags "-Wall -ggdb -fPIC"

set ccc    g++
set cc     gcc
set ar     ar
set ranlib ranlib
set moc    moc
set gm     glib-genmarshal

# ----------------------------------------------------------------------
# Global
#

set global(cppflags) "-Wall -ggdb -fPIC -DGC_DEBUG -DDEBUG"
set global(ldflags)  "-lgc -ggdb -DGC_DEBUG -DDEBUG"

# set global(cppflags) "-Wall -O4 -fPIC"
# set global(ldflags)  "-lgc -O4"

set global(prefix)      "/usr/local"
set global(install_dir) "/usr/local"
set global(include_dir) [get global install_dir]/include/sarl

set global(version3) 0.1.0
set global(version2) 0.1
set global(version1) 0

#
# Load globals from build_config.txt
#

if { [file exists "build_config.txt"] } {
    set fid [open "build_config.txt" r]
} else {
    set fid [open "../build_config.txt" r]
}

while { ! [eof $fid] } {
    set line [gets $fid]
    set line [string trim $line " \n\r"]
    if { $line != "" && [string index $line 0] != "\#" } {
	set i [string first = $line]
	set name [string range $line 0 [expr $i - 1]]
	set value [string range $line [expr $i+1] [string length $line]]
	set global($name) $value
    }
}


