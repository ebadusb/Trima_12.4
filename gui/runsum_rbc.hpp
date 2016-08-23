/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2006 Gambro BCT, Inc. All rights reserved

*     Author:       Dan Slauson

*     Class name:   Screen_RUNSUM_RBC

*     File name:    runsum_rbc.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class will receive a run summary record from Proc.
         From this record, all the information specific to the RBC
         product will be displayed on this screen.  This screen inherits
         the runsum_product class, which contains all the functionality
         for the flags.


*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/runsum_rbc.hpp 1.9 2008/07/10 20:30:33Z dslausb Exp $
$Log: runsum_rbc.hpp $
Revision 1.9  2008/07/10 20:30:33Z  dslausb
Hopefully last bit of stringId standardization stuff
Revision 1.8  2008/07/09 23:53:05Z  dslausb
More string ID standardization as requested by Marita
Revision 1.7  2008/07/03 20:18:21Z  dslausb
Further standardization of string IDs for localization
Revision 1.6  2007/09/11 21:35:28Z  dslausb
Update to run summary and flagging functionality
Revision 1.5  2007/08/10 15:26:55Z  dslausb
Screen changed for DRBC Automated Split
Revision 1.4  2006/09/11 19:23:49Z  dslausb
Revision 1.3  2006/05/08 16:41:03Z  dslausb
7151 - Fixed broken stuff
Revision 1.2  2006/05/05 21:46:04Z  dslausb
IT 7212 - Separated up flag and labeling run summary screens into their respective products
Revision 1.1  2006/05/02 18:03:43Z  dslausb
Initial revision
*/

#ifndef __RUNSUM_RBC_HPP__
#define __RUNSUM_RBC_HPP__

#include "runsum_product.hpp"

class Screen_RUNSUM_RBC
   : public runsum_product
{
public:
   // CONSTRUCTOR
   Screen_RUNSUM_RBC(GUI_SCREEN_ID screenId = GUI_SCREEN_RUNSUM_RBC);

   // DESTRUCTOR
   virtual ~Screen_RUNSUM_RBC();

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

   // PROCESS_PRODUCT_WARNING_DATA
   // This method would be called by the Msg_Receive_Run_Stats message (upon receipt).
   // This method formats the data and inserts it into the various display
   // text objects that hold and display the data.
   virtual void processProductWarningData ();

}; // End of Screen_RUNSUM_RBC declaration

// End of single include define guardian
#endif

/* FORMAT HASH 9d478ef0938e5917a453202237582b02 */
