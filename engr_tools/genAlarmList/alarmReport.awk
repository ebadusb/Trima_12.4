#! /usr/env gawk
#
# Create .xml file from the alarm information for a list of languages
# Create .csv file from the alarm information for English-only (for use by QA)
#
# By default, .xml is written to standard out, and .csv to 'alarmList.csv'. 
# To change the defaults, run gawk to set variables xmlFile and/or csvFile.
# Example: gawk -v xmlFile=myAlarms.xml -v csvFile=myAlarms.csv
#
# Invoke using the genAlarmList.bat cmd script, which will first make a list of
# language files found in the current source tree, and then use that list as
# input to this script.
#
# This script does not follow the usual awk procedure of reading lines from an
# input stream and comparing them against a set of rules. Instead, it handles
# its input stream in a single rule using a getline loop. Most of the
# processing is handled by auxiliary scripts, which locate their data from
# source files in the source tree, and passing the results into a rule for that
# script. Hence, this script will pass through all its rules exactly once, and
# the last one will generate the desired xml output file, placed in the
# directory in which it is invoked. This should eliminate the need for
# modifying the generation tools with successive releases of software, as long
# as the location and format of the data does not change significantly :-),
#
# It depends on the following code files:
#       alarm.h
#       alarmtb.cpp
#       alarmcontainer.cpp
#       trimver.h
#       $LANGDIR/string*.info (LANGDIR environment variable is 6.3 and later)
#
# It must be executed from somewhere in a build tree for Trima, but may be
# called from any convenient level in the tree. The top of the tree is expected
# to be ../..

# The following are defined in companion source file common.awk:
# - function chkfields()
# - function setPathSepAndNulFile()
# - function readEnum()


####################################################################################################
# The only file that will be read is stdin, which provides a list of language files. Our processing
# will be handled by first determining in the BEGIN block the path from the current directory to the
# start of the tree for deriving the paths to the files we will read. Subsequent blocks (each executed
# exactly once) will read and collect the data to be output by the last code block.
#
BEGIN {
  doVer = doNames = doLang = doTab = doType = 1;
  Version = ""
  numLangs = 0
  if ( csvFile == "" ) csvFile = "alarmList.csv"
  if ( xmlFile == "" ) xmlFile = "/dev/stdout"
  errorOut = "/dev/stderr" 

  # Determine if this is Unix or DOS
  setPathSepAndNulFile()
    
  # Set the assumed top level directory
  top = ".." PATHSEP ".." PATHSEP
  print "Top of Trima source tree: ", top > errorOut
}


####################################################################################################
# This block determines the trima build number, and will use it as feedback as processing begins
# Read the trima version number; default to 99.99 if it can't be recognized
#
doVer {

  trimver = top "trima_common" PATHSEP "trimver.h"
  while( !Ver && (( getline line < trimver ) > 0 ))
  {
    if( !match( line, / char .*\"(Version [0-9.]*)\"/, vnum )) continue
    Version = vnum[ 1 ]
    print Version                                                       > errorOut
  } 
  close( trimver )

  if( ! Version )
  {
    print "___________________________________________________________" > errorOut
    print ""                                                            > errorOut
    print "ERROR: Trima version format unrecognized: assuming 99.99   " > errorOut
    print "___________________________________________________________" > errorOut
    print ""                                                            > errorOut
    Version = "99.99"
  }

  doVer = 0
}


####################################################################################################
# Build an association between each alarm name and its enumeration value
#
doNames {

  ALARM_ENUM_FILE = top "trima_common" PATHSEP "alarms.h"
  numLines = readEnum(ALARM_ENUM_FILE, "ALARM_VALUES", 0, names)

  # read it again but reverse the mapping to have enum tag as the key
  readEnum(ALARM_ENUM_FILE, "ALARM_VALUES", 1, enumarr)
    
  # Mark the range start and end values for the 3 categories
  # Note: adding 0 to the value makes it get treated as an integer
  rangeBeg[ 1 ] = enumarr[ "FIRST_PROCEDURE_ALARM"   ] + 1
  rangeEnd[ 1 ] = enumarr[ "LAST_PROCEDURE_ALARM"    ] + 0
  rangeBeg[ 2 ] = enumarr[ "FIRST_FATAL_ERROR_ALARM" ] + 1
  rangeEnd[ 2 ] = enumarr[ "LAST_FATAL_ERROR_ALARM"  ] + 0
  rangeBeg[ 3 ] = enumarr[ "FIRST_SAFETY_ALARM"      ] + 0
  rangeEnd[ 3 ] = enumarr[ "LAST_SAFETY_ALARM"       ] + 0

  # Sanity check the ranges were found
  for (indx=1; indx <= 3; indx++)
  {
    if (rangeBeg[indx] == 0 || rangeEnd[indx] == 0)
    {
      printf("ERROR: alarm range %d : Beg=%d or End=%d\n",
             indx, rangeBeg[indx], rangeEnd[indx])
      numLines = 0
    }
  }

  if( 0 == numLines ) 
  {
    print "____________________________________________" > errorOut
    print ""                                             > errorOut
    print "ERROR: Problem with " ALARM_ENUM_FILE         > errorOut
    print "____________________________________________" > errorOut
    print ""                                             > errorOut
    exit 1
  }

  print "alarm names:", numLines > errorOut
  doNames = 0
}


####################################################################################################
# Get the translation table xlatStr and language ID map langId
#
doLang {
  numLangs = readLanguages(langId, xlatStr)
  doLang = 0
}


####################################################################################################
# Find the alarm type (priority) for each alarm, along with its name, display tag and dlog string.
#
doType {
  # Read the association between alarm id and alarm type
  numLines = 0

  # The location of the alarm type table
  ALARM_DATA_FILE = top "trima_common_reentrant" PATHSEP "alarmcontainer.cpp"

  print "Alarm data in:", ALARM_DATA_FILE > errorOut

  # Read each line, looking for 
  awkCmd = "gawk -f parseType.awk " ALARM_DATA_FILE
  while( (awkCmd | getline line) > 0 )
  {
    numLines++
    fields = split( line, info, /,/ )
    if ( ! chkfields( fields, 4, numLines )) exit 3
    key             = info[ 1 ]
    types[ key ]    = info[ 2 ]
    sub( /_/, " ", types[ key ] )   # eliminate _ in the type field
    display[ key ]  = info[ 3 ]
    dlog[ key ]     = info[ 4 ]

    # For each table entry, add an empty status string for each language

    for( langNum = 1; langNum <= numLangs; langNum++ )
    {
      statStr[ key, langId[ langNum ]] = "";
    }
  }
  close( awkCmd )

  print "alarm entries:", numLines > errorOut
  doType = 0
}


####################################################################################################
# Associate each alarm name (string) with the Body and Status strings
# for each language requested.
#
# Inputs:
#   Lines of the form
#           'ALARM_NAME, ALARM_BODY_STRING_ID, STATUS_STRING_ID'
#   extracted from "alarmtb.cpp"
#
#   Array associating each STRING_ID and language identifier with the translated string
#       xlatStr[ id, lang ] <- translated string
#   constructed above by the !Lang rule
#
# Outputs:
#   Arrays associating each ALARM_NAME and language with the ALARM_BODY_STRING
#   and STATUS_STRING
#       bodyStr[ alarm, lang, entryNo ]
#       statStr[ alarm, lang ]
#       bitmapId[ alarm ]
#
doTab {
  numLines = 0

  # It is desired to initialize the status string to an empty string for all
  # alarm/language combinations.

  # Create the alarm <-> string associations for input
  awkCmd = "gawk -f parseTab.awk " top "gui" PATHSEP "alarmtb.cpp"

  # Process each alarm <-> string association
  while( (awkCmd | getline line) > 0 )
  {
    numLines++

    # Parse the input line to get its alarm, body string id, and status string id
    fields = split( line, info, /,/ )
    if ( ! chkfields( fields, 4, numlines )) exit 3
    alarm  = info[ 1 ]
    bodyId = info[ 2 ]
    statId = info[ 3 ]
    bmId   = info[ 4 ]
    more[ alarm ]++     # Some alarms have multiple alarm bodies, so count them
        
    bitmapId[ alarm, more[ alarm ] ] = bmId

    # Search the (2 dimensional) string translation table using a string ID
    # and language as indices
    #
    for ( iLang in langId ) 
    {
      lang = langId[iLang]

      # Add entry to body string array, indexed by the alarm, language, and count
      #
      if ( (bodyId,lang) in xlatStr )
      {
        bodyStr[alarm, lang, more[ alarm ]] = xlatStr[bodyId, lang]
      }
      else if ( bodyId != "TEXT_NULL" )
      {
        print "bodyId not found:" lang "|" bodyId > errorOut
      }

      # If not yet defined, add status string for this alarm and language
      # (count is irrelevant; a given alarm has only one status line)
      #
      if ( statStr[alarm, lang] == "" )
      {
        if ( (statId,lang) in xlatStr )
        {
          statStr[alarm, lang] = xlatStr[statId, lang]
        }
        else if ( statId != "TEXT_NULL" )
        {
          print "statId not found:" lang "|" statId > errorOut
        }
      }
    }
  }
  close( awkCmd )

  print "alarmtb entries:", numLines > errorOut
  doTab = 0
}


####################################################################################################
# Output the XML and CSV files
#
END {
  # Print XML header
  printXmlHeader( xmlFile )

  # Print CSV header
  printf "Trima %s Alarms\n\n", Version > csvFile 
  print "Alarm Id,Alarm Number,Alarm Name,Alarm Type,Data Log Alarm String,Display Order,Display Type,Status Line,Alarm Body Text" > csvFile


  # The numeric alarm numbers lie in three ranges:
  # 1) procedure alarms,
  # 2) fatal alarms
  # 3) safety alarms.
  # The alarm numbers are expected to be
  # contiguous in each of the two ranges.
  # 
  for( range = 1; range <= 3; range++ )
  {
    for( indx = rangeBeg[ range ]; indx < rangeEnd[ range ]; indx++ )
    {
      if( indx in names )
      {
        key = names[ indx ]

        # # Treat the "DISP_NONE" alarms as obsolete or N/A
        # if ( display[ key ] == "DISP_NONE" )
        # {
        #    ## printf "Ignoring DISP_NONE:\t%5s = %s\n", indx, key > errorOut
        #    continue
        # }
                
        # Treat DEAD/UNUSED alarms as obsolet or N/A
        if ( match( key, /DEAD_.*|UNUSED_.*/ ) )
        {
          ## printf "Ignoring DEAD/UNUSED:\t%5s = %s\n", indx, key > errorOut
          continue
        }
                
        # Undefined display types are okay for CSV file (used by QA),
        # but omitted from XML
        if ( display[ key ] == "" )
        {
          ## printf "Including blank display entry:\t%5s = %s\n", indx, key > errorOut
          printCsvAlarm( indx, key )
        }
        else
        {
          printCsvAlarm( indx, key )
          printXmlAlarm( indx, key )
        }
      }
      else break
    }
  }
  print "</dita>" > xmlFile
}


####################################################################################################
# Function to print the xml data for a single alarm. The parameters are the
# alarm name string 'key' and the numeric alarm identifier 'alarmNum'
#
function printXmlAlarm( alarmNum, key )
{
  print "<reference id=\"" key "\">"                        > xmlFile
  print "<title/>\n<refbody>"                               > xmlFile
  print "<section id=\"number\">" alarmNum "</section>"     > xmlFile
  _info = key
  gsub( /_/, " ", _info )
  print "<section id=\"info\">" _info "</section>"          > xmlFile
  print "<section id=\"type\">" types[ key ] "</section>"   > xmlFile
  print "<section id=\"log\">" dlog[ key ] "</section>"     > xmlFile

  # There may be more than one language specified for this alarm. The status
  # lines of each version for the current alarm will be in this section.
  print "<section id=\"name\">"                             > xmlFile
  for( alarm_lang in statStr )
  {
    split( alarm_lang, fld, SUBSEP )
    alarm = fld[ 1 ]
    mLang = fld[ 2 ]
    if( key == alarm )
    {
      print "<ph id=\"name\" otherprops=\"" mLang "\">" statStr[ alarm_lang ] "</ph>" > xmlFile
    }
  }
  print "</section>"                                        > xmlFile

  # There may be more than one alarm body line for each alarm. They will all be
  # printed in this section, for each language specified. The first section
  # will be labeled 'explanation', and any others are identified as
  # 'moreinfoXXX', where XXX is a sequence number.
  for( _indx = 1; _indx <= more[ key ]; _indx++ )
  {
    if( _indx == 1 ) label = "explanation"
    else             label = ( "moreinfo" _indx-2 )
    print "<section id=\"" label "\">"                      > xmlFile
    for( alarm_lang_entry in bodyStr )
    {
      split( alarm_lang_entry, fld, SUBSEP )
      alarm = fld[ 1 ]
      mLang = fld[ 2 ]
      entry = fld[ 3 ]
      if(( key == alarm ) && ( entry == _indx ))
      {
        print "<ph id=\"" label "\" otherprops=\"" mLang "\">" bodyStr[ alarm_lang_entry ] "</ph>" > xmlFile
      }
    }
    print "</section>"                                      > xmlFile
  }
  print "</refbody>\n</reference>"                          > xmlFile   # end-of-section tags for this alarm
}


####################################################################################################
# Function to print the CSV data for a single alarm. The parameters are the
# numeric alarm identifier 'alarmNum' and alarm name string 'key'
#
function printCsvAlarm( alarmNum, key )
{
  _info = key
  gsub( /_/, " ", _info )

  _orig_OFS = OFS
  OFS = ","

  # fix Dlog entry for any XML tags (< and >) and replace commas with semi-colons
  _dlog = dlog[ key ]
  gsub( /&lt;/, "<", _dlog )
  gsub( /&gt;/, ">", _dlog )
  gsub( /,/, ";", _dlog )

  # There may be more than one language specified for this alarm, but
  # only the English-US version will be printed.
  #
  _lang = "en-us"

  # Get the status line. Replace any commas with semi-colons
  _status = statStr[ key, _lang ]
  if ( _status == "" )
    _status = "No Entry. Displayed as System Test Failure."
  else
    gsub( /,/, ";", _status )

  # There may be more than one record of alarm body text for this alarm.
  # Again, only the English-US version will be printed.
  #
  # If more than one explanation applies, subsequent records are prefixed
  # with "(more info)" followed by a sequence number.
  #
  _moreinfo = "(more info),,,,"
  _more_cnt = more[ key ]
  _body     = "No Entry."
  _indx     = 1

  # Determine the 'Display Type'
  _dispType = "Text Only"
  _bmId     = bitmapId[ key, _indx ] 
  if ( _bmId != "" && _bmId != "BITMAP_NULL" ) _dispType = "Text and Bitmap"
  
  if ( _more_cnt == 0 )
  {
    # No body/dlog text provided. Print empty strings for this record

    if ( types[ key ] == "OPERATOR ATTENTION" )
    {
      _dispType = "Operator Info Prompt"
      _body = _status = ""
    }

    print key, alarmNum, _info, types[ key ], _dlog, _indx, _dispType, _status, _body > csvFile
    return
  }
  
  for( _indx = 1; _indx <= _more_cnt; _indx++ )
  {
    # Determine the 'Display Type' (can be more than one possible)
    _dispType = "Text Only"
    _bmId     = bitmapId[ key, _indx ] 
    if ( _bmId != "" && _bmId != "BITMAP_NULL" ) _dispType = "Text and Bitmap"

    # Get English text and replace commas with semi-colons
    _body = bodyStr[ key, _lang, _indx ]

    gsub( /,/, ";", _body )
    
    ## print key, _body > "body.str"

    if ( _indx == 1 )
    {
      print key, alarmNum, _info, types[ key ], _dlog, _indx, _dispType, _status, _body > csvFile
    }
    else
    {
      _bmId = bitmapId[ key, _indx ] 
      if ( _bmId != "" && _bmId != "BITMAP_NULL" )
          _dispType = "Text and Bitmap"
      print _moreinfo, _indx, _dispType, _status, _body > csvFile
    }
  }
  
  OFS = _orig_OFS
}
