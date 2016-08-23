////////////////////////////////////////////////////////////////////////////
// Copyright(c) 2002 Gambro BCT, Inc.  All rights reserverd
//
// Author:       Regis McGarry
//
// Class Name:   trima_time
//
// Description:
// This class provides the ability to change time on Trima.
//
// $Log: trima_time.h $
// Revision 1.4  2002/08/26 20:59:30Z  rm70006
// Fixed bug using static datalog constructors.
// Revision 1.3  2002/08/16 16:51:15Z  rm70006
// Added data logging to class.
//
////////////////////////////////////////////////////////////////////////////


#include <time.h>


class trima_time
{
public:
   // Sets time on Trima.  When call returns, OS time has been changed and time
   // change message has been sent out.
   static void set (const time_t& t);
};

/* FORMAT HASH 7c8a011b71041c91ecc3c8edc140af24 */
