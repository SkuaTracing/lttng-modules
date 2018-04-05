#!/usr/bin/perl

use strict;

my $file = $ARGV[0];
my $str = do {
    local $/ = undef;
    open my $fh, "<", $file
        or die "could not open $file: $!";
    <$fh>;
};

# my $str = '';
my $regex = qr/(?<!\\$\n)^LTTNG_TRACEPOINT_EVENT(?:(?:_CODE)?(?:_MAP)?\(|_INSTANCE(?:_MAP)?\(\w+,\s*)(?<event>\w+?),/mp;

my @matches = ( $str =~ /$regex/g );

# use Data::Dumper;
# print Dumper(\@matches);

while(my $elem=shift(@matches))
{
	print "$file:$elem\n";
}

# if ( $str =~ /$regex/g ) {
#   print "Whole match is ${^MATCH} and its start/end positions can be obtained via \$-[0] and \$+[0]\n";
#   # print "Capture Group 1 is $1 and its start/end positions can be obtained via \$-[1] and \$+[1]\n";
#   # print "Capture Group 2 is $2 ... and so on\n";
# }

# ${^POSTMATCH} and ${^PREMATCH} are also available with the use of '/p'
# Named capture groups can be called via $+{name}
