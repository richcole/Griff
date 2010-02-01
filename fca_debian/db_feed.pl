#!/usr/bin/perl

$WORKDIR="/home/kde/rja/debian";
$DB="debian";
$DB_HOST="localhost";
$DB_USER=`cat $WORKDIR/db-user`;
$DB_WORD=`cat $WORKDIR/db-pass`;
chomp($DB_WORD);
chomp($DB_USER);


$SAT_SIZE = 500;	# how many packages to check for installability by one job

# get date
my ($Sekunden, $Minuten, $Stunden, $Monatstag, $Monat,
    $Jahr, $Wochentag, $Jahrestag, $Sommerzeit) = localtime(time);
$Monat       += 1;
$Jahrestag   += 1;
$Monat        = $Monat     < 10 ? $Monat     = "0".$Monat     : $Monat;
$Monatstag    = $Monatstag < 10 ? $Monatstag = "0".$Monatstag : $Monatstag;
$Stunden      = $Stunden   < 10 ? $Stunden   = "0".$Stunden   : $Stunden;
$Minuten      = $Minuten   < 10 ? $Minuten   = "0".$Minuten   : $Minuten;
$Sekunden     = $Sekunden  < 10 ? $Sekunden  = "0".$Sekunden  : $Sekunden;
$Jahr        += 1900;
my $now      = $Jahr."-".$Monat."-".$Monatstag." ".$Stunden.":".$Minuten.":".$Sekunden;
my $now_date = $Jahr."-".$Monat."-".$Monatstag;

# generate batch dir
$BATCHDIR = "$WORKDIR/$now_date";
mkdir ($BATCHDIR, 0777);

# get packages with ruby script
print "getting packages ... ";
`cd $BATCHDIR; ruby $WORKDIR/download_packages.rb`;

# generate CNF
print "generating CNF ... ";
`export LD_LIBRARY_PATH=$WORKDIR/sarl3; cd $BATCHDIR; zcat *.gz | $WORKDIR/packages_to_cnf`;

# count number of packages
print "counting packages ... ";
$NO_OF_PACKAGES=`zcat $BATCHDIR/*.gz | grep -e "^Package: " | wc -l`;

use DBI();
$dbh = DBI->connect("DBI:mysql:database=$DB;host=$DB_HOST", $DB_USER, $DB_WORD, {RaiseError => 1, AutoCommit => 1});
$sth = $dbh->prepare("DELETE FROM jobs WHERE batch_date = ?");
$sth->execute($now);
$sth = $dbh->prepare("INSERT INTO jobs (job_type,job,insert_date,state,batch_date) VALUES (?,?,?,?,?)");

print "inserting into database ... ";
# insert jobs into database
for ($i=1; $i<=$NO_OF_PACKAGES; $i=$i+$SAT_SIZE) {
  $sth->execute("sat", $i." ".($SAT_SIZE-1), $now, "ready", $now_date);
}

$dbh->disconnect();

print "finished!\n";
