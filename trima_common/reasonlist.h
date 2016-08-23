// Static Model

//

#ifndef __VerifyFlags__ReasonList__
#define __VerifyFlags__ReasonList__


// Include files
#define PQMONITOR_BASE
#include "verifyflags.h"
#undef PQMONITOR_BASE

#include <stdarg.h>
#include <limits.h>

typedef unsigned char BitArray_t;  // Change this typedef to change the container for bit arrays.

/* NOTE: These macros assume CHAR_BIT bits per storage unit, in which sizeof is measured. */
#define BITS_PER_ELEM         int(sizeof(BitArray_t) * CHAR_BIT)
#define DIMENSION_FOR_BITS(s) (((s) / (BITS_PER_ELEM)) + ((s) % BITS_PER_ELEM ? 1 : 0))
#define INDEX_OF_BIT(n)       ((n) / (BITS_PER_ELEM))
#define BIT_NUMBER(n)         (BITS_PER_ELEM - 1 - ((n) % BITS_PER_ELEM))
#define BIT_SET(n)            ((BitArray_t)1 << (BITS_PER_ELEM - 1 - ((n) % BITS_PER_ELEM)))
#define BIT_MASK(n)           ((BitArray_t) ~1 << (BITS_PER_ELEM - 1 - ((n) % BITS_PER_ELEM)))

#define MAXIMUM_REASONS       (LAST_REASON - NULL_REASON)

// This class encapsulates the functionality needed to accumulate verify flag
// and reason settings for a particular run or prediction.
class ReasonList
{
public:
   ReasonList();
   ~ReasonList();

   void Log (const char* text) const;

   int Get () const;

   int Get (ENUMERATED_REASON theReason) const;

   bool isReason (ENUMERATED_REASON theReason) const {return (Get(theReason) != 0); }

   void Put (ENUMERATED_REASON theReason);

   void Remove (ENUMERATED_REASON theReason);

   void ClearAll ();

   const char* ToString (ENUMERATED_REASON theReason);

   void Initialize ();

   void Initialize (ENUMERATED_REASON r1,
                    ENUMERATED_REASON r2 = NULL_REASON,
                    ENUMERATED_REASON r3 = NULL_REASON,
                    ENUMERATED_REASON r4 = NULL_REASON,
                    ENUMERATED_REASON r5 = NULL_REASON,
                    ENUMERATED_REASON r6 = NULL_REASON,
                    ENUMERATED_REASON r7 = NULL_REASON,
                    ENUMERATED_REASON r8 = NULL_REASON,
                    ENUMERATED_REASON r9 = NULL_REASON,
                    ENUMERATED_REASON r10 = NULL_REASON,
                    ENUMERATED_REASON r11 = NULL_REASON,
                    ENUMERATED_REASON r12 = NULL_REASON,
                    ENUMERATED_REASON r13 = NULL_REASON,
                    ENUMERATED_REASON r14 = NULL_REASON,
                    ENUMERATED_REASON r15 = NULL_REASON,
                    ENUMERATED_REASON r16 = NULL_REASON,
                    ENUMERATED_REASON r17 = NULL_REASON,
                    ENUMERATED_REASON r18 = NULL_REASON,
                    ENUMERATED_REASON r19 = NULL_REASON,
                    ENUMERATED_REASON r20 = NULL_REASON);
protected:
private:

   BitArray_t _reasons[ DIMENSION_FOR_BITS(MAXIMUM_REASONS) ];

   void initReasons (ENUMERATED_REASON aReason,
                     ... /*Variable*/);

};

// END CLASS DEFINITION ReasonList

#endif // __VerifyFlags__ReasonList__

/* FORMAT HASH 121343619b5bf74170232850963ff9ed */
