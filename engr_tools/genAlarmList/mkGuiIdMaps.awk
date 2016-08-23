#! /usr/bin/env awk
#
# gawk script for generating CSV file for Trima's GUI Screen and Button IDs
#
# Inputs:
# - First, it reads C/C++ enums from FILE via readEnum() [defined in common.awk]
# - Assumes remaining input is C++ source containing the string mappings
#
# Outputs:
# - Writes two CSV output files:
#     1) guiScreenIds.csv -> mapping for enum GUI_SCREEN_ID
#     2) guiButtonIds.csv -> mapping for enum GUI_BUTTON_ID
# 
# Sample Usage:
#
#   gawk -f mkGuiButtonStrs.awk -f common.awk gui.pp
#
# Where: gui.pp is the compiler's pre-processor output for $(GUI_DIR)/gui.cpp 

BEGIN {
    doVersion = doGuiEnums = 1;
    Version  = "";
    errorOut = "/dev/stderr";
    outFile  = "/dev/stdout"

    setPathSepAndNulFile()
    
    # Set the assumed top level directory
    TOP_DIR = ".." PATHSEP ".." PATHSEP

    # The list of files to parse, relative to TOP_DIR directory
    #
    PROJECT_REVISION_FILE = TOP_DIR "projectrevision"
    # GUI_DIR             = TOP_DIR "gui" PATHSEP
    # GUI_ENUM_FILE       = GUI_DIR "gui.h"
    # GUI_STRS_FILE       = GUI_DIR "gui.cpp"
}

################################################################################
# This block determines the Trima build number
#
doVersion {

    # Parse the trima version number from the PROJECT_REVISION_FILE
    #
    while ( (getline line < PROJECT_REVISION_FILE) > 0 )
    {
        if ( match( line, /\$(ProjectRevision:.*)\$/, vnum ) )
        {
            Version = vnum[ 1 ]
            break
        }
    } 
    close( PROJECT_REVISION_FILE )

    if ( !Version )
    {
        print "___________________________________________________________" > errorOut
        print ""                                                            > errorOut
        print "WARNING: Trima version format unrecognized: assuming 99.99 " > errorOut
        print "___________________________________________________________" > errorOut
        print ""                                                            > errorOut
        Version = "99.99"
    }

    ## print Version > errorOut
    doVersion = 0
}

################################################################################
#
# This block parses the C++ enums from file GUI_ENUM_FILE
#
doGuiEnums {

    GUI_ENUM_FILE = FILENAME
    
    # Get the map of GUI Screen IDs
    #
    printf("Reading GUI_SCREEN_ID from: %s\n", GUI_ENUM_FILE) > errorOut
    numScreenIds = readEnum(GUI_ENUM_FILE, "GUI_SCREEN_ID", 1, ScreenIdEnum)

    # Get the map of GUI Button IDs
    #
    printf("Reading GUI_BUTTON_ID from: %s\n", GUI_ENUM_FILE) > errorOut
    numButtonIds = readEnum(GUI_ENUM_FILE, "GUI_BUTTON_ID", 1, ButtonIdEnum)

    printf("ScreenIdEnum has: %3d entries\n", numScreenIds)  > errorOut 
    printf("ButtonIdEnum has: %3d entries\n", numButtonIds)  > errorOut 

    if (numScreenIds == 0 || numButtonIds == 0) 
    {
       print "ERROR reading enum(s)" > errorOut
       exit 1
    }    

    doGuiEnums = 0
}

################################################################################
#
# Now parse the input (GUI_STRS_FILE) for multiple items ...
#
# Get mapping for GUI_SCREEN_ID:
#
# Sample from scrnIdToStr():
#
#   case GUI_SCREEN_NULL_SCREEN : return "NULL";
#   case GUI_SCREEN_SYSDONOR : return "SYSDONOR";
#   case GUI_SCREEN_SYSCASSETTE : return "SYSCASSETTE";
#   // case GUI_BUTTON_HELP: return "HELP";
#
/^[ \t]+case GUI_SCREEN/ {

   match( $0, /.*(\<GUI_SCREEN_.*\>)[ \t]*:[ \t]*return.*"(.*)"/, map )
   scrnId  = map[1]
   scrnStr = map[2]
   if (scrnId in ScreenIdEnum)
   {
      ScreenIdStrs[scrnId] = scrnStr
      ## printf("ScreenIdStrs[%s] -> '%s'\n", scrnId, scrnStr) > errorOut
   }
}

# Also get the mapping for GUI_BUTTON_ID:
#
# Here we parse the btnIdToStr(). Some samples:
#
#   case GUI_BUTTON_GOBACK : return "GO BACK";
#   case GUI_BUTTON_CANCEL : return "CANCEL";
#   // case GUI_BUTTON_HELP: return "HELP";
#
/^[ \t]+case GUI_BUTTON/ {

   match( $0, /.*(\<GUI_BUTTON_.*\>)[ \t]*:[ \t]*return.*"(.*)"/, map )
   btnId  = map[1]
   btnStr = map[2]
   if ( btnId in ButtonIdEnum )
   {
       ButtonIdStrs[btnId] = btnStr
       ## printf("ButtonIdStrs[%s] -> '%s'\n", btnId, btnStr) > errorOut
   }
}


################################################################################
# This block finalizes everything, writing the output
#
END {
    outFile = "guiScreenIds.csv"
    printComment( "Trima " Version )
    printComment( "Mapping for enum GUI_SCREEN_ID" )
    header = "numValue,enumTag,stringRepr"
    printEnumAsStrs(ScreenIdEnum, ScreenIdStrs, header)
    
    outFile = "guiButtonIds.csv"
    printComment( "Trima " Version )
    printComment( "Mapping for enum GUI_BUTTON_ID" )
    header = "numValue,enumTag,stringRepr"
    printEnumAsStrs(ButtonIdEnum, ButtonIdStrs, header)

}

################################################################################
# Prints an enum mapping as CSV records
#
function printEnumAsStrs(EnumVals, EnumStrs, header)
{
    print header > outFile

    delete xxx
    delete yyy
    
    # Make a map keyed by the enum's numeric value
    for (enum in EnumVals)
    {
        # Store the numeric value as a sortable ASCII string
        numv = sprintf("%03d", EnumVals[enum] + 0)
        xxx[numv] = enum
    }

    # Sort xxx by it's indices, putting result in yyy
    # len = asorti( xxx, yyy, "@ind_num_asc")  # not available with all gawk impls :-(
    len = my_asorti( xxx, yyy )

    # Now iterate over the enums of interest in ascending numerical order
    for (indx=1; indx <= len; indx++)
    {
        numv = yyy[indx]
        enum = xxx[numv]
        if (enum in EnumStrs) text = EnumStrs[enum] ; else text = "UNKNOWN"
        
        printf("%d,%s,%s\n", EnumVals[enum], enum, text) > outFile
    }
}

################################################################################
# Helper functions for printing the XML elements
# 
function printComment( comment )
{
    print "###," comment > outFile
}
