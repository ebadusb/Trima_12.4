#!/usr/bin/perl
################################################################################
#
# mkAuthorMap.pl [filespec | p4datespec] [p4creds] 
#
# This script generates a file-to-author mapping to standard out. The files to
# map are set by: 1) auto-genereated using DOS dir command for all C/C++ files,
# 2) a specified file list, or 3) Perforce query
#
# Options:
#
# filespec is ONE of the following to specify pathnames to be mapped:
#
#  -a       : auto-generate the file via DOS: dir /b /s *.cpp *.c *.hpp *.h
#  -f FILE  : the file of paths to map (either file system or depot paths)
#
# p4datespec is ONE of the following for doing a Perforce change query:
#
#  -n NDAYS : integer number of days to go back
#  -d DATE  : date string formatted as: YYYY/MM/DD
#  -l LABEL : Perforce label from which DATE will be inferred
#
# p4creds are optional Perforce credentials (default uses environment vars):
#
#  -c P4CLIENT : client name
#  -u P4USER   : username 
#  -P P4PASSWD : password
# 
################################################################################

use strict;
use feature "say";
use Time::localtime;
use Time::Piece;
use Getopt::Std;

# Input file containing files to be mapped
my $SrcFileList = "";

################################################################################
# Constants for Trima paths, Eclipse project and depot paths
#
my $TRIMA_SRC_DIR  = "Main";
my $TRIMA_PROJECT  = qq(TrimaMain);
my $TRIMA_DEPOT    = qq(//depot/main/embedded/Trima/$TRIMA_SRC_DIR);
my $p4ClientRoot   = GetP4ClientRoot();
my $TRIMA_ABS_PATH = qq($p4ClientRoot/Trima/$TRIMA_SRC_DIR);

# Variables for P4 datespec query
my $now    = localtime;
my $nowStr = $now->strftime("%Y/%m/%d %H:%M:%S");
my $nDays  = 0;
my $begYMD = "";
my $begLbl = "";

# Defaults for the Perforce credentials
my $p4creds = "";
my $client  = $ENV{"P4CLIENT"};
my $login   = $ENV{"P4USER"};
my $passwd  = $ENV{"P4PASSWD"};

# If doing a P4 query, output file will generated for Parasoft -include option
my $mkIncludeFile   = 0;
my $PsoftIncludeLst = "changed_files.lst";
my $P4ERR_FILE      = "p4errs.txt";

################################################################################
# Read command-line options
#
my %options = ();
getopts("c:u:P:n:d:l:f:a", \%options);

$client = $options{c} if defined $options{c};
$login  = $options{u} if defined $options{u};
$passwd = $options{P} if defined $options{P};

# Build the Perforce credentials if provided
$p4creds .= qq(-c $client ) if $client ne "";
$p4creds .= qq(-u $login )  if $login  ne "";
$p4creds .= qq(-P $passwd ) if $passwd ne "";

#
# Print the XML header now so that any debug output will in a comment block
#
say qq(<?xml version="1.0" encoding="utf-8"?>);
say "<!--";

################################################################################
# This simplistic parsing of mutually exclusive options
# imposes the following precedence: -a, -f, -n , -d, -l
#
if (defined $options{a})
{
    $SrcFileList = "allsrc.dos";
    system qq(cmd /c dir /b /s *.cpp *.c *.hpp *.h >$SrcFileList);
    say qq(Using DOS to auto-generate: $SrcFileList);
}
elsif (defined $options{f})
{
    $SrcFileList = $options{f};
    say qq(Mapping source files from: $SrcFileList);
}
elsif (defined $options{n})
{
    # We'll compute th begin date from nDays ago
    $nDays = $options{n};

    die "Invalid! nDays must be [1,30], stopped" if ($nDays < 0 || $nDays > 30);

    my $nDaysAgoSecs = $now - $nDays*24*60*60;
    my $ago = localtime($nDaysAgoSecs);
    $begYMD = $ago->strftime("%Y/%m/%d");
    ## say qq(nDays=$nDays nDaysAgoSecs=$nDaysAgoSecs ago=$ago beg=$begYMD);
    say qq(nDays : $nDays);
}
elsif (defined $options{d})
{
    # The begin date is assumed to be in YYYY/MM/DD format
    $begYMD = $options{d};
}
elsif (defined $options{l})
{
    # Get the begin date from the label
    $begLbl = $options{l};
    my $p4info = `p4 $p4creds labels -m 1 -e $begLbl`;
    my @result = split(" ", $p4info);
    $begLbl = @result[1];
    $begYMD = @result[2];
    say qq(Label : $begLbl ==> begYMD: $begYMD);
}
else
{
    die "No valid options specified! stopped";
}

# Do the Perforce query?
if ( $begYMD =~ m/201[3-9]\/[01]?[0-9]\/[0123]?[0-9]/ )
{
    # Generate the input file of files changed since $begYMD using Perforce query
    $SrcFileList = "changed_files.p4";
    system qq(p4 $p4creds files $TRIMA_DEPOT/...\@$begYMD,\@now > $SrcFileList);

    say qq(P4Beg : $begYMD);
    say qq(P4End : $nowStr);
    say qq(P4Cred: $p4creds);

    $mkIncludeFile = 1;
}
elsif ( $begYMD ne "" )
{
    die "Invalid p4 datespec? begYMD=$begYMD ... stopped";
}


################################################################################
# Returns the author (username) for the input file by consulting Perforce. 
# The filename can be either a Perforce depot path, or an absolute filesystem
# path. With depot format, the optional revision spec can be used. For example:
#
# //depot/embedded/Trima/Main/projectrevision#2  --> author for rev 2
# //depot/embedded/Trima/Main/projectrevision    --> author for latest/head rev
# C:\Dev\trima\Trima\Main\projectrevision        --> author for latest/head rev
#
# If the file has only single rev, it's possibly was branched, in which case,
# it will look futher back to get the last author with a rev > 1
#
sub GetAuthor
{
    my $fileName = shift(@_);
    my $author   = "unknown";
    my $newest   = "tbd";
    my $revnum   = 0;
    my $action   = "tbd";
    my $change   = 0;
    my $cnt      = 0;

    # List of explicit change numbers that should be ignored for authorship:
    # 55074 : 'Reformat Trima Code' using uncrustify
    # [55320, 55324, 55326, 55309]
    #       : Change of file-type attribute to remove RCS keyword expansion
    my $ignoreChanges = '(55074|55320|55324|55326|55309)';

    my $flog = `p4 $p4creds filelog -i "$fileName" 2>>$P4ERR_FILE`;

    my @lines = split("\n", $flog);

    foreach (@lines)
    {
        chomp;

        ## Perforce sample output:
        ## //depot/main/embedded/Trima/Main/projectrevision
        ## ... #2 change 55098 edit on 2013/08/12 by jheiusb@jheiusb_TrimaWS_01 (text) 'blah blah'
        ## ... ... branch into //depot/main/embedded/Trima/__Dev__/DaveBranches/9863/projectrevision#1
        ## ... ... branch into //depot/main/embedded/Trima/__Dev__/DaveBranches/PredictRefactor/projectrevision#1
        ##
        if (m/(^... #)([0-9]+)( change )([0-9]+) ([a-z]+) on ([0-9\/]+) by (.*)(@.*) /)
        {
            $revnum = $2;
            $change = $4;
            $action = $5;
            $author = lc($7);

            # Cache if this is the first (newest) author found
            if (++$cnt == 1) { $newest = $author; }

            # Return now if a specifc rev was specified or it's the initial revision:
            return $author if ($fileName =~ m/.*#[0-9]+/);
            return $author if ($action eq "add");
            
            # Check if this change should be ignored
            next if ($change =~ m/$ignoreChanges/);

            # If we found a revnum > 1, break out of the loop;
            # else, keep looking back to the previous branches
            #
            last if $revnum > 1;
        }
    }

    # If we never found a revnum > 1, then use the first/newest author found
    $author = $newest if ($revnum == 1);

    return $author;
}

#################################################################################
# Returns the P4 client root as an absolute file system path
#
sub GetP4ClientRoot
{
    my $root  = "tbd";
    my $info  = `p4 $p4creds info`;
    my @lines = split("\n", $info);
        
    foreach (@lines)
    {
        if (m/Client root:/)
        {
            $root = (split())[2];
            $root =~ s|\\|/|g;
            last;
        }
    }
    return $root;
}

################################################################################
# Open our I/O files
#
open(INFILE,  "< $SrcFileList")   || die "Could not open $SrcFileList, stopped";
if ($mkIncludeFile)
{
    open(OUTFILE, "> $PsoftIncludeLst") || die "Could not open $PsoftIncludeLst, stopped";
}

unlink $P4ERR_FILE;

################################################################################
# Now start the generation of XML authorship mapping (written to standard output)
#
say "-->";
say "<authorship>";

################################################################################
# The following loop iterates through all the files and assigns owners to each.
#
my $lnum = 0;     # line number counter
while (<INFILE>)
{
    # Set for DOS line separator if necessary on the first line
    if (++$lnum == 1 && m/\r/ ) { $/ = "\r\n"; }

    chomp;

    my $fileName = $_;

    # Change the slashes to Unix
    $fileName =~ s|\\|/|g;

    # Ignore these subdirectories:
    next if ($fileName =~ m/engr_tools|unit_test|guitest/);
    next if ($fileName =~ m/vxworks_sim|vxworks_target\/.*\//);

    my $path = "tbd";
    my $psoft_path = "tbd";

    # If this a depot file path ...
    if (m/$TRIMA_DEPOT/)
    {
        # Extract the depot path. Ex: //depot/main/embedded/Trima/Main/cds/software_cds.cpp
        # Here, we retain the optional rev spec
        $_ = $fileName;
        $fileName = (split(" "))[0];

        # Here, we lop off the optional rev spec.
        $path = (split("#"))[0];

        # And convert to an Eclipse path: /TrimaMain/cds/software_cds.cpp
        $path =~ s/$TRIMA_DEPOT/\/$TRIMA_PROJECT/;
    }
    else # assume it's an absolute filesystem path
    {
        # Skip if file doesn't exist
        ## next unless (-e $fileName);   #### not working with cygwin Perl?

        # Convert to an Eclipse path
        $path = $fileName;
        $path =~ s/$TRIMA_ABS_PATH/\/$TRIMA_PROJECT/;
    }

    # Skip if file is not C/C++ src:
    next unless m/.*\.[ch]|.*\.[ch]pp/;

    my $author = GetAuthor($fileName);

    say qq(   <file author="$author" path="$path" />);

    if ( $mkIncludeFile )
    {
        # Write to the Parasoft file include: **/cds/software_cds.cpp
        $psoft_path = $path;
        $psoft_path =~ s/\/$TRIMA_PROJECT/\*\*/;
        say OUTFILE ($psoft_path);
    }
}

say "</authorship>";
##say "</mapping>";
