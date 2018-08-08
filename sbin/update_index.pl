#!/usr/bin/perl
# written for multilevel pki tool
# 2018
# Klaus Dieter Wolfinger
# www.itconsulting-wolfinger.de
# (C) 

$openssl = '/usr/bin/openssl';
$crt_filename = $ARGV[0];
$index_filename = "root-index.txt";
# $index_filename = $ARGV[1];
open(OPENSSL, "$openssl x509 -noout -serial -subject -enddate -in $crt_filename |") ||
  die "Can't open $openssl: $!";
  
print "Starting CRL Update with file $ARGV[0]...\n";

while(<OPENSSL>) {
  chomp;
  ($key, $value) = split(/\s*=\s*/, $_, 2);
  $key = uc($key);
  if ($key eq "NOTAFTER") {
    ($mon,$day,$time,$year,$stuff) = split(/ +/, $value, 5);
    $time =~ s/://g;
    %Months = ( 'Jan' => '01', 'Feb' => '02', 'Mar' => '03', 'Apr' => '04',
                'May' => '05', 'Jun' => '06', 'Jul' => '07', 'Aug' => '08',
                'Sep' => '09', 'Oct' => '09', 'Nov' => '11', 'Dec' => '12' );
    $value = sprintf(qq[%s%02d%02d${time}Z], substr($year,2,2), $Months{$mon}, $day);
  }
  #printf(qq[%s="%s"\n], $key, $value);
  $$key = $value;
}
close(OPENSSL);

print "Updating $index_filename...";

open(DB, ">> $index_filename") || die "Can't open $index_filename: $!";
print DB "V\t$NOTAFTER\t\t$SERIAL\tunknown\t$SUBJECT\n";
close(DB);

print "done.\n";