#! /bin/sh
#
# create_baseline: creates stringxx.cpp file from translation file and stringen.cpp 
#
#
#ifdef __USAGE
#usage:
#%C -i <infile> -l <languagename> -o <outfile>
#
#where:
# -i <infile> the input translation file
# -l <languagename> the name of the language translated to
# -p <langprefix> the single letter used to represent the language
# -o <outfile> the output filename - stringxx.cpp where xx is two letters 
#              signifying the language
# -u flag denoting that this is a UCS2 encoded language
#
#endif

#
# usage message routine
#
#

print_usage ()
{
   echo " "
   echo "create_baseline -i <infile> -l <languagename> -o <outfile>"
   echo " "
   echo "where:"
   echo " -i <infile> the input translation file"
   echo " -l <languagename> the name of the language translated to"
   echo " -p <langprefix> the single letter used to represent the language"
   echo " -o <outfile> the output filename - stringxx.cpp where xx is two" 
   echo "              letters signifying the language"
   echo " -u flag denoting that this is a UCS2 encoded language"
   echo " "
   exit 1
}


# main script

PATH=.:/bin:/usr/bin

#
# get the options
#

iflag=0
oflag=0
lflag=0
pflag=0
uflag=0 

infile=""
outfile=""
language=""
langprefix="" 

while getopts ?i:o:l:p:u opt $*
do
    case $opt in
         i)  infile=$OPTARG ; iflag=1 ;;
         o)  outfile=$OPTARG ; oflag=1 ;;
         l)  language=$OPTARG ; lflag=1 ;;
         p)  langprefix=$OPTARG ; pflag=1 ;;
         u)  uflag=1 ;;
        \?) echo ; echo ; print_usage ;;
    esac
done

#
# test the options - no defaults except for outfile
#

if test $iflag -eq 0
then
   echo No Translation File Specified
   exit 0
fi
if test $lflag -eq 0
then
   echo No Language Specified!
   exit 0
fi
if test $oflag -eq 0
then
   echo No Output File Specified!
   echo Writing to File stringxx.cpp
   outfile="stringxx.cpp"
fi
if test $pflag -eq 0
then
   echo No Language Prefix Specified
   exit 0
fi
echo Input File is \"$infile\"
echo Language is \"$language\"
echo Output File is \"$outfile\"
echo Prefix is \"$langprefix\"
#
# run the awk script
#
if test $uflag -eq 1
then
   awk -f seg_UCS2.awk < $infile > tempin.cpp
else
   cp $infile tempin.cpp
fi
awk -f ILEtoCPP.awk -v LNAME=$language -v LANG=$langprefix tempin.cpp stringen.cpp > $outfile 
rm tempin.cpp

