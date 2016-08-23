
/************************> ENGINEER TOOLS DOMAIN <****************************

*     Copyright(c) 2009 CaridianBCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     File name:   genReasonTable.cpp

*     Description:

*		This is a Win32 utility that generates a CSV
		file that maps reason codes to the text displayed
*		in the GUI.

*************************> ENGINEER TOOLS DOMAIN <****************************
*	
*	$Header: E:/BCT_Development/Trima5.R/Trima/engr_tools/reason_table/rcs/genReasonTable.cpp 1.1 2009/03/09 22:15:18Z dslausb Exp $
*	$Log: genReasonTable.cpp $
*	Revision 1.1  2009/03/09 22:15:18Z  dslausb
*	Initial revision
*
****************************************************************************/

#include "stdio.h"
#include <iostream>
#include "reasonMap.h"
#include "verifyflags.h"
#include "trimver.h"
#include "stringTable.h"

TABLE_CONTAINER stringTable::_tables; // Must be here before langTableHeader.h so static globals
									  // are initialized in the right order

#include "langTableHeader.h"

// getStringIdFromReasonCode define in gui/reasonToSTring.cpp
TEXT_STRING_ID getStringIdFromReasonCode(ENUMERATED_REASON_TO_SHOW reasonToShow);

// _reason_map defined trima_common/reasonMap.h
extern REASON_MAPPING _reason_map[];

ENUMERATED_REASON_TO_SHOW getShowReasonFromReason(ENUMERATED_REASON reason)
{
	for (int reasonMapIndex = 0; _reason_map[reasonMapIndex].reason != NULL_REASON; reasonMapIndex++)
	{
		if (_reason_map[reasonMapIndex].reason == reason)
			return _reason_map[reasonMapIndex].show;
	}

	return SHOW_NOTHING;
}

// The main entry point
int main(int argc, char* argv[])
{
	char csvFileName[256];

	if (argc < 3)
	{
		fprintf(stderr, "Useage: genReasonTbl -CSV <name>\n-or-\ngenReasonTbl -XML <names>\n", argc);
		return -1;
	}

	if (strcmp(argv[1], "-CSV") == 0)
	{
		sprintf(csvFileName, "%s.csv", argv[2]);

		fprintf(stdout, "Generating reason file %s.\n", csvFileName);

		// Create the File Name.
		FILE *outFile = fopen(csvFileName,"w");

		// Make sure it opened OK.
		if (outFile == 0)
		{
			fprintf(stderr, "Unable to open CSV file %s\n", csvFileName);
			return -2;
		}

		// Label the top of the CSV file.
		fprintf(outFile, "Trima %s Reason Codes\n\n", DCL_TEXT_PWRUP_VERSION);

		// Output column labels
		fprintf(outFile, "Reason Name, Reason Id, Reason String\n");

		for (int reasonCode = 1; reasonCode < LAST_REASON; reasonCode++)
		{
			ENUMERATED_REASON_TO_SHOW showReason = getShowReasonFromReason((ENUMERATED_REASON)reasonCode);

			if (showReason < REASON_TO_SHOW_FIRST_ITEM ||
				showReason >= REASON_TO_SHOW_END_OF_LIST)
			{
				cerr << "Unable to find reason code " << reasonCode << " in map" << endl;
				continue;
			}

			TEXT_STRING_ID stringId = getStringIdFromReasonCode(showReason);

			if (stringId <= TEXT_NULL ||
				stringId >= TEXT_DEF_END)
			{
				cerr << "Invalid string ID " << stringId << " returned for reason code " << reasonCode << endl;
				continue;
			}

			string reasonString = stringTable::getStringFromLang(argv[2], stringId);

			fprintf(outFile, "%s, %d, %s\n",
					ENUMERATED_REASON_NAME(reasonCode),
					reasonCode,
					reasonString.c_str());
		}

		fprintf(stdout, "Reason file %s generated successfully.\n", csvFileName);

		//close the file
		fclose (outFile);
	}
	else if (strcmp(argv[1], "-XML") == 0)
	{
		fprintf(stderr, "TODO: XML not implemented yet\n");
	}
	else
	{
		fprintf(stderr, "Useage: genReasonTbl -CSV <name>\n-or-\ngenReasonTbl -XML <names>\n", argc);
		return -1;
	}

	return 0;
}


