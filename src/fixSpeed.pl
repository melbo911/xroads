#!/usr/bin/perl
#
# run though the roads.net file and change car speed to 50% by default
#
# reads file from STDIN
#

$n = int($ARGV[0]);
$n = 50 if ( $n == 0 );

#print "n = $n\n";

while(<STDIN>) {
   if ( /^CAR_DRAPED/ || /^CAR_GRADED/ ) {
      @t = split(' ');
      if ( defined $t[3] ) {
         $t[3] = int( ($t[3] / 100) * $n );
      }
      $t[3] = 1 if ( $t[3] < 1 );
      
      $x = shift(@t);
      print "$x\t",join(' ',@t),chr(0x0d),chr(0x0a);
   } else {
      print $_;
   }
}
