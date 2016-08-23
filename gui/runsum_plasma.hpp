/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2006 GAMBRO BCT, Inc. All rights reserved

*     Author:       Dan Slauson

*     Class name:   Screen_RUNSUM_PLASMA

*     File name:    runsum_plasma.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class will receive a run summary record from Proc.
         From this record, all the information specific to the plasma
         product will be displayed on this screen.  This screen inherits
         the runsum_product class, which contains all the functionality
         for the flags.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.1x/Trima/gui/rcs/runsum_plasma.hpp 1.4 2006/06/08 20:24:44Z dslausb Exp dslausb $
$Log: runsum_plasma.hpp $
Revision 1.4  2006/06/08 20:24:44Z  dslausb
Changed layout according to gui review on 6/5
Revision 1.3  2006/05/08 16:40:57Z  dslausb
7151 - Fixed broken stuff
Revision 1.2  2006/05/05 21:45:46Z  dslausb
IT 7212 - Separated up flag and labeling run summary screens into their respective products
Revision 1.1  2006/05/02 16:23:07Z  dslausb
Initial revision
*/


// Single include define guardian
#ifndef __RUNSUM_PLASMA_HPP__
#define __RUNSUM_PLASMA_HPP__

#include "runsum_product.hpp"

// CLASS DECLARES
class Screen_RUNSUM_PLASMA
   : public runsum_product
{
public:

   // CONSTRUCTOR
   Screen_RUNSUM_PLASMA();


   // DESTRUCTOR
   virtual ~Screen_RUNSUM_PLASMA();

   // PHYSICAL BUTTON HANDLER
   // This is the method that handles the verify buttons being pressed.
   static void physical_button_handler (void*);

   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);

   // PROCESS_CONTINUE_BUTTON
   // This method is virtual and should be overriden by any derived class using
   // the continue button.
   virtual void process_continue_button ();

   // INITIALIZE_STATUS_LINE
   // This is to initialize the status line one time
   virtual void initialize_status_line ();

private:

   // PROCESS_PRODUCT_WARNING_DATA
   // This method would be called by the Msg_Receive_Run_Stats message (upon receipt).
   // This method formats the data and inserts it into the various display
   // text objects that hold and display the data.
   virtual void processProductWarningData ();
}; // End of Screen_RUNSUM_PLASMA declaration

// End of single include define guardian
#endif

/* FORMAT HASH b907f62faf04b0d80c27b21bb7d30bef */
