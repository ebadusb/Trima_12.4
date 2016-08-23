/*******************************************************************************
 *
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      cds.h
 *             Common Data Store Unit Test 
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file declares an example Common Data Store
 *
 * HISTORY:    $Log: cds.h $
 * HISTORY:    Revision 1.6  2002/10/21 17:42:02Z  rm70006
 * HISTORY:    Update unit tests.
 * HISTORY:    Revision 1.5  2002/09/13 20:10:17Z  rm70006
 * HISTORY:    Updated unit test.
 * HISTORY:    Revision 1.4  2002/09/03 14:38:52Z  rm70006
 * HISTORY:    make test cds multi-write
 * HISTORY:    Revision 1.3  2002/06/17 18:46:36Z  rm70006
 * HISTORY:    Changed cds test class to match changes made to datastore class.
 * HISTORY:    Revision 1.2  2002/06/04 17:19:49Z  rm70006
 * HISTORY:    Updated test code.
*******************************************************************************/


#ifndef __CDS
#define __CDS

#include "datastore.h"
#include "carray.h"

#include <iostream>
#include <string>

typedef enum
{
   THIS,
   THAT,
   THEOTHER
} EnumType;



typedef struct
{
   int    x;
   float  y;
   double z;
} MessageType;



typedef struct
{
   int count1;
   int count2;
} CountType;



typedef carray<MessageType, 50> MessageArrayType;

typedef carray<char, 80> CharArrayType;



// Needed operators
inline std::ostream & operator <<(std::ostream &strm, const MessageType &n)
{
   strm << "(" << n.x << ", " << n.y << ", " << n.z << ")";
   return strm;
}


inline std::ostream & operator <<(std::ostream &strm, const MessageArrayType &n)
{
   for (int i = 0; i < n.size(); i++)
   {
      strm << "(" << n[i].x << ", " << n[i].y << ", " << n[i].z << ")" << endl;
   }
   return strm;
}


inline std::ostream & operator <<(std::ostream &strm, const CharArrayType &n)
{
   for (int i = 0; i < n.size(); i++)
   {
      strm << n[i];
   }
   return strm;
}


//
// Derived Container.
// Contains collection of elements making up a logical data store.
// User must register his role.  Role of ROLE_RW grants authority to write data.
//
class CDS : public MultWriteDataStore
{
// Data Members
public:
   BaseElement<int>              BE1;       // Test BaseElement int
   BaseElement<float>            BE2;       // Test BaseElement float
   BaseElement<double>           BE3;       // Test BaseElement double
   BaseElement<EnumType>         BE4;       // Test BaseElement enum
   BaseElement<CharArrayType>    BE5;       // Test BaseElement char array
   BaseElement<MessageType>      BE6;       // Test BaseElement struct
   BaseElement<MessageArrayType> BE7;       // Test BaseElement struct array


   RangedElement<int>            RE1;       // Test RangedElement int
   RangedElement<float>          RE2;       // Test RangedElement float
   RangedElement<double>         RE3;       // Test RangedElement double
   RangedElement<EnumType>       RE4;       // Test RangedElement enum

   BaseElement<CountType>        count;

// Class Methods
public:
   ~CDS() { };


   CDS (Role role) :
     MultWriteDataStore ("CDS", role)
   {
      // Base Element Registers
      BE1.Register (this, PFR_RECOVER, 1);
      BE2.Register (this, PFR_RECOVER, 2.0f);
      BE3.Register (this, PFR_RECOVER, 3.0);
      BE4.Register (this, PFR_RECOVER, THIS);
      BE5.Register (this, PFR_RECOVER);
      BE6.Register (this, PFR_RECOVER);
      BE7.Register (this, PFR_RECOVER);


      // Ranged Element Registers
      RE1.Register (this, PFR_RECOVER, 5,     50.0,     8.0);
      RE2.Register (this, PFR_RECOVER, 0.0f, 200.0f,    1.0f);
      RE3.Register (this, NO_PFR,      0.0,    3.14159, 2.5);
      RE4.Register (this, NO_PFR,      THIS,  THEOTHER, THAT);

      count.Register(this, NO_PFR);
   };

// Data Members
private:



// Class Methods
private:
   CDS();  // Base Constructor not available
};

#endif
