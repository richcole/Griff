# ----------------------------------------------------------------------
# Projects
#

set ignored_projects { qt }
set projects         { sarl gtk } 
set contexts         [concat global $projects]

# ----------------------------------------------------------------------
# Qt
#

set qt(cppflags) {
    -I/usr/include/qt3
}

set qt(cppsources) {
    sarl_lattice_view_qt
    sarl_lattice_view_label
}

set qt(moc_headers) {
    sarl_lattice_view_qt
}

set qt(libname) {
    sarlqt
}

set qt(headers) ""
set qt(headers) [concat $qt(headers) $qt(cppsources)]

# ----------------------------------------------------------------------
# Sarl
#

set sarl(cppsources) {
    sarl_rect               sarl_point              sarl_string_table 
    sarl_bit_set            sarl_power_set          sarl_power_map  
    sarl_context            sarl_lattice            sarl_diagram 
    sarl_context_examples   sarl_diagram_examples   sarl_wrapper 
    sarl_array              sarl_int_array          sarl_finalization
    sarl_layout             sarl_int_set            sarl_hash_map
    sarl_point_set          sarl_metrics            sarl_context_formats
    sarl_exceptions         sarl_clarified_context  sarl_sat
    sarl_cnf                sarl_variable_closure   sarl_min_unsat
    sarl_factor_clauses     sarl_ksubset            sarl_contraction_map
    sarl_implication_set    sarl_prune_clauses      sarl_sort
}

set sarl(cpptests) [suffix $sarl(cppsources) _test]
set sarl(cppprogs) {
    prune_clauses sarl_sat_main dump_uninstallable_packages gen_conflict_context 
    gen_sat_context min_unsat sarl_min_unsat_cover reduce_clauses
}


set sarl(headers) {
    sarl_assert sarl_gc sarl_types
}

set sarl(headers) [concat $sarl(headers) $sarl(cppsources)]

set sarl(libname) sarl

set sarl(cppflags) "-I."

# ----------------------------------------------------------------------
# Gtk
#

set gtk(cppsources) {
    sarl_lattice_view_gtk sarl_lattice_view_interp sarl_label
}

set gtk(marshallers) {
    sarl_lattice_view_gtk_marshaller
}

set gtk(ldflags) \
    "`pkg-config --libs gtk+-2.0` -L/usr/lib/debug/ -L. -lsarl"

set gtk(cppflags)  "-I. `pkg-config --cflags gtk+-2.0`"

set gtk(headers) ""
set gtk(headers) [concat $gtk(headers) $gtk(cppsources)]

set gtk(libname) sarlgtk

set gtk(cppprogs) [suffix $gtk(cppsources) _test]

foreach m $gtk(marshallers) { 
    lappend gtk(csources) $m 
}

