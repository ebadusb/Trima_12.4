/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Screen_RUNSUMM2

*     File name:    runsumm2.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This is a screen derived from the base apheresis class. It
         displays product data received from the procedure domain at
         the end of the run.


*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/runsumm2.hpp 1.24 2008/10/03 16:20:13Z dslausb Exp dslausb $
$Log: runsumm2.hpp $
Revision 1.24  2008/10/03 16:20:13Z  dslausb
Move away from dynamic memory where possible
Revision 1.23  2008/08/16 19:12:29Z  dslausb
Mark's Screenshot Code
Revision 1.23  2008/08/15 20:37:31Z  mereusb
IT 8107: Change for screenshot scripts.
Revision 1.22  2008/07/25 13:04:55  dslausb
- Overhaul the way procedure and cassette data are seen to be more robust to PFR
- Change UGL makefile so that it's not built so frequently
Revision 1.21  2008/07/03 15:04:27Z  mereusb
IT 8107 - Improved screenshot coverage.
Revision 1.20  2008/06/12 17:37:16  dslausb
IT 8679 - Dashes for zeroes
Revision 1.19  2008/02/25 20:00:53Z  dslausb
More fixes for string.info files
Revision 1.18  2007/10/24 22:04:42Z  dslausb
IT 8246 - Change run summary 2 for procedures with two RBC products.
Revision 1.17  2007/08/10 15:27:11Z  dslausb
Screen changed for DRBC Automated Split
Revision 1.16  2007/04/17 20:53:36Z  dslausb
IT 7948 - Make sure that all blinkable text on the run summary screen is visible on screen allocation.
Revision 1.15  2007/04/16 18:02:59Z  dslausb
Fixed runsummary blinking text and screen transitions
Revision 1.14  2007/02/15 21:40:35Z  dslausb
GUI review changes - don't show progress bar for undelivered products.
Revision 1.13  2006/12/13 16:01:48Z  dslausb
Added RAS value to 2nd runsum screen
Revision 1.12  2006/09/11 19:24:14Z  dslausb
Revision 1.11  2006/03/03 17:29:46Z  dslausb
First attempt at bringing over MSS stuff
Revision 1.10  2003/10/16 17:02:34Z  jl11312
- changes for displaying run summary during rinse back (IT 5862)
Revision 1.9  2003/06/17 16:57:28Z  rm10919
Japanese configuration changes.
Revision 1.8  2003/01/06 18:31:20Z  jl11312
- removed Zinc, added Ugl interface layer, modified handling of bitmaps and text
Revision 1.7  2002/11/11 07:19:19  sb07663
replaced char[] constructs with guistring equivalents
Revision 1.6  2002/08/15 22:17:20  sb07663
compiling version for Vx
Revision 1.5  2002/05/07 14:00:02  sb07663
First cut at VxWorks compatibility
Revision 1.4  2000/06/22 21:37:45  ms10234
Move the Run Summary structure to the trima common project
Revision 1.3  1996/07/08 16:16:59  tw09453
Update the file header

*/


// Single include define guardian
#ifndef RUNSUMM2_HPP
#define RUNSUMM2_HPP


// CLASS INCLUDE
#include "aphwind.hpp"
#include "btncanc.hpp"
#include "btngobk.hpp"
#include "btncont.hpp"
#include "btnlowl.hpp"
#include "bitmap.h"
#include "display_text.h"
#include "runsummary.h"

#include "trimamessages.h"
#include "blinkCollection.hpp"
#include "textDataField.hpp"

// CLASS DECLARES
class Screen_RUNSUMM2
   : public Base_Apheresis_Window
{
private:

   bool _showCompPlasmaIcon;

   // The runsumm1 data cartouche bitmap object
   Bitmap
      _bmpPanel,
      _bmpPanelDrbcVersion,
      _bmpPanelCompPlsVersion;

   Bitmap _bmpCompPls_Icon;

   //  Different continue button for Japanese configured machines.
   Button_Bitmap* _btnAltReset;

   // DISPLAY TEXT OBJECTS ---------------------
   Display_Text
   // Section Labels
      _txtSectionLblPlt,
      _txtSectionLblPlas,
      _txtSectionLblPlasForComp,
      _txtSectionLblPltForComp,
      _txtSectionLblRbcForComp,
      _txtSectionLblRbc,
      _txtSectionLblRbc2;

   TextDataField

      _dataPltProdVol,
      _dataPltYield,
      _dataPltYieldForComp,
      _dataPltAcVolForComp,
      _dataPltAcVol,
      _dataPltAasVol,
      _dataPltAasVolForComp,
      _dataPltProdVolComp,

      _dataPlasProdVol,
      _dataPlasAcVol,
      _dataPlasProdVolForComp,
      _dataPlasAcVolForComp,
      _dataCompPls,

      _dataRbcProdVol,
      _dataRbcAcVol,
      _dataRbcAasVol,

      _dataRbcProdVolForComp,
      _dataRbcAcVolForComp,
      _dataRbcAasVolForComp,

      _dataRbc2ProdVol,
      _dataRbc2AcVol,
      _dataRbc2AasVol;

   bool                 _COMP_PLS_ERS;

   BlinkCollection      _blinkCollection;

   ConfirmBoxConfirmMsg _msgRebootConfirmed;
   ConfirmBoxCancelMsg  _msgRebootCanceled;

   void createConfirmCallbacks ();
   void destroyConfirmCallbacks ();

   void confirmCB ();
   void cancelCB ();

   void callConfirmBox ();

protected:

public:

   // CONSTRUCTOR
   Screen_RUNSUMM2();

   // DESTRUCTOR
   virtual ~Screen_RUNSUMM2();

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


   // PROCESS_GOBACK_BUTTON
   // This method is virtual and should be overriden by any derived class using
   // the goback button.
   virtual void process_goback_button ();

   ////// MESSAGING DECLARATIONS AND SUPPORTING POINTERS //////

protected:
   void outputData ();

   Message <RUN_SUMMARY_STRUCT> _msgPtrRunStats;
   // PROCESS_RUNSUMM2_DATA
   // This method is called by the runsumm2 data message (upon receipt).
   // This method formats the data and inserts it into the various display
   // text objects that hold and display the data.
   void processRunStats ();

   void chooseScreenLayout (bool splitDrbcLayout);

   void chooseBlinkingText (bool flashPasStats,
                            bool flashRasStats,
                            bool flashPlasmaStats,
                            bool isSplitLayout);

   void setData (const RUN_SUMMARY_STRUCT& runSummaryStats);

    #if CPU==SIMNT
protected:
   virtual void handleTraverseMessage ();
    #endif /* if CPU==SIMNT */
};


// End of single include define guardian
#endif

/* FORMAT HASH fd66d2342c8f0981ba4861e13b89f342 */
