/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      mssConnect.h
 *
 *    This state will send gui a message upon entrance, indicating setup completed.
 *    This state will wait for GUI mesg indication that the SS is connected
 *    inorder to transition (via the state table)
 *
 */

/*
$Header: //BCTquad3/home/BCT_Development/Trima5.1x/Trima/proc/proc/rcs/mssConnect.h 1.3 2006/03/15 17:02:01Z jheiusb Exp jheiusb $
$Log: mssConnect.h $
Revision 1.3  2006/03/15 17:02:01Z  jheiusb
add recalc of PLT ss
Revision 1.2  2006/03/03 17:31:51Z  jheiusb
initial merge
Revision 1.1  2006/03/03 17:16:37Z  jheiusb
Initial revision
Revision 1.3  2005/10/07 22:06:27Z  jheiusb
7202 - ADD END RUN LOGIC AND SOME CLEANUP
Revision 1.2  2005/10/05 20:35:13Z  jheiusb
7202 -- initial flow of MSS proc-GUI model stubs

*/
  #ifndef MSS_CONNECT_H  //  Prevents multiple inclusions.
#define MSS_CONNECT_H

#include <stateabs.h>


// Class Declaration
class MSSConnect
   : public StateAbs
{
   DECLARE_STATE(MSSConnect);
public:

   // Default constructor
   MSSConnect();

   // Copy constructor
   MSSConnect(const MSSConnect& state);

   // Default destructor
   virtual ~MSSConnect();

   virtual StateAbs* clone () const { return ( new MSSConnect(*this)); }

protected:

   //
   // Functions to do this states processing ...
   //
   virtual int preProcess ();
   virtual int postProcess ();

   //
   // Function called upon first entrance to this state
   //
   virtual int preEnter ();


   virtual int transitionStatus ();

   //
   // Function called upon transition from this state
   //
   virtual int postExit ();

   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const MSSConnect& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:



};


#endif

/* FORMAT HASH 6fa799b99262133e2bcd8656f6685ea2 */
