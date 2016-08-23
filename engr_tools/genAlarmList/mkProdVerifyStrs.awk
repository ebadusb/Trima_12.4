#! /usr/bin/env awk
#
# gawk script for generating XML for Trima's product verification labeling
# and reason codes.
#
# See the list of files (XXX_FILE) below that are used for parsing
#

# As @include is not supported by all gawk, add '-f common.awk' to gawk command
#@include "common.awk"

BEGIN {
    doVersion = doVerifInfo = doReasonInfo = doLang = 1;
    Version  = "";
    errorOut = "/dev/stderr";
    
    if ( xmlFile == "" ) xmlFile = "/dev/stdout"

    setPathSepAndNulFile()
    
    # Set the assumed top level directory
    top = ".." PATHSEP ".." PATHSEP
    print "Top of Trima source tree: " top > errorOut

    # The list of files to parse, relative to top directory
    #
    PROJECT_REVISION_FILE = top "projectrevision"
    TRIMA_COMMON_DIR      = top "trima_common" PATHSEP
    GUI_DIR               = top "gui" PATHSEP
    VERIFY_ENUM_FILE      = TRIMA_COMMON_DIR "verifyflags.h"
    REASON_ENUM_FILE      = TRIMA_COMMON_DIR "reasonMap.h"
    REASON_STRS_FILE      = GUI_DIR "reasonToString.cpp"
    RUNSUM_PROD_FILE      = GUI_DIR "runsum_product.cpp"
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

    print Version > errorOut
    doVersion = 0
}

################################################################################
# This block reads the Verification enums and button strings
#
doVerifInfo {

	# Sadly, not all gawk versions support length() on an array; hence, counters
	#
	numBtns = 0
	numVFlag = 0
	
    # Get the map of verification flags
    #
    numPairs = readEnum(VERIFY_ENUM_FILE, "ENUMERATED_FLAG", 1, VerifEnum)

    # Parse the RUNSUM_PROD_FILE for multiple items ...
    #
    while ( (getline line < RUNSUM_PROD_FILE) > 0 )
    {
        # Get mapping from LOGICAL_BUTTON to the TEXT_STRING_ID (the text
        # put on verification buttons)
        #
        # Sample from LogicalButtonCollection::getTextForLogicalButton():
        #
        # case LB_COUNT_PLATELET_PRODUCT : return textButtonFlagPltWbcs;
        #
        if ( match( line, /case[ \t]*(\<LB_.*\>)[ \t]*:[ \t]*return.*(\<text.*\>)/, map ) )
        {
            btnId = map[1]
            txtId = map[2]
            VerifBtns[btnId] = txtId
            numBtns++
            ## printf("VerifBtns['%s'] -> '%s'\n", btnId, txtId) > errorOut
        }

        # Also get the mapping between VERIFY flags and LOGICAL button ID
        #
        # Here we parse the TheVerifyFlagToLogicButtonMap[]
        #
        else if ( match( line, /{[ \t]*(\<.*\>),[ \t]*(\<LB_.*\>)[ \t]*}/, map ) )
        {
            vFlag = map[1]
            btnId = map[2]
            if ( vFlag in VerifEnum )
            {
                VFlag2Btn[ vFlag ] = btnId
                numVFlag++
                ## printf("VFlag2Btn['%s'] -> '%s'\n", vFlag, btnId) > errorOut
            }
        }
    }
    close(RUNSUM_PROD_FILE)

    print "VerifEnum has:\t", numPairs > errorOut 
    print "VerifBtns has:\t", numBtns  > errorOut
    print "VFlag2Btn has:\t", numVFlag > errorOut

    doVerifInfo = 0
}

################################################################################
# This block gets the reason and product verification strings
#
doReasonInfo {

	# Sadly, not all gawk versions support length() on an array; hence, counters
	#
	numStrs = 0
	numHdrs = 0
	numVerif = 0

    # Get the map of reason enumerations
    #
    numPairs = readEnum(REASON_ENUM_FILE, "ENUMERATED_REASON_TO_SHOW", 1, ReasonEnum)

    # Parse the REASON_STRS_FILE for a number of items ...
    #
    kk = 0
    while ( (getline line < REASON_STRS_FILE) > 0 )
    {
        # Get the mapping from ENUMERATED_REASON_TO_SHOW enums to TEXT_STRING_ID
        #
        # Sample from getStringIdFromReasonCode():
        #
        # case SHOW_CENTRIFUGE_STOPPED : return textProdVerifReasonCentrifugeStopped;
        #
        if ( match(line, /case[ \t]*(\<SHOW_.*\>)[ \t]*:[ \t]*return.*(\<text.*\>)/, map) )
        {
            enum = map[1]
            text = map[2]
            ReasonStrs[enum] = text
            numStrs++
            ## printf("ReasonStrs['%s'] -> '%s'\n", enum, text) > errorOut
        }

        # Get mapping from ENUMERATED_FLAG to header displayed for a reason
        # Note that VerifEnum[] array must already be initialized
        #
        # Sample from getReasonHeaderIdFromVerifyFlag():
        #
        # case VERIFY_PLATELET_WBCS :     return textProdVerifReasonHeaderPurity;
        # case CONC_OUT_OF_RANGE :        return textProdVerifReasonHeaderPltConc;
        #
        if ( match(line, /case[ \t]*(\<.*\>)[ \t]*:[ \t]*return.*(\<.*ReasonHeader.*\>)/, map) )
        {
            enum = map[1]
            text = map[2]
            if ( enum in VerifEnum )
            {
                ReasonHdrs[ enum ] = text
                numHdrs++
                ## printf("ReasonHdrs['%s'] -> '%s'\n", enum, text) > errorOut
            }
        }
        
        # Get the various mappings from GUI_SCREEN_ID to the TEXT_STRING_ID for
        # the strings that are fixed per each of the Product Verifcation Screens
        #
        # Samples from:
        #   getProdVerifyCategory(),
        #   getProdVerifyDoLabel(),
        #   getProdVerifyDontLabel(),
        #   getProdVerifyStatusLine()
        #
        # PVSMap_Category[GUI_SCREEN_RUNSUM_PLASMA] = textProdVerifLeukLabelPlasma;
        # PVSMap_DoLabel[GUI_SCREEN_RUNSUM_PLASMA]  = textProdVerifLeukMsgPlasLeukoreduced;
        # PVSMap_DontLabel[GUI_SCREEN_RUNSUM_PLT]   = textProdVerifLeukMsgPltNotLeukoreduced;
        # PVSMap_StatusLine[GUI_SCREEN_RUNSUM_RBC]  = textStatEndOfRunRbc;
        #
        if ( match(line, /PVSMap_(.*)\[(GUI_.*)\].*=.*(\<text.*\>)/, map) )
        {
            type = map[1]
            scrn = map[2]
            text = map[3]
            ## print scrn, type, text > errorOut

            InfoTypes[type] = 1
            ScreenIds[scrn] = 1
            ProdVerif[scrn, type] = text
            numVerif++
        }
    }
    close( REASON_STRS_FILE )

    print "ReasonEnum has:\t", numPairs > errorOut
    print "ReasonStrs has:\t", numStrs  > errorOut
    print "ReasonHdrs has:\t", numHdrs  > errorOut
    print "ProdVerif  has:\t", numVerif > errorOut
    
    doReasonInfo = 0
}

################################################################################
# Get the translation textId map (LangTextMap) and the language ID map (LangIds)
#
doLang {
    numLangs = readLanguages(LangIds, LangTextMap)
    ## print "LangIds has:\t", numLangs     > errorOut
    doLang = 0
}

################################################################################
# This block finalizes everything, writing the output
#
END {
    printXmlHeader( xmlFile )
    printProdVerifScreenInfo()
    printProdVerif()
    printReasonStrs()
    
    print "</dita>" > xmlFile
}

################################################################################
# Prints the Product Verification screen info as XML
#
function printProdVerifScreenInfo()
{
    printComment( "Product Verification Screen Info" )

    # InfoTypes[type] = 1
    # ScreenIds[scrn] = 1
    # ProdVerif[scrn, type] = text

    si_len = my_asorti( ScreenIds, si_keys )
    it_len = my_asorti( InfoTypes, it_keys )
    
    for (si=1; si <= si_len; si++)
    {
        scrn = si_keys[si]

        # Here we lop off the GUI_ prefix from screen enum
        printRefBeg( gensub( /GUI_/, "", 1, scrn ) )

        printSection( "type", "ProductScreenInfo" )

        for (it=1; it <= it_len; it++)
        {
            type = it_keys[it]
            printStrings( type, ProdVerif[scrn, type] )
        }

        printRefEnd()
    }
}

################################################################################
# Prints the Product Verification strings as XML
#
function printProdVerif()
{
    delete xxx
    delete yyy

    # Iterate over the Verify flags that have button strings
    for (enum in VFlag2Btn)
    {
        # Store the numeric value as a sortable ASCII string
        numv = sprintf("%02d", VerifEnum[enum] + 0)
        xxx[numv] = enum
    }

    # Sort xxx by it's indices, putting result in yyy
    len = my_asorti(xxx, yyy)

    printComment( "Verification Flags" )

    # Loop over the VerifEnums of interest in ascending order
    for (indx=1; indx <= len; indx++)
    {
        enum = xxx[ yyy[indx] ]
        
        # Here we lop off the VERIFY_ prefix from enum
        printRefBeg( gensub( /VERIFY_/, "", 1, enum ) )

        printSection( "type",   "VerifyFlag" )
        printSection( "enum",   enum )
        printSection( "number", VerifEnum[enum] )
        printStrings( "buttonText", VerifBtns[VFlag2Btn[enum]] )
        printStrings( "reasonHeader", ReasonHdrs[enum] )

        printRefEnd()
    }
}

################################################################################
# Prints the Reason strings as XML
#
function printReasonStrs()
{
    printComment( "Reason Codes" )

    delete xxx
    delete yyy
    
    # We only care about the reasons to which a string is mapped
    for (enum in ReasonStrs)
    {
        # Store the numeric value as a sortable ASCII string
        numv = sprintf("%02d", ReasonEnum[enum] + 0)
        xxx[numv] = enum
    }

    # Sort xxx by it's indices, putting the sorted keys in yyy
    # len = asorti( xxx, yyy, "@ind_num_asc")  # not available with all gawk impls :-(
    len = my_asorti( xxx, yyy )

    # Now iterate over the reasons of interest in ascending numerical order
    for (indx=1; indx <= len; indx++)
    {
        enum = xxx[ yyy[indx] ]

        # Here we lop off the SHOW_ prefix from enum
        printRefBeg( gensub( /SHOW_/, "", 1, enum ) )

        printSection( "type",   "Reason" )
        printSection( "enum",   enum )
        printSection( "number", ReasonEnum[enum] )
        printStrings( "text",   ReasonStrs[enum] )

        printRefEnd()
    }
}

################################################################################
# Helper functions for printing the XML elements
# 
function printComment( comment )
{
    print "<!-- " comment " -->" > xmlFile
}

function printRefBeg(id)
{
    # begin a reference element
    print "<reference id=\"" id "\">"   > xmlFile
    print " <title/>"                   > xmlFile
    print " <refbody>"                  > xmlFile
}

function printRefEnd()
{
    # end of reference element
    print " </refbody>"    > xmlFile
    print "</reference>"   > xmlFile
}

function printSection(id, val)
{
    if (val == "")
        print "  <section id=\"" id "\">"                       > xmlFile
    else
        print "  <section id=\"" id "\">" val "</section>"      > xmlFile
}

function printStrings(id, textId)
{
    # Print the section header for this id
    printSection( id, "" )
    
    # Here we pring the textId itself (for diagnostics only; may remove)
    print "   <!-- textId=" textId " -->"

    # Here we print the corresponding string values for each language
    for( ii in LangIds )
    {
        langId = LangIds[ii]
        
        if ((textId, langId) in LangTextMap)
            printLangVal( langId, LangTextMap[textId, langId] )
        else
            print "No LangTextMap entry for: " langId, textId > errorOut
    }

    # Print the section footer
    print "  </section>"   > xmlFile
}


function printLangVal(langId, textVal)
{
    print "   <ph id=\"text\" otherprops=\"" langId "\">" textVal "</ph>" > xmlFile
}

