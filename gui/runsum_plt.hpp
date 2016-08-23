/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2006 Gambro BCT, Inc. All rights reserved

*     Author:       Dan Slauson

*     Class name:   Screen_RUNSUM_PLT

*     File name:    runsum_plt.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class will receive a run summary record from Proc.
         From this record, all the information specific to the platelet
         product will be displayed on this screen.  This screen inherits
         the runsum_product class, which contains all the functionality
         for the flags.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.1x/Trima/gui/rcs/runsum_plt.hpp 1.4 2006/06/08 20:24:54Z dslausb Exp dslausb $
$Log: runsum_plt.hpp $
Revision 1.4  2006/06/08 20:24:54Z  dslausb
Changed layout according to gui review on 6/5
Revision 1.3  2006/05/08 16:41:00Z  dslausb
7151 - Fixed broken stuff
Revision 1.2  2006/05/05 21:45:52Z  dslausb
IT 7212 - Separated up flag and labeling run summary screens into their respective products
Revision 1.1  2006/05/02 16:58:49Z  dslausb
Initial revision
*/

#ifndef __RUNSUM_PLT_HPP__
#define __RUNSUM_PLT_HPP__

#include "runsum_product.hpp"

class Screen_RUNSUM_PLT
   : public runsum_product
{
public:
   // CONSTRUCTOR
   Screen_RUNSUM_PLT();

   // DESTRUCTOR
   virtual ~Screen_RUNSUM_PLT();

   // PHYSICAL BUTTON HANLER
   // Deal with user pressing a verify button
   //
   static void physical_button_handler (void*);

   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);

   // PROCESS_CONTINUE_BUTTON
   // This method is virtual and should be overriden by any derived class using
   // the continue button.
   virtual void process_continue_button ();

private:

   // PROCESS_PRODUCT_WARNING_DATA
   // This method would be called by the Msg_Receive_Run_Stats message (upon receipt).
   // This method formats the data and inserts it into the various display
   // text objects that hold and display the data.
   virtual void processProductWarningData ();
}; // End of Screen_RUNSUM_PLT declaration

// End of single include define guardian
#endif

/* FORMAT HASH a3b72bcec1663325f2ae5c0cc7b0e236 */
