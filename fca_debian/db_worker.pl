#!/usr/bin/perl

$MAX_RUNNING_TIME = 20 * 60 * 60;  # 20 hours
$WORKDIR="/home/kde/rja/debian";
$DB="debian";
$DB_HOST="bugs";
$DB_WORD=`cat $WORKDIR/db-pass`;
$DB_USER=`cat $WORKDIR/db-user`;
$HOSTNAME=`hostname`;
chomp($DB_USER);
chomp($DB_WORD);
chomp($HOSTNAME);

my $PID;

use DBI();
use Time::Local 'timelocal_nocheck';

open (LOGFILE, ">>$WORKDIR/db_worker_$HOSTNAME.log");
print LOGFILE get_date()."\n";
close (LOGFILE);

# check if another script/solver is running
if (am_i_running("db_worker_".$HOSTNAME.".pid")) {
  # another one is running, check how long it is running on that job
  $dbh = DBI->connect("DBI:mysql:database=$DB;host=$DB_HOST", $DB_USER, $DB_WORD, {RaiseError => 1, AutoCommit => 1});
  $sth = $dbh->prepare("SELECT start_date,id FROM jobs WHERE state = ? AND machine = ?");
  $sth->execute("running", $HOSTNAME);
  if (@row = $sth->fetchrow_array) {
    $startdate = $row[0];
    $job_id = $row[1];
    $now = get_date();
    # check, if job runs longer than $MAX_RUNNING_TIME
    if(diff_in_seconds($now, $startdate) > $MAX_RUNNING_TIME) {
      # change entry in database to "failed"
      $sth = $dbh->prepare("UPDATE jobs SET state = ?, finish_date = ? WHERE id = ?");
      $sth->execute("failed", get_date(), $job_id);
      # kill job
      kill(15,$PID);
      `killall min_unsat.exe`;
      print "killed job with PID $PID and id $job_id \n";
    } else {
     print "difference is ".diff_in_seconds($now, $startdate)."\n";
    }
  } else {
    print "database gave no running jobs\n";
  }
  $sth->finish;
  $dbh->disconnect;
  exit;
} else {
  print "i am not running\n";
}

# delete unfinished jobs in database
$dbh = DBI->connect("DBI:mysql:database=$DB;host=$DB_HOST", $DB_USER, $DB_WORD, {RaiseError => 1, AutoCommit => 1});
$sth = $dbh->prepare("UPDATE jobs SET state = ?, machine = ? WHERE state = ? AND machine = ?");
$sth->execute("ready", "", "running", $HOSTNAME);
$dbh->disconnect;

# MAIN LOOP START
while (1==1) {
  # get a new job
  $dbh = DBI->connect("DBI:mysql:database=$DB;host=$DB_HOST", $DB_USER, $DB_WORD, {RaiseError => 1, AutoCommit => 0});
  $sth = $dbh->prepare("SELECT job_type,job,batch_date,id FROM jobs WHERE state = ? ORDER BY id LIMIT 1");
  $sth->execute("ready");
  if (@row = $sth->fetchrow_array ) {
    # get data for job
    $job_type = $row[0];
    $job = $row[1];
    $batch_date = $row[2];
    $job_id = $row[3];

    # mark the job as running
    $sth = $dbh->prepare("UPDATE jobs SET state = ?, machine = ?, start_date = ? WHERE id = ?");
    $sth->execute("running", $HOSTNAME, get_date(), $job_id);
    $dbh->commit;
    $dbh->disconnect;
    
    # set batch dir 
    $BATCHDIR="$WORKDIR/$batch_date"; 

    # check which solver to run
    if ($job_type eq "sat") {
      @jobparts = split (" ", $job); 
      # check all packages for installability
      `cd $BATCHDIR; export LD_LIBRARY_PATH=$WORKDIR/sarl3; nice -n 19 $WORKDIR/dump_uninstallable_packages.exe packages.cnf packages.lex $jobparts[0] $jobparts[1] > installable-$jobparts[0]-$jobparts[1].txt`;
      # get all uninstallable packages
      @uninst_packages = ();
      if (open (FILE, "<$BATCHDIR/installable-$jobparts[0]-$jobparts[1].txt")) {
        while (<FILE>) {
          if ($_ =~ /state=Uninstallable$/) {
            if ($_ =~ /^Checking (.*?): num vars/) {
              push (@uninst_packages, $1);
            }
          }
        }
        close (FILE);
        # schedule job for every uninstallable package
        $dbh = DBI->connect("DBI:mysql:database=$DB;host=$DB_HOST", $DB_USER, $DB_WORD, {RaiseError => 1, AutoCommit => 0});
        $sth = $dbh->prepare("INSERT INTO jobs (job_type,job,insert_date,state,batch_date) VALUES (?,?,?,?,?)");
        foreach (@uninst_packages) {
          $sth->execute("min", $_, get_date(), "ready", $batch_date);
        }
        $dbh->commit; 
        $dbh->disconnect;
      } else {
        # job got not executed -> block it
        $dbh = DBI->connect("DBI:mysql:database=$DB;host=$DB_HOST", $DB_USER, $DB_WORD, {RaiseError => 1, AutoCommit => 1});
        $sth = $dbh->prepare("UPDATE jobs SET state = ?, machine = ?, finish_date WHERE id = ?");
        $sth->execute("failed", $HOSTNAME, get_date(), $job_id);
        $dbh->disconnect;
      }
    } else {
      # run min unsat solver    
      `export LD_LIBRARY_PATH=$WORKDIR/sarl3; cd $BATCHDIR; nice -n 19 $WORKDIR/reduce_clauses.exe $job.cnf $job.reduced.cnf $job.mapping.txt`;
      `export LD_LIBRARY_PATH=$WORKDIR/sarl3; cd $BATCHDIR; nice -n 19 $WORKDIR/min_unsat.exe $job.reduced.cnf $job.reduced.min_clauses.cnf`;
      # generate graph
      #`cd $BATCHDIR; ruby $WORKDIR/clauses_to_dot.rb $job.reduced.min_clauses.cnf packages.lex $job.mapping.txt reasons.txt > $job.reduced.min_clauses.dot`;
      #`cd $BATCHDIR; dot -Tpng $job.reduced.min_clauses.dot -o $job.reduced.min_clauses.png`

    }
    # mark job as finished
    $dbh = DBI->connect("DBI:mysql:database=$DB;host=$DB_HOST", $DB_USER, $DB_WORD, {RaiseError => 1, AutoCommit => 1});
    $sth = $dbh->prepare("UPDATE jobs SET state = ?, machine = ?, finish_date = ? WHERE id = ? AND state != ?");
    $sth->execute("finished", $HOSTNAME, get_date(), $job_id, "failed");
    $dbh->disconnect;
  } else {
    $dbh->commit;
    $dbh->disconnect;
    # wait for 10 minutes
    sleep (10*60);
  }
# MAIN LOOP END
} 

# FUNCTIONS

sub get_date {
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
  return $Jahr."-".$Monat."-".$Monatstag." ".$Stunden.":".$Minuten.":".$Sekunden;
}


sub am_i_running {
  $LOCKFILE = shift;

  if (open (FILE, "<$LOCKFILE")) {
    while (<FILE>) {
      $PID = $_;
    }
    close (FILE);
    chomp($PID);

    if (kill(0,$PID)) {
      return 1;
    } 
  }
  open (FILE, ">$LOCKFILE");
  print FILE $$;
  close (FILE);
  return 0;
}
 
# compute difference between two ISO-datetime in seconds (parse sth. like "2005-09-28 08:57:45")
sub diff_in_seconds  {
  $a = shift;
  $b = shift;
  chomp ($a);
  chomp ($b);
  # split strings into numbers
  $a =~ m/([0-9]{4})-([0-9][0-9])-([0-9][0-9]) ([0-9][0-9]):([0-9][0-9]):([0-9][0-9])/;
  $ayear = $1; $amonth = $2-1; $aday = $3; $ahour = $4; $amin = $5; $asec = $6;
  $b =~ m/([0-9]{4})-([0-9][0-9])-([0-9][0-9]) ([0-9][0-9]):([0-9][0-9]):([0-9][0-9])/;
  $byear = $1; $bmonth = $2-1; $bday = $3; $bhour = $4; $bmin = $5; $bsec = $6;
  $atime = timelocal_nocheck($asec,$amin,$ahour,$aday,$amonth,$ayear);
  $btime = timelocal_nocheck($bsec,$bmin,$bhour,$bday,$bmonth,$byear);
  return abs($atime-$btime);
}
