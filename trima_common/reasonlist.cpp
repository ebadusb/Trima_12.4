#include <vxworks.h>

//
// Static Model

#include "reasonlist.h"
#include "trima_datalog.h"


ReasonList::ReasonList()
   : _reasons()
{}

ReasonList::~ReasonList()
{}

// Returns 0 if _reasons[*] == 0; else the count of reasons.
// This implies that the verify flag for which this reason object exists is set.
int ReasonList::Get () const
{
   int j, count;

   for ( j = 0, count = 0; j < MAXIMUM_REASONS; j++ )
   {
      if ( (_reasons[INDEX_OF_BIT(j)] & BIT_SET(j) ) != (BitArray_t)0 ) count++;
   }
   return ( count );
}

// Returns whether theReason occurs in _reasons. If
// theReason does not occur in the _reasons array returns 0.
// To determine if aReasonEnumeration value occurs, use the following:
//
//     if( Get( aReasonEnumeration ) )
//     {
//        It occurs.
//     }
//     else
//     {
//        It doesn't occur.
//     }
int ReasonList::Get (ENUMERATED_REASON theReason) const
{

   int        j = ((int)theReason - (int)NULL_REASON);
   BitArray_t v = _reasons[INDEX_OF_BIT(j)];
   v &= BIT_SET(j);
   return( v != (BitArray_t)0 );
}

// This operation encapsulates the function of appending a non-duplicating
// reason for the flag with which the instantiation is identified.
void ReasonList::Put (ENUMERATED_REASON theReason)
{

   if ( theReason != NULL_REASON && theReason != LAST_REASON )
   {
      int j = ((int)theReason - (int)NULL_REASON);
      _reasons[INDEX_OF_BIT(j)] |= BIT_SET(j);
   }

}

// This operation encapsulates the function of removing a reason for the flag
// with which the instantiation is identified.
void ReasonList::Remove (ENUMERATED_REASON theReason)
{
   if ( theReason != NULL_REASON && theReason != LAST_REASON )
   {
      int j = ((int)theReason - (int)NULL_REASON);
      _reasons[INDEX_OF_BIT(j)] &= BIT_MASK(j);
   }

}

const char* ReasonList::ToString (ENUMERATED_REASON theReason)
{
   return ENUMERATED_REASON_NAME(theReason);
}


void ReasonList::ClearAll ()
{
   for (int i = 0; i < MAXIMUM_REASONS; i++)
   {
      if ( (_reasons[INDEX_OF_BIT(i)] & BIT_SET(i) ) != (BitArray_t)0 )
      {
         DataLog(log_level_proc_pqi_info) << "Reason " << ToString( (ENUMERATED_REASON)i) << " has been cleared.  No product collected." << endmsg;
         Remove((ENUMERATED_REASON)i);
      }
   }
}


void ReasonList::Initialize ()
{
   int j;
   for (j = 0; j < (int)(sizeof(_reasons) / sizeof(_reasons[0])); j++ )
   {
      _reasons[j] = 0;
   }
}

void ReasonList::Initialize (ENUMERATED_REASON r1,
                             ENUMERATED_REASON r2 /* = NULL_REASON*/,
                             ENUMERATED_REASON r3 /* = NULL_REASON*/,
                             ENUMERATED_REASON r4 /* = NULL_REASON*/,
                             ENUMERATED_REASON r5 /* = NULL_REASON*/,
                             ENUMERATED_REASON r6 /* = NULL_REASON*/,
                             ENUMERATED_REASON r7 /* = NULL_REASON*/,
                             ENUMERATED_REASON r8 /* = NULL_REASON*/,
                             ENUMERATED_REASON r9 /* = NULL_REASON*/,
                             ENUMERATED_REASON r10 /* = NULL_REASON*/,
                             ENUMERATED_REASON r11 /* = NULL_REASON*/,
                             ENUMERATED_REASON r12 /* = NULL_REASON*/,
                             ENUMERATED_REASON r13 /* = NULL_REASON*/,
                             ENUMERATED_REASON r14 /* = NULL_REASON*/,
                             ENUMERATED_REASON r15 /* = NULL_REASON*/,
                             ENUMERATED_REASON r16 /* = NULL_REASON*/,
                             ENUMERATED_REASON r17 /* = NULL_REASON*/,
                             ENUMERATED_REASON r18 /* = NULL_REASON*/,
                             ENUMERATED_REASON r19 /* = NULL_REASON*/,
                             ENUMERATED_REASON r20 /* = NULL_REASON*/)
{

   Initialize();
   initReasons(r1,  r2,  r3,  r4,  r5,  r6,  r7,  r8,  r9, r10,
               r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, LAST_REASON);
}

void ReasonList::initReasons (ENUMERATED_REASON aReason,
                              ... /*Variable*/)
{

   va_list which;

   va_start(which, aReason);
   while (aReason != LAST_REASON )
   {
      if ( aReason != NULL_REASON ) Put(aReason);
      aReason = (ENUMERATED_REASON)va_arg(which, int);
   }
   va_end(which);

}



void ReasonList::Log (const char* text) const
{
   DataLog(log_level_proc_pqi_debug) << text << ": REASONLIST: ";

   for (int i = 0; i < (int)DIMENSION_FOR_BITS(MAXIMUM_REASONS); i++)
   {
      DataLog(log_level_proc_pqi_info) << std::hex << (int)_reasons[i] << std::dec << " ";
   }

   DataLog(log_level_proc_pqi_debug) << endmsg;
}

/* FORMAT HASH d6ddf4b25edd1672c705b8974d4df411 */
