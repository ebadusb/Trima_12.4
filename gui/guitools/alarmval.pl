########
######## Trima Alarm breakout program: alarm name; enum value; description; type ########
########
######## T. Wahl - 10/5/1999
########
######## Input files: alarms.h, an_alr_h.cpp (from Trima proc directory)
########              MUST BE PRESENT in the same directory as this PERL program.
########
######## Output file:  alrmval.txt (CSV, quoted string record)
########


#
######## INITIALIZATION ########
#

$TRUE  = 1;
$FALSE = 0;
$STRING_LINE_COMMENT = "//" ;
$OPEN_WIGGLY = "{" ;

### enumeration symbol and its numeric value
%hashtab_enum_val = () ;

### enumeration symbol and its text description
%hashtab_enum_text = () ;

### enumeration symbol and its text rep. of type (e.g. A1, A2, R1...)
%hashtab_enum_type = () ;



#
######## FILES ########
#

### File (input) containing alarm enums and text descriptions
$ALFILE_enum_text       = "<alarms.h" ;

### File (input) containing the alarm types
$ALFILE_type            = "<an_alr_h.cpp" ;

### File (output) containing the alarm enum, enum number, type, description
$ALFILE_report          = ">alrmval.txt" ;




#
######## Open the first input file ########
#

### Open the update file
 open (ENUM_VALS, $ALFILE_enum_text)
      or die "Couldn't open the alarm enum values file (" . $ALFILE_enum_text . "): $!\n" ;


### Find, and post all the alarm enums and calculated their corresponding values.

$READING_ENUMS = $FALSE ;
$READING_TEXT  = $FALSE ;
$CURRENT_ENUM_NUMBER = 0 ;

ENUMVAL_LOOP : while (defined($enum_line = <ENUM_VALS>)) {

   chomp $enum_line ;

#print $enum_line . "\n" ;

   ### toss comments and blank lines
   if ($enum_line =~ m/^\s*$STRING_LINE_COMMENT/i || $enum_line =~ m/^\s*$/i) {
      next ENUMVAL_LOOP;
   }

   ### check for mode changes i.e. in enum defs (or out); in alarm list (or out)...

   ### we've entered the enum structure
   if ($enum_line =~ m/^\s*enum\s+ALARM_VALUES/i) {

      $READING_ENUMS = $TRUE ;
      next ENUMVAL_LOOP;
   }

   ### we've left the enum structure
   if ($enum_line =~ m/^\s*NULL_ALARM\s*$/i) {

      $READING_ENUMS = $FALSE ;
      next ENUMVAL_LOOP;
   }

   ### we've entered the alarm list structure
   if ($enum_line =~ m/^\s*the_alarm_struct/i) {

      $READING_TEXT = $TRUE ;
      next ENUMVAL_LOOP;
   }

   ### we've left the alarm list structure
   if ($enum_line =~ m/^\s*$OPEN_WIGGLY\s*NULL_ALARM/i) {

      $READING_TEXT = $FALSE ;
      next ENUMVAL_LOOP;
   }



   ### Process the alarms according to mode i.e. READING_ENUM or READING_TEXT

   if ($READING_ENUMS) {

      $enum_line =~ m/^\s*(\w+)[,=]/i ;

      if (!defined($1)) {
         print "Undefined enum line encountered = " . $enum_line . "\n";
         next ENUMVAL_LOOP;
      }
      else
      {
        $current_enum = $1 ;

        ### check for the safety demarcation
        if ($current_enum eq "FIRST_SAFETY_ALARM") {

           $enum_line =~ m/=(\d+)/i ;
           if (!defined($1)) {
              print "Undefined first-safety-alarm number = " . $enum_line . "\n";
              exit 999 ;
           }
           else
           {
              $CURRENT_ENUM_NUMBER = $1 ;
           }
        } # END OF: if ($current_enum eq "FIRST_SAFETY_ALARM") {


        $hashtab_enum_val{$current_enum} = $CURRENT_ENUM_NUMBER ;
        $CURRENT_ENUM_NUMBER++ ;
      } # END OF: "else" for "if (!defined($1)) {


   } # END OF:  if ($READING_ENUMS) {...



   if ($READING_TEXT) {

      $enum_line =~ m/^\s*$OPEN_WIGGLY\s*(\w+)\s*,\s*\"(.*)\"/i ;

      $current_enum = $1 ;
      $current_text = $2 ;

      if (!defined($1) || !defined($2)) {
         print "Undefined alarm enum text = " . $enum_line . "\n";
         exit 990 ;
      }
      else
      {
        $hashtab_enum_text{$current_enum} = $current_text ;
      }


   } # END OF:  if ($READING_TEXT) {...


  } # END OF: while (defined($enum_line = <ENUM_VALS>)) {

  close ENUM_VALS ;



#
######## Open the second input file ########
#

 ### Open the alarm table file
 open (ENUM_TYPE, $ALFILE_type)
      or die "Couldn't open the alarm type file (" . $ALFILE_type . "): $!\n" ;


### Find, and post all the alarm types

$READING_TYPES = $FALSE ;


ENUMTYPE_LOOP : while (defined($enum_line = <ENUM_TYPE>)) {

   chomp $enum_line ;


#print $enum_line . "\n" ;

   ### toss comments and blank lines
   if ($enum_line =~ m/^\s*$STRING_LINE_COMMENT/i || $enum_line =~ m/^\s*$/i) {
      next ENUMTYPE_LOOP;
   }

   ### we've entered the alarm table structure
   if ($enum_line =~ m/^\s*aContainer_Alarms\s+anAlarm_Handler::alarms/i) {

      $READING_TYPES = $TRUE ;
      next ENUMTYPE_LOOP;
   }

   ### we've left the alarm table structure
   if ($enum_line =~ m/^\s*aContainer_Alarms\s*\(\s*NULL_ALARM/i) {

      $READING_TYPES = $FALSE ;
      next ENUMTYPE_LOOP;
   }


   ### store all of the alarm types
   if ($READING_TYPES) {

      $enum_line =~ m/^\s*aContainer_Alarms\s*\(\s*(\w+)\s*,\s*(\w+)/i ;

      $current_enum = $1 ;
      $current_type = $2 ;

      ### add the type of the alarm to the type hash table
      $hashtab_enum_type{$current_enum} = $current_type ;

   } # if ($READING_TYPES) {


} # END OF:  while (defined($enum_line = <ENUM_TYPE>)) {


### close the enum type file
close ENUM_TYPE ;



#
######## Open the report file for output ########
#

### Open the alarm table file
 open (REPORT_TYPE, $ALFILE_report)
      or die "Couldn't open the report file (" . $ALFILE_report . "): $!\n" ;

### Create heading line

   $report_line =  "\"# Alarm type\",\"Alarm number\",\"Alarm enum.\",\"Alarm description\"\n" ;

   printf REPORT_TYPE "%s", $report_line ;

### Get all the enum symbols and their cooresponding values
foreach $enum_key ( sort( keys( %hashtab_enum_val ) ) ) {

   ### extract the enum key
   $curr_enum = $enum_key ;

   ### extract the enum value
   $curr_enum_val = $hashtab_enum_val{$curr_enum} ;

   ### extract the enum text
   $curr_enum_text = $hashtab_enum_text{$curr_enum} ;

   ### extract the enum type
   $curr_enum_type = $hashtab_enum_type{$curr_enum} ;

   if ($curr_enum_type eq "OPERATOR_ATTENTION") {
       $curr_enum_type = "OP_ATT" ;
   }


   ### create a report line

   $report_line = $curr_enum_type .
                   "," .
                  $curr_enum_val  .
                   "," .

                   "\"" .
                  $curr_enum      .
                   "\"" .

                   "," .

                   "\"" .
                  $curr_enum_text .
                   "\"" .

                   "\n" ;

   printf REPORT_TYPE "%s", $report_line ;


} # END OF: foreach $enum_key ( sort( keys( %hashtab_enum_val ) ) ) {

close REPORT_TYPE ;


