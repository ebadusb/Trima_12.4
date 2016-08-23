/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Screen_SYSTEST

*     File name:    systest.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class represents the animated screen that appears while
         the blood tubing set is being tested. This class is a derived
         class from the base apheresis class.

*************************< FOCUSSED GUI DOMAIN >****************************

$Log: systest.hpp $
Revision 1.11  2008/10/03 16:20:55Z  dslausb
Move away from dynamic memory where possible
Revision 1.10  2007/02/22 20:36:05Z  dslausb
GUI changes for new disposable test.
Revision 1.9  2003/01/06 18:32:33Z  jl11312
- removed Zinc, added Ugl interface layer, modified handling of bitmaps and text
Revision 1.8  2002/11/11 07:28:47  sb07663
replaced char[] constructs with guistring equivalents
Revision 1.7  2002/08/15 22:17:47  sb07663
compiling version for vx
Revision 1.6  2002/05/07 14:01:19  sb07663
Revision 1.5  2002/02/19 20:23:20  rm70006
IT 5596.
Add special handlers for language refresh.
Revision 1.4  1996/07/08 20:13:36Z  tw09453
Update file header

*/



// Single include define guardian
#ifndef SYSTEST_HPP
#define SYSTEST_HPP


// CLASS INCLUDE
#include "aphwind.hpp"
#include "bitmap.h"
#include "display_text.h"

// MESSAGE DECLARES
#include "trimamessages.h"
#include "timermessage.h"

// LOCAL DEFINES
// Bitmap display states
enum DISP_TEST_BITMAP_STATE
{
   BITMAP_STATE_A,
   BITMAP_STATE_B
};


// CLASS DECLARES

class Screen_SYSTEST
   : public Base_Apheresis_Window
{

private:

   // The two alternating versions of the system test graphic
   Bitmap
            _bmpDispTstA,
            _bmpDispTstB ;

   // The display text objects
   Display_Text
      _txtDispTst,
      _txtDontConnectAc;

   // The bitmap state - Either A or B
   DISP_TEST_BITMAP_STATE _bitmapState ;


   // Screen Event Handler pointers
   TestCompletedMsg       _msgTestCompleted ;

   CloseCrossoverClampMsg _msgShowClampCrossover;

   bool                   _a2donorMitigation;

   ShowClampFromARMMsg    _msgShowClampFromARM;

   void testCompleted ();

   void showClampCrossover ();

   void showClampDisplay ();
   void showClampPLTbag ();
   void showClampRBCbag ();
   void showClampPlasmaAirBag ();

   // Screen Timer Handler pointers
   TimerMessage _animationTimer ;
   // DISPLAY_ALTERNATE_SYSTEST_GRAPHICS
   // This method displays the first of two graphics showing the pressurizing
   // and depressurizing of the disposable. It is called as a result of a Router
   // timer notification.
   void animateGraphics ();

   TimerMessage _halfSecondDelay ;
   void delayCallback ();


protected:



public:


   // CONSTRUCTOR
   Screen_SYSTEST();


   // DESTRUCTOR
   virtual ~Screen_SYSTEST();


   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

   // INITIALIZE_STATUS_LINE
   // This is to initialize the status line one time
   virtual void initialize_status_line ();

};  // END of Screen_SYSTEST Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH 741112b7171604e212c4b22512cef800 */
