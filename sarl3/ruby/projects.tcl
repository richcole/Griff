# ----------------------------------------------------------------------
# Projects
#

set projects { sarl sarlgtk sarlqt }
set contexts [concat global $projects]

# ----------------------------------------------------------------------
# Global
#

set global(cppflags) "-Wall -ggdb -fPIC"
set global(ldflags) "-lgc"

set global(install_dir) "/usr/local"

set global(version3) 0.1.0
set global(version2) 0.1
set global(version1) 0

# ----------------------------------------------------------------------
# Sarl
#

set sarl(cppsources) {
    sarl_rb_get_object
    sarl_rb
}

set sarl(rubylib) sarlrb 
set sarl(ldflags)  "-L. -L.. -L/usr/local/lib -lpthread -ldl -lcrypt -lm -lc -lsarl -L/mnt/debian-mount/usr/local/lib"
set sarl(cppflags) "-I. -I.. -I/usr/lib/ruby/1.8/i486-linux/ -I/usr/lib/ruby/1.8/i386-linux -I/mnt/debian-mount/usr/local/include/sarl"

# ----------------------------------------------------------------------
# Gtk
#

set gtk(cppsources) {
    sarl_gtk_rb 
    sarl_lattice_view_gtk_rb
}

set gtk(ldflags)  "`pkg-config --libs gtk+-2.0` -lpthread -ldl -lcrypt -lm -lc -L. -L.. -L/usr/local/lib -lsarl -lsarlgtk -L/mnt/debian-mount/usr/local/lib"
set gtk(cppflags) "-I. -I.. `pkg-config --cflags gtk+-2.0` -I/usr/lib/ruby/1.8/i486-linux/ -I/usr/lib/ruby/1.8/i386-linux/ -I/mnt/debian-mount/usr/local/include/sarl"

set gtk(rubylib) sarlrbgtk



