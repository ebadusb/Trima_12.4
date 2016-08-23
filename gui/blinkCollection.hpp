/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2007 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  BlinkCollection

*     File name:   blinkCollection.hpp

*     Contents:    Declarations of class methods

*     Description: This class defines a set of BlinkableObject pointers that
        are to be blinked at a set interval.  These can be any object derived
        from BlinkableObject (including Display_Text and TextDataField).

        Up to ten items can be added to the collection at construction, or they
        can be added at a later time, as well.  Upon calling startBlink(),
        all items in the collection are continuously blinked at the passed
        interval (default is 1000).

        Upon calling stopBlink, the collection is emptied and the timer is
        stopped.  This should be called in the screen's deallocate resources
        function.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/blinkCollection.hpp 1.2 2008/06/12 23:37:02Z dslausb Exp $
$Log: blinkCollection.hpp $
Revision 1.2  2008/06/12 23:37:02Z  dslausb
IT 8679 - Dashes for zeroes
Revision 1.1  2007/04/16 18:13:43Z  dslausb
Initial Revision

*/



#ifndef __BLINK_COLLECTION_HPP__
#define __BLINK_COLLECTION_HPP__

#include <vector>
#include "osgraphics.h"
#include "timermessage.h"

class BlinkableObject;

class BlinkCollection
{
private:
   bool _visible;

   // VECTOR TYPES
   typedef vector<BlinkableObject*> WINDOW_OBJECT_COLLECTION;
   typedef WINDOW_OBJECT_COLLECTION::iterator WINDOW_OBJECT_ITERATOR;

   WINDOW_OBJECT_COLLECTION _blinkObjects;
   TimerMessage             _blinkTimer ;

public:

   BlinkCollection(  // Allow ten pointers to be passed right off the bat.
      BlinkableObject* obj1 = NULL,
      BlinkableObject* obj2 = NULL,
      BlinkableObject* obj3 = NULL,
      BlinkableObject* obj4 = NULL,
      BlinkableObject* obj5 = NULL,
      BlinkableObject* obj6 = NULL,
      BlinkableObject* obj7 = NULL,
      BlinkableObject* obj8 = NULL,
      BlinkableObject* obj9 = NULL,
      BlinkableObject* obj10 = NULL
      );

   void addObject (BlinkableObject* obj);
   void startBlink (unsigned long blinkInterval = 1000);
   void stopBlink ();

   void blink ();


};


#endif // #ifndef __BLINK_COLLECTION_HPP__

/* FORMAT HASH c28b42bbe3d43d82169e58013ce47053 */
