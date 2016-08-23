/************************> ENGINEER TOOLS DOMAIN <****************************

*     Copyright(c) 2009 CaridianBCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     File name:   StringCrcGen.cpp

*     Contents:    Main routine for command line tool

*     Description: This is the source for a command line tool that compiles
		in the source from gui/stringInfoCrc.cpp and calls it to spit out a
*		CRC for a string.info file.  All functionality is in that file.  This
		just wraps it in a simple console app.
*
*************************< ENGINEER TOOLS DOMAIN >****************************
* 
*	$Header: E:/BCT_Development/Trima5.R/Trima/engr_tools/guiTools/stringCrcGen/rcs/stringCrcGen.cpp 1.2 2009/03/23 19:21:56Z dslausb Exp dslausb $
*	$Log: stringCrcGen.cpp $
*	Revision 1.2  2009/03/23 19:21:56Z  dslausb
*	Chop special characters out of CRCs.
*	Revision 1.1  2009/03/23 16:31:10Z  dslausb
*	Initial revision
*	Initial revision
*
******************************************************************************/

#include <iostream>
#include "crcgen.h"
using namespace std;

// Codify the errors
enum returnTypes
{
	HOORAY,
	BAD_COMMAND_LINE,
	BAD_FILE_PATH,
	CRC_GEN_FAILURE
};

// Here's the prototype for the function in gui/stringInfoCrc.cpp
unsigned long getStringInfoCrc ( const char* pathName, const char* logPath = NULL );

// Check to see if the file exists and can be read.
// (Open it as read-only, check the pointer, close it).
bool fileExists(char* fileName)
{
	if (!fileName)
	{
		cerr << "File name point is NULL." << endl;
		return false;
	}

	FILE* stringInfoFile = fopen(fileName, "r");

	if (!stringInfoFile)
	{
		cerr << "Unable to open file \"" << fileName << "\"." << endl;
		return false;
	}

	fclose(stringInfoFile);
	return true;
}

// Main function. Expects one argument containing the
// path to a string.info language file.  Outputs the CRC
// to standard output.
int main(int argc, char* argv[])
{
	// Check the number of arguments
	if (argc < 2)
	{
		cerr << "Useage: stringCrcGen <filename> <filename> <...>" << endl;
		return BAD_COMMAND_LINE;
	}

	/* If you want to, you can log a bunch of extra output here.
	char logFileName[256];
	sprintf(logFileName, "%s.log", argv [1]);
	*/

	for (int argIndex = 1; argIndex < argc; argIndex++)
	{
			
		// Make sure the file exists and can be opened for reading
		if (!fileExists(argv[argIndex]))
		{
			cerr << "Unable to open path " << argv[argIndex] << endl;
			continue;
		}

		// Get the CRC value (this function is from gui/stringInfoCrc.cpp)
		unsigned long crcVal = getStringInfoCrc(argv[argIndex]);

		// Make sure we got a valid CRC value.
		if (crcVal)
		{
			cout << "Crc for " << argv[argIndex] << ":" << hex << crcVal << endl;
		}
		else cerr << "CRC generation failed for " << argv[argIndex] << endl;
	}

	// Hooray!!!!
	return HOORAY;
}


