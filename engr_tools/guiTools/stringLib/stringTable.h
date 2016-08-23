
/************************> ENGINEER TOOLS DOMAIN <****************************

*     Copyright(c) 2009 CaridianBCT, Inc. All rights reserved

*     Author:      Dan Slauson (x4664)

*     Class name:  N/A

*     File name:   stringTable.h

*     Description:

	This file provides a base class for the string lib language tables.

*************************< FOCUSSED GUI DOMAIN >*****************************
* $Header: E:/BCT_Development/Trima5.R/Trima/engr_tools/guiTools/stringLib/rcs/stringTable.h 1.2 2009/05/20 22:12:27Z dslausb Exp dslausb $
*
* $Log: stringTable.h $
* Revision 1.2  2009/05/20 22:12:27Z  dslausb
* Generalize for other tools alarm table
* Revision 1.1  2009/05/20 18:40:45Z  dslausb
* Initial revision
*
*****************************************************************************/

#ifndef __STRING_TABLE_H__
#define __STRING_TABLE_H__

#include <map>
#include <string>
#include "string_id.h"

using namespace std;

class stringTable; // Forward declaration

typedef map<string,stringTable*> TABLE_CONTAINER;

class stringTable
{
protected:

	static TABLE_CONTAINER _tables;

	map<int,string> _literalTable;

	virtual void init(string langHashCode) = 0;

	string getString(TEXT_STRING_ID item);

	static void stripStringJunk(string& returnString);

public:
    
	static string getStringFromLang(string langHashCode, TEXT_STRING_ID item);
	static const char * getLanguageStr(const char* language);
};

#endif // #ifndef __STRING_TABLE_H__


