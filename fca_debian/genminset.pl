#!/usr/bin/perl

use strict;

my $TRUE = 1;
my $FALSE = 0;

#my $p = "cdebconf-0.87";
my $p = "gnucash-1.8.10-18";
my $infile = "$p.reduced.cnf";
my $lexfile = "packages.lex";
my $outfile = "";


# read package names
my %lex = (); my $line; my @splitted = ();
open (FILE, "<$lexfile");
while (<FILE>) {
    $line = $_;
    chomp ($line);
    @splitted = split (" ", $line);
    $lex{$splitted[0]} = $splitted[1];
}
close (FILE);
    

my %int_clauses = ();
my %int_vars = ();
my %stack = ();
my @stack = ();


# read clauses
my %clauses = read_clauses($infile);

# get the package to install
my $installpackage = get_install_package();

# get all conflicting variables and add conflict to int_clauses;
my %vars_con = get_conflicting_vars ();


# loop trough all variables in negative clauses
foreach (keys %vars_con) {
    $int_vars{$_} = check_variable ($_);
    # make every variable from a conflicting clause interesting
    #$int_vars{$_} = $TRUE;
}

open (FILE, ">$p.pruned.cnf");
# output clauses
foreach (keys %int_clauses) {
    print FILE "$_\n";
}
close (FILE);


exit;
# generate large graph
print "dot ";
`ruby clauses_to_dot.rb $p.reduced.cnf packages.lex $p.combined.mapping.txt reasons.txt > $p.dot`;
print "ps ";
`dot -Tps $p.dot > $p.ps`;
print "poster ";
`poster -p1x2A4 $p.ps > $p.poster.ps`;


# checks if variable is "interesting", and then adds the ingoing
# edges to interesting variables to min_clauses
sub check_variable {
    my $me = shift;
    my $var;
    my $interesting = $FALSE;

    print my_stack()." ($me)\n";

    # I'm already visited --> loop!
    if (on_stack ($me)) {
	print "loop \n";
	return $TRUE;
    }
	
    # I've been visited --> return result
    if (exists $int_vars{$me}) {
	print "visited \n";
	return $int_vars{$me};
    }

    if ($me == $installpackage) {
	print "INSTALL \n";
	# we reach the package we want to install --> interesting
	$int_vars{$me} = $TRUE;
	return $TRUE;
    }

    my %asc_vars = get_asc_vars($me);
    my $noof_asc_vars = keys %asc_vars;
    if ($noof_asc_vars < 1) {
	print "no in-edges \n";
	# we have reached a package which has no in-edges --> not interesting
	$int_vars{$me} = $FALSE;
	return $FALSE;
    }

    my_push ($me);
    foreach $var (keys %asc_vars) {
	# check all in-edges
	if (check_variable ($var)) {
	    # is interesting --> add clause
	    add_clause ($me, $var);
	    # me is also interesting
	    $interesting = $TRUE;
	}
    }
    my_pop ($me);

    $int_vars{$me} = $interesting;
#    print "\n";

    return $interesting;
}


sub on_stack {
    my $clause = shift;
    foreach (@stack) {
	if ($_ == $clause) {
	    return $TRUE;
	}
    }
    return $FALSE;
}
sub my_pop {
    my $clause = shift;
    pop (@stack);
}
sub my_push {
    my $clause = shift;
    push (@stack, $clause);
}
sub my_stack {
    return join (" ", @stack);
}


# adds all clauses, which say that var2 depends on var1
sub add_clause {
    my $var1 = shift;
    my $var2 = shift;
    foreach (%clauses) {
	if (contains_var ($_, $var1) && contains_var ($_, -$var2)) {
	    $int_clauses{$_} = 1;
	}
    }
}




# returns the package which should be installed
sub get_install_package {
    foreach (keys %clauses) {
	if ($_ =~ m/^[0-9]* *$/){
	    return $_;
	}
    }
}


# get all variables which we can reach through in-edges
# INPUT: a variable
# OUTPUT: a set of variables
sub get_asc_vars {
    my $invar = shift;
    my %outvars = ();
    my $clause;
    my @vars;
    foreach $clause (%clauses) {
	if (!is_conflict($clause) && contains_var($clause, $invar)) {
	    # not a conflict and contains invar
	    @vars = split (" ", $clause);
	    foreach (@vars) {
		# add all negative vars
		if ($_ < 0) {
		    $outvars{abs($_)} = 1;
		}
	    }
	}
    }
    return %outvars;
}


# returns TRUE, if variable is in clause with given sign
sub contains_var {
    my $clause = shift;
    my $var = shift;
    my @vars = split (" ", $clause);
    foreach (@vars) {
	if ($_ == $var) {
	    return $TRUE;
	}
    }
    return $FALSE;
}

# returns all variables which are in conflicts
# INPUT: a set of clauses (global variable %clauses)
# OUTPUT: a set of variables
sub get_conflicting_vars {
    my $clause;
    my %vars_con = ();
    my @vars = ();
    foreach $clause (keys %clauses) {
	if (is_conflict($clause)) {
	    # add clause to interesting clauses
	    $int_clauses{$clause} = 1;
	    # add vars
	    @vars = split (" ", $clause);
	    foreach (@vars) {
		$vars_con{abs($_)} = 1;
	    }
	}
    }
    return %vars_con;
}


# checks if a clause is a conflict
# INPUT: a clause
# OUTPUT: TRUE or FALSE
sub is_conflict {
    my $clause = shift;
    my @vars = split (" ", $clause);
    foreach (@vars) {
	if ($_ > 0) { 
	    return $FALSE;
	}
    }
    return $TRUE;
}

# reads clauses from file
# INPUT: filename
# OUTPUT: set of clauses
sub read_clauses {
    my $infile = shift;
    my %clauses = ();
    my $line;
    open (FILE, "<$infile");
    while (<FILE>) {
	$line = $_;
	chomp ($line);
	$clauses{$line} = 1;
    }
    close ($infile);
    return %clauses;
}
