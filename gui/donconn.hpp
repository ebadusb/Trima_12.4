/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Screen_DONCONN

*     File name:    donconn.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class is a specific apheresis screen derived from the base class
         Base_Apheresis_Window.  This screen requests that the user connect
         the donor to the disposable set, and waits until the continue button
         is pressed.

*************************< FOCUSSED GUI DOMAIN >****************************
*/

// Single include define guardian
#ifndef DONCONN_HPP
#define DONCONN_HPP

#include <vxworks.h> // Needed for SIMNT compiler directives

// CLASS INCLUDE
#include "aphwind.hpp"
#include "bitmap.h"
#include "display_text.h"
#include "button_bitmap.h"
#include "discmetr.hpp"
#include "procedure_status.h"

// CLASS DECLARES

class Screen_DONCONN
   : public Base_Apheresis_Window
{

private:

   // The donor connection placard bitmap object
   Bitmap
            _bmpDiversionJp,
            _bmpDiversion,
            _bmpNoDiversionJp,
            _bmpNoDiversion;

   // The display text objects
   Display_Text
      _txtPanelOneNoDiversion,
      _txtPanelOneDiversion,
      _txtPanelTwo,
      _txtPanelThreeNoDiversion,
      _txtPanelThreeDiversion,
      _txtPanelThreeNoDiversionJp,
      _txtPanelThreeDiversionJp;

   Button_Bitmap _btnStartDraw;

   // Discrete pressure meter used for "stick" feedback
   Discrete_Meter _stickMeter ;

   const ProcedureStatus::ProcSnapshot& _currentProcData;

   virtual void chooseDynamicTextAndGraphics (bool bloodDiversionOn, bool dontShowSealSampleBag);

   virtual void deallocateDynamicTextAndGraphics ();

public:


   // CONSTRUCTOR
   Screen_DONCONN();


   // DESTRUCTOR
   virtual ~Screen_DONCONN();


   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();


   // PROCESS_CANCEL_BUTTON
   // This method is virtual and should be overriden by any derived class using
   // the cancel button.
   virtual void process_cancel_button ();


   // PROCESS_START_DRAW_BUTTON
   // This method is virtual and should be overriden by any derived class using
   // the continue button.
   virtual void processStartDrawBtn ();


   // MANAGE_STICK_PRESSURE_DATA
   // This routine receives the proc status information including the access pressure
   // it will cause the stick pressure meter to display this information.
   virtual void manage_stick_pressure_data () ;

   // INITIALIZE_STATUS_LINE
   // This is to initialize the status line one time
   virtual void initialize_status_line ();

#if (CPU==SIMNT)
protected:
   virtual void handleTraverseMessage ();
#endif // #if (CPU==SIMNT)

}; // END of Screen_DONCONN Class Declaration


// End of single include define guardian
#endif /* ifndef DONCONN_HPP */

/* FORMAT HASH 496a18b75c02680f9239d830be432ba6 */
