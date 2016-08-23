/*******************************************************************************
 *
 *  ProcedureList:  a data structure for storing Predict's list of procedures.
 *
 *  Procedures are stored in ascending procedure number order (decreasing priority).
 *
 *  Requirements:
 *
 *     Procedures must be inserted in ascending, consecutive order.  (We could
 *     remove this requirement, which is a little brittle, at the expense of a
 *     more complex container; doesn't seem necessary at the moment).
 *
 ******************************************************************************/

#ifndef PROCEDURE_LIST_H
#define PROCEDURE_LIST_H

#include "procdet.h"  // procedureDetail base class
#include <vector>


class ProcedureList
{
private:

   typedef std::vector<procedureDetail*> Container_t;

   static const int MAXLEN;

public: // TYPES

   typedef Container_t::iterator iterator;
   typedef Container_t::const_iterator const_iterator;

public: // METHODS

   // Singleton access

   static ProcedureList& Instance ();


   iterator begin () { return _list.begin(); }
   iterator end  () { return _list.end()  ; }

   void insert (procedureDetail*); // set procedure number? Yeah

   // This is *not* array (0-based) lookup, it's a map lookup by procedure
   // number.  Accessing an invalid entry is a fatal error
   procedureDetail* operator [] (int);

   procedureDetail* get (int);   // as operator[], but will return null pointer

   // safely replace one procedure with another with the same procedure number
   void replace (procedureDetail*);

   void clear (); // calls delete on all pointers, resets to original state

   size_t size () { return _list.size(); }

   //////////////////////////////////////////////////////////////////////
   //
   //  Currently selected procedure access and control

   // set currently selected procedure
   // void setSelected (int procedureNumber);     // preferred
   // void setSelected (procedureDetail* p); // deprecated

   // bool selected();                 // returns true if a procedure has been selected
   // procedureDetail& selectedProc(); // returns slected procedure; FATAL if no procedure has been selected.

   procedureDetail*& Selected () { return _selected; }
   procedureDetail*& Current  () { return _current ; }
   const int&        MaxDesignator () { return _maxDesignator; }

   //////////////////////////////////////////////////////////////////////
   //
   //  Convenience functions

   void setAllUnoptimized ();


private: // METHODS

   ProcedureList();
   ~ProcedureList();


   // For now, noncopyable and nonassignable.  Not for any deep reason, but just because we don't
   // need them and it's easier not to get into pointer-ownership issues.
   ProcedureList(const ProcedureList&);
   ProcedureList& operator = (const ProcedureList&);

private: // DATA
   Container_t _list;


   //  Singletons
   static procedureDetail* _selected ;
   static procedureDetail* _current  ;
   static int              _maxDesignator ;

};


#endif

/* FORMAT HASH 3397fbc443b28987f473a0216ff779a6 */
