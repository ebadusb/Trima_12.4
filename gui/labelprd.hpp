/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1999 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Screen_LABELPRD

*     File name:    labelprd.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This screen will be invoked when Proc determines that the
         platelet product can be labeled as meeting the WBC contamination
         criteria (within limits). It is a simple placard with text directing
         the operator to label the product.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.1x/Trima/gui/rcs/labelprd.hpp 1.9 2003/04/23 16:07:48Z pn02526 Exp dslausb $
$Log: labelprd.hpp $
Revision 1.9  2003/04/23 16:07:48Z  pn02526
Re-enable verify and label screen displays using PQ data from the CDS.
Revision 1.8  2003/01/06 11:31:15  jl11312
- removed Zinc, added Ugl interface layer, modified handling of bitmaps and text
Revision 1.7  2002/11/11 06:34:57  sb07663
replaced char[] constructs with guistring equivalents
Revision 1.6  2002/08/15 22:16:41  sb07663
Compiling version for Vx
Revision 1.5  2002/05/07 13:58:11  sb07663
First cut at VxWorks compatibility
Revision 1.4  2001/05/29 19:31:09  sb07663
Updates for count messages / reasons per V5 specification
Revision 1.3  2001/05/16 22:50:51  pn02526
Revise count/verify flags code for new Product Quality data
Revision 1.2  2000/06/22 15:37:43  ms10234
Move the Run Summary structure to the trima common project
Revision 1.1  1999/06/15 00:15:19  TW09453
Initial revision

*/


// Single include define guardian
#ifndef LABELPRD_HPP
#define LABELPRD_HPP



// CLASS INCLUDE


#include "aphwind.hpp"
#include "btncanc.hpp"
#include "btngobk.hpp"
#include "btncont.hpp"
#include "btnlowl.hpp"
#include "bitmap.h"
#include "display_text.h"


// MESSAGE DECLARES

#include "message.h"
#include "runsummary.h"
#include "pqinformation.h"


// CLASS DECLARES



class Screen_LABELPRD
   : public Base_Apheresis_Window
{

private:

   // The runsumm1 data cartouche bitmap object
   Bitmap _bitmapPltTextBox, _bitmapPlzTextBox;

   // The display text label objects
   Display_Text _textHeading,
                _textLblPltBody,
                _textLblPlzBody ;

   // The Product Quality data access class
   PQInformation _PQA;

public:

   // CONSTRUCTOR
   Screen_LABELPRD();

   // DESTRUCTOR
   virtual ~Screen_LABELPRD();

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

protected:

   Message<RUN_SUMMARY_STRUCT>* _msgPtrReceiveRunStats;
   // PROCESS_LABELPRD_DATA
   // This method would be called by the Msg_Receive_Run_Stats message (upon receipt).
   // This method formats the data and inserts it into the various display
   // text objects that hold and display the data.
   void process_labelprd_data () ;

};  // END of Screen_LABELPRD Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH 7633258662f64fba7ee4198e142bd3a9 */
