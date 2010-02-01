#!/usr/bin/perl

my $GRAPHTYPE="png";
my $WORKDIR="/home/kde/rja/debian";
my $HOST="studie.kde.informatik.uni-kassel.de";
my $WORD=`cat $WORKDIR/z-pass`;
my $USER=`cat $WORKDIR/z-user`;
chomp ($WORD);
chomp ($USER);

# generate dot files and the graphs, extract package information from package files

use strict;
use File::Find;
use Net::FTP;

my $DIR;
my %PACKAGES = ();
my @description = ();
my $package;
my $version;
my $complete;
my $line;
my $ftp;

# directory name as argument
if ($#ARGV != 0) {
  print "usage:   generate_graphinfo.pl DIR\n";
} else {
  # get dir
  $DIR = shift @ARGV;
  # find all the packages
  print "getting all package names ... \n";
  find(\&generate, ($DIR));
  # unzip package description 
  print "uncompressing package description ...\n";
  `zcat $DIR/unstable-*gz > $DIR/packages.txt`;
  # connect to Zope
  print "connecting to $HOST ...\n";
  $ftp = Net::FTP->new($HOST, Debug => 0, Port => 10021) or die "Cannot connect to $HOST: $@";
  $ftp->login($USER, $WORD) or die "Cannot login ", $ftp->message;
  # create new directory and change into it
  print "creating directory on $HOST ...\n";
  $ftp->mkdir("/kde/debian/$DIR") or die "error creating directory on server : $@";
  $ftp->cwd("/kde/debian/$DIR") or die "error changing to directory on server : $@";
  print "reading package descriptions ... \n";
  # read packagedescription
  open (FILE, "<$DIR/packages.txt");
  while (<FILE>) {
    $line = $_;
    if ($line =~ m/^Package: /) {
      # new package
      @description = ();           # saves all the lines
      push (@description, $line);
      $line =~ m/^Package: (.*)/;  # extract package name
      $package = $1;
      chomp ($package);
    } elsif ($line =~ m/^Version: /) {
      # save version number
      push (@description, $line);
      $line =~ m/^Version: (.*)/;  # extract version number
      $version = $1;
    } elsif ($line =~ m/^$/) {
      # next package
      $complete = "$package-$version";
      if (exists $PACKAGES{$complete}) {
        # is uninstallable 
        print "$complete: txt(";
        # generate description file
        # remove colons from filename (for http-compliance)
        my $description_filename = "$complete.description.txt";
        $description_filename =~ s/:/_/g;
        if (not -e "$DIR/$description_filename") {
          # file does not exist -> generate it
          open (DESC, ">$DIR/$description_filename") || die "could not open $DIR/$description_filename for writing\n";
          foreach $line (@description) {
            print DESC $line;
            print ".";
          }
          close (DESC);     
        } else {
          print "-";
        }
        print ") ";
        # reduce clauses
        if (not -e "$DIR/$complete.combined.mapping.txt") { 
          # file does not exists -> generate it
          `export LD_LIBRARY_PATH=$WORKDIR/sarl3; cd $DIR; $WORKDIR/sarl3/reduce_clauses.exe $complete.reduced.min_clauses.cnf $complete.reduced.min_clauses.reduced.cnf $complete.reduced.mapping.txt`;
          `cd $DIR; cat $complete.mapping.txt $complete.reduced.mapping.txt > $complete.combined.mapping.txt`;
          print "red ";
        } else {
          print "- ";
        }
        # generate dot file
        if (not -e "$DIR/$complete.reduced.min_clauses.dot") {
          # file does not exists -> generate it
          `cd $DIR; ruby $WORKDIR/clauses_to_dot.rb $complete.reduced.min_clauses.reduced.cnf packages.lex $complete.combined.mapping.txt reasons.txt > $complete.reduced.min_clauses.dot`;
          print "dot ";
        } else {
          print "- ";
        }
        # generate graph file
        # remove colons from filename (for http-compliance)
        my $graph_filename = "$complete.$GRAPHTYPE";
        $graph_filename =~ s/:/_/g;
        if (not -e "$DIR/$graph_filename") {
          # file does not exists -> generate it
          `cd $DIR; dot -T$GRAPHTYPE $complete.reduced.min_clauses.dot -o $graph_filename`;
          print "$GRAPHTYPE ";
        } else {
          print "- ";
        }
        # put files on server 
        $ftp->binary;
        $ftp->put("$DIR/$description_filename");
        $ftp->put("$DIR/$graph_filename");
        print "ftp\n";
      } 
    } else {
      push (@description, $line);  # save line 
    }  
  }
  close (FILE);
}
# copy all $GRAPHTYPE to dilbert
#`ncftpput -P 10021 -u $USER -p $WORD $HOST kde/debian/$DIR $DIR/*.$GRAPHTYPE`;
# copy all descriptions to dilbert
#`ncftpput -P 10021 -u $USER -p $WORD $HOST kde/debian/$DIR $DIR/*.description.txt`;



# collect package names
sub generate {
  return unless -f $_;
  my $file = $_;
  if ($file =~ m/reduced.min_clauses.cnf$/) {
    $file =~ m/(.*?).reduced.min_clauses.cnf/;
    # remember package
    $PACKAGES{$1} = 1;
  }
}

