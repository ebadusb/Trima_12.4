/****************************************************************************
 *
 * TITLE:      scoped_cds_lock.hpp
 *
 *             Provides a scoped lock for CDS objects, providing the usual
 *             benefits of RAII design.
 *
 * USAGE:
 *
 * A ScopedCDSLock is safer than calling Lock() and Unlock() on a CDS object,
 * because you can't forget to write the Unlock(), and if there's a code path
 * out of the code block that you didn't notice, the ScopedCDSLock's dtor will
 * release the mutex.  Example:
 *
 * ScopedCDSLock lock( my_CDS_object );
 *
 * The CDS object is locked by this ctor, and is unlocked in the dtor.
 *
 * If you wish to explicitly release the mutex at a given point (rather than
 * retain the mutex until the end of the enclosing code block), call the
 * release() member function (so-named to match the common convention for smart
 * pointers).
 *
 * ScopedCDSLocks are noncopyable, non-assignable, and have no default ctor.
 * Any use in which these might be desirable is almost certainly a bad idea.
 *
 *
 * AUTHOR: Dave Steffen
 *
 ***************************************************************************/



#ifndef SCOPED_CDS_LOCK
#define SCOPED_CDS_LOCK


#include "datastore.h"

class ScopedCDSLock
{
public:

   ScopedCDSLock(DataStore& c)
      : _cds(c), _locked(true) { _cds.Lock(); }

   ~ScopedCDSLock() { if (_locked) _cds.Unlock(); }

   void release () { _cds.Unlock(); _locked = false; }

private:

   ScopedCDSLock();                                  // unimplemented , no default ctor
   ScopedCDSLock(const ScopedCDSLock&);              // unimplented   , noncopyable
   ScopedCDSLock& operator = (const ScopedCDSLock&); // unimplemented , nonassignable

private:

   DataStore& _cds;
   bool       _locked;
};

#endif // scope guard

/* FORMAT HASH 08638341d153ef55b2ab76099e7a9c55 */
