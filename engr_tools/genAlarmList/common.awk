#! /usr/bin/env awk

################################################################################
# Determine if Unix or DOS and set PATHSEP and NULFILE accordingly
#
function setPathSepAndNulFile()
{
   # Determine if this is Unix or DOS
   "cd" | getline pwd
   close( "cd" )
   if ( "" == pwd )
   {
      # Use Unix pwd command
      "pwd" | getline pwd
      close( "pwd" )
      PATHSEP = "/"
      NULFILE = "/dev/null"
   }
   else
   {
      PATHSEP = "\\"
      NULFILE = "nul"
   }
}

################################################################################
# Checks that the number of fields seen on a line is correct
#
function chkfields( fields, expect, numLines )
{
   if (fields != expect)
   {
      printf( "ERROR: saw %d fields, expected %d\n", fields, expect) > errorOut
      printf( "%d: %s\n", numLines, line) > errorOut
   }
   return (fields == expect)
}

################################################################################
# This function parses a file to create a map from the specified enum,
# where:
#   fileName : the input file to parse
#   enumName : the enum string to look for
#   mapStyle : output mapping spec for enumeration tag and numeric value pairs:
#              0 -> mapOut[num] = tag 
#              1 -> mapOut[tag] = num
# 
function readEnum(fileName, enumName, mapStyle, mapOut)
{
   numPairs = 0
   awkCmd = "gawk -f parseEnum.awk -v enum_name=" enumName " " fileName
   ## print awkCmd
   while( (awkCmd | getline line) > 0 )
   {
      numPairs++
      nFields = split(line, kv, /,/)
      if ( ! chkfields( nFields, 2, numPairs ) ) exit 3

      num = kv[1] + 0 # enum numeric value (add 0 to make it a number)
      tag = kv[2]     # enum tag/name

      if (mapStyle == 0)
         mapOut[ num ] = tag
      else
         mapOut[ tag ] = num
        
      ## printf("%4d : %s\n", num, tag)
   }
   close( awkCmd )

   return numPairs
}

################################################################################
# Make a table of the languages to be processed.
#
# Inputs:
#   Language file names are read from the input pipe. The English file is
#   named string.info, while any other languages are named string_xx-yy.info,
#   where xx identifies a language and yy a locale.
#
#   Assumes the files exist under the directory defined by environment
#   variable LANGDIR.
#
#   Parse each language file to identify each string identifier for that
#   language with the displayable string value.
#
# Outputs:
#   langId[i] : contains the xx-yy language/locale ids
#   xlatStr[strId, langId] : GUI strings mapped by string & language ids
#
function readLanguages(langId, xlatStr)
{
   nLang = 0
   do
   {
      nLang++
      sub( /gui[/]/, "" )
      languages[ nLang ] = $0
   } while( getline > 0 )
    

   # Make sure at least one language string file was provided
   if( nLang == 0 )
   {
      print "_______________________________" > errorOut
      print ""                                > errorOut
      print "ERROR: No language files found." > errorOut
      print "_______________________________" > errorOut
      print ""                                > errorOut
      exit 1
   }

   # Read the association between alarm id and language string for each
   # language file Infer the language name from the file name.
   #
   print "Processing files for the following languages: " > errorOut
   for( indx in languages )
   {
      numLines = 0
      language = languages[indx]
        
      # Infer the languageId from the file name.
      if( match( language, /string.*.info/ ) )
      {
         langId[ indx ] = substr( language, 8, 5 )

         # For some unfortunate reason, the en-us strings are in a file
         # called 'string.info' so special processing is necessary
         if( langId[ indx ] ~ /info/ ) langId[ indx ] = "en-us"
      }
      else
      {
         print "_______________________________________________" > errorOut
         print ""                                                > errorOut
         print "ERROR: invalid language file name:", language    > errorOut
         print "_______________________________________________" > errorOut
         print ""                                                > errorOut
         exit 2
      }

      # Get path to the language file from environment variable
      #
      langFile  = ENVIRON[ "LANGDIR" ] PATHSEP language

      printf "%d) %s : %s\n", indx, langId[indx], langFile       > errorOut
        
      # Associate the string id for each string with its translated value
      #
      awkCmd = "gawk -f parseLang.awk " langFile 
      while( (awkCmd | getline line) > 0 )
      {
         numLines++
         fields = split( line, info, /@!@/ )
         if ( ! chkfields( fields, 2, numLines ) ) exit 3
         strId  = info[ 1 ]
         strVal = info[ 2 ]
         gsub( /"/, "", strVal )

         xlatStr[strId, langId[indx]] = strVal
      }
      close( awkCmd )
   }

   return nLang
}

################################################################################
# Print the XML header for a DITA schema
#
function printXmlHeader( filename )
{
   print "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" > filename
   print "<!DOCTYPE dita PUBLIC \"-//OASIS//DTD DITA Composite//EN\" \"ditabase.dtd\">" > filename
   print "<dita>" > filename
}

################################################################################
# Sorts an array by its keys (indices). Returns N, the number of elements.
# On return, keys[] contains the sorted keys of src[], indexed from 1 to N.
#
# This is my implementation of the asorti() function that comes with gawk 3.1.6;
# sadly, VxWorks 6.9 uses gawk 3.1.0 which doesn't support asorti(). 
#
function my_asorti(src, keys)
{
   # Copy the keys of x into numerically indexed array 
   cnt = 0
   for (indx in src)
   {
      cnt++
      keys[cnt] = indx
   }

   # Now sort the keys 
   asort(keys)

   return cnt
} 