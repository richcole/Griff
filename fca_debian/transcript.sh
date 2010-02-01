#!/bin/bash

# Compute the CNF Expression
#   inputs: available, status
#   outputs: packages.cnf packages.lex
./package_solver var/lib/dpkg/{available,status} $1

# Ensure that the CNF Expression is not solvable
if [ ! ../sarl3/sarl_sat_main.exe packages.cnf ] ; then 
    echo "Error: this package is not in a conflict state."
    exit -1
fi

# split the CNF into foreground and background
#
# This unasserts 1 and asserts the clauses the are implied by 1 in the 
# foreground
ruby split_cnf.rb packages.cnf packages-background.cnf packages-foreground.cnf


# find the minimal conflicting sets
../sarl3/gen_conflict_context.exe packages-background.cnf packages-foreground.cnf > packages-conflict.txt

# Compute the implication context for the clauses that were present in the conflict context
../sarl3/gen_sat_context.exe packages-{background,foreground}.cnf packages-conflict.txt > packages-sat.oaf

# Convert oaf to cxt
pushd ../sarl3/ruby
ruby oaf_to_cxt.rb ../../fca_debian/packages{-sat.oaf,.lex,-sat.cxt}
popd

# load the result
pushd ../sarl3/ruby
ruby lattice_viewer_app.rb &
popd

