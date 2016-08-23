/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2007 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  BlinkCollection

*     File name:   blinkCollection.cpp

*     Contents:    Declarations of class methods

*     Description: See corresponding header file blinkCollection.hpp

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/blinkCollection.cpp 1.3 2008/06/12 23:36:59Z dslausb Exp $
$Log: blinkCollection.cpp $
Revision 1.3  2008/06/12 23:36:59Z  dslausb
IT 8679 - Dashes for zeroes
Revision 1.2  2007/04/17 20:53:29Z  dslausb
IT 7948 - Make sure that all blinkable text on the run summary screen is visible on screen allocation.
Revision 1.1  2007/04/16 18:13:40Z  dslausb
Initial Revision

*/


#include "blinkCollection.hpp"
#include "display_text.h" // Needed for BlinkableObject class.


BlinkCollection::BlinkCollection(BlinkableObject* obj1 = NULL,
                                 BlinkableObject* obj2 = NULL,
                                 BlinkableObject* obj3 = NULL,
                                 BlinkableObject* obj4 = NULL,
                                 BlinkableObject* obj5 = NULL,
                                 BlinkableObject* obj6 = NULL,
                                 BlinkableObject* obj7 = NULL,
                                 BlinkableObject* obj8 = NULL,
                                 BlinkableObject* obj9 = NULL,
                                 BlinkableObject* obj10 = NULL
                                 )
   : _visible(true),
     _blinkTimer(0, Callback<BlinkCollection>(this, &BlinkCollection::blink), TimerMessage::DISARMED)
{
   // Add the objects
   if (obj1) _blinkObjects.push_back(obj1);
   if (obj2) _blinkObjects.push_back(obj2);
   if (obj3) _blinkObjects.push_back(obj3);
   if (obj4) _blinkObjects.push_back(obj4);
   if (obj5) _blinkObjects.push_back(obj5);
   if (obj6) _blinkObjects.push_back(obj6);
   if (obj7) _blinkObjects.push_back(obj7);
   if (obj8) _blinkObjects.push_back(obj8);
   if (obj9) _blinkObjects.push_back(obj9);
   if (obj10) _blinkObjects.push_back(obj10);
}

void BlinkCollection::addObject (BlinkableObject* obj)
{
   if (obj) _blinkObjects.push_back(obj);
}

void BlinkCollection::startBlink (unsigned long blinkInterval = 1000)
{
   // Make all objects visible before deleting collection.
   for (WINDOW_OBJECT_ITERATOR it = _blinkObjects.begin(); it != _blinkObjects.end(); it++)
      (*it)->blinkInit();

   _blinkTimer.interval(blinkInterval);
   _blinkTimer.armTimer(TimerMessage::ARMED);
}

void BlinkCollection::stopBlink ()
{
   // Delete the blink timer
   _blinkTimer.armTimer(TimerMessage::DISARMED);

   // Make all objects visible before deleting collection.
   for (WINDOW_OBJECT_ITERATOR it = _blinkObjects.begin(); it != _blinkObjects.end(); it++)
      (*it)->reset();

   // Clear out the collection
   _blinkObjects.clear();

   // Reset the visible trait
   _visible = true;
}

void BlinkCollection::blink ()
{
   // Switch visibility trait
   _visible = !_visible;

   // Traverse through objects and set visibity
   for (WINDOW_OBJECT_ITERATOR it = _blinkObjects.begin(); it != _blinkObjects.end(); it++)
      (*it)->blink(_visible);
}

/* FORMAT HASH ef5d011cf7c6fc6a550c02aecfe0f446 */
