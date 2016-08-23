/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2008 CaridianBCT, Inc. All rights reserved

*     Author:       Dan Slauson

*     Class name:   Screen_JP_CLAMP_BLOOD_DIVERSION

*     File name:    jpClampBloodDiversion.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class represents the screen that directs the operator to
         close the blood diversion clamp.  It should only be displayed if
         the features.bin option "jp_sample_bag_configuration" is configured
         on, which will only be for Japan.  This screen is the default for
         the AC connect state, but if we are not configured to display it,
         the system just transitions to the AC Connect screen, and this screen
         displays nothing.


*************************< FOCUSSED GUI DOMAIN >****************************

$Log: jpClampBloodDiversion.hpp $
Revision 1.2  2009/05/12 22:29:14Z  dslausb
Japan-specific screen work
Revision 1.1  2008/11/04 16:20:02Z  dslausb
Initial revision

*/

// Single include define guardian
#ifndef __JP_CLAMP_BLOOD_DIVERSION_HPP__
#define __JP_CLAMP_BLOOD_DIVERSION_HPP__

#include <vxworks.h> // Needed in header for CPU compiler directives.

// CLASS INCLUDE
#include "aphwind.hpp"
#include "bitmap.h"
#include "display_text.h"


// CLASS DECLARES



class Screen_JP_CLAMP_BLOOD_DIVERSION
   : public Base_Apheresis_Window
{

private:
   // The disposable clamp placard bitmap object
   Bitmap _bmpClampBloodDiversion;

   // The display text objects
   Display_Text
      _txtClampBloodDiversion,
      _txtDontConnectAc;

public:

   // CONSTRUCTOR
   Screen_JP_CLAMP_BLOOD_DIVERSION();

   // DESTRUCTOR
   virtual ~Screen_JP_CLAMP_BLOOD_DIVERSION();

   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);

   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

   // PROCESS_CONTINUE_BUTTON
   // This method is virtual and should be overriden by any derived class using
   // the continue button.
   virtual void process_continue_button ();

       #if (CPU==SIMNT)
   void handleTraverseMessage ();
       #endif // #if (CPU==SIMNT)


};  // END of Screen_JP_CLAMP_BLOOD_DIVERSION Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH c866e7943dcc2af00aac8667ba93d82a */
