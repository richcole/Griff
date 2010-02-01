
EXES = \
  assert_csv apply_rules execute_action dump_quad_store \
  assert_graph assert_graph_from_file query_graph retract_graph \
  print_cxt2

install: $(EXES:%=%.exe)
	bash install_exes.sh $(EXES)

uninstall: 
	bash uninstall_exes.sh $(EXES)

binary_tgz_package: $(EXES:%=%.exe) config_vars.sh
	bash create_binary_package.sh $(EXES:%=%.exe) 
	
	