
/************************> ENGINEER TOOLS DOMAIN <****************************

*     Copyright(c) 2009 CaridianBCT, Inc. All rights reserved

*     Author:      Dan Slauson (x4664)

*     Class name:  N/A

*     File name:   stringTable.cpp

*     Description:

	This file provides implementation of functions for the base class
	string lib language tables.

*************************< FOCUSSED GUI DOMAIN >*****************************
* $Header: E:/BCT_Development/Trima5.R/Trima/engr_tools/guiTools/stringLib/rcs/stringTable.cpp 1.2 2009/05/20 22:33:54Z dslausb Exp dslausb $
*
* $Log: stringTable.cpp $
* Revision 1.2  2009/05/20 22:33:54Z  dslausb
* Update header information
*
*****************************************************************************/

#include <stdio.h>
#include "string_id.h"
#include "stringTable.h"

const char * stringTable::getLanguageStr(const char* language)
{
	if (strcmp(language, "string") == 0) {
		return "en-us";
	}
	else {
		return language + 7;
	}
}

void stringTable::stripStringJunk(string& returnString)
{
	int returnStringLen = returnString.size();
	char strippedStr[2048];
	int strippedStrIndex = 0;

	// Strip wacky characters that don't play nice with CSV formatting.
	for (int strIndex = 0; strIndex < returnStringLen; strIndex++,strippedStrIndex++)
	{
		switch (returnString[strIndex])
		{
		case '\n':
			strippedStr[strippedStrIndex++]='\\';
			strippedStr[strippedStrIndex]='n';
			break;

		case '\t':
			strippedStr[strippedStrIndex++]='\\';
			strippedStr[strippedStrIndex]='t';
			break;

		case ',':
			strippedStr[strippedStrIndex]=';';
			break;

		default:
			strippedStr[strippedStrIndex] = returnString[strIndex];
			break;
		}
	}

	strippedStr[strippedStrIndex] = '\0';

	returnString = strippedStr;
}

string stringTable::getString(TEXT_STRING_ID item)
{
	if (item < TEXT_NULL || item >= TEXT_DEF_END)
	{
		fprintf(stderr, "Request for bad text item %d\n", item);
		return string("");
	}

	return _literalTable[item];
}


string stringTable::getStringFromLang(string langHashCode, TEXT_STRING_ID item)
{
	if (item == textEmpty) return "";

	if (_tables.find(langHashCode) == _tables.end() )
	{
		fprintf(stderr, "Unable to find key \"%s\"\n", langHashCode.c_str());
		return "bummer.";
	}

	string returnString = _tables[langHashCode]->getString(item);
	stripStringJunk(returnString);
	return returnString;
}

