/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2006 Gambro BCT, Inc. All rights reserved

*     Class name:   Screen_AAS_RUN_SUMMARY

*     File name:    aasRunSummary.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class represents the screen that is raised when the
         automated additive solutions are terminated and offline
         addition is necessary. It comes just before runsumm1.

         Note that this is now the default screen for the POST_RUN
         state since it comes first in sequence.  Because of this,
         and the fact that it's not usually necessary to show this
         screen, allocate_resources just calls for a run summary.
         When this run summary arrives, the screen makes a decision
         about whether it's necessary to show anything here.  If not,
         we move on to run summary 1.

*************************< FOCUSSED GUI DOMAIN >****************************
* HISTORY:
* Revision 1.5  2008/07/03 14:49:10Z  mereusb
* IT 8107 - Improved screenshot coverage.
* Revision 1.4  2008/06/10 14:44:31  mereusb
* IT 8107: Lots of little foundational changes to help ease writing of screenshot scripts.
* Revision 1.3  2008/04/01 14:58:26  dslausb
* IT 8077 - Implement separate end run screens for RAS and PAS
* Revision 1.2  2008/03/26 22:39:58Z  dslausb
* Changed function signature
* Revision 1.1  2008/03/24 16:15:44Z  dslausb
* Initial revision
*----CHANGED NAME FROM mssIncomplete.hpp to aasRunSummary.hpp---dslausb
* Initial revision
*/

// Single include define guardian
#ifndef __AAS_RUN_SUMMARY_HPP__
#define __AAS_RUN_SUMMARY_HPP__


// CLASS INCLUDE
#include <vxworks.h> // Included here so that SIMNT is defined
#include "aphwind.hpp"
#include "bitmap.h"
#include "button_bitmap.h"
#include "discrete_button.h"
#include "display_text.h"
#include "trimamessages.h"
#include "runsummary.h"

/////////////////////////
// class OfflineMeteringItem
// **********************
// A quickie class to organize the labels into their respective
// purposes.
class OfflineMeteringItem
{
private:
   string       _logName;
   Display_Text _txtLbl, _txtData, _txtUnit;
   Bitmap       _dataBox;

   OSRegion     _origLblRegion, _origDataRegion, _origUnitRegion;

   bool         _haveSetOriginalRegions;

public:
   OfflineMeteringItem(const string& logName,
                       TEXT_STRING_ID lblId,
                       TEXT_STRING_ID dataId,
                       TEXT_STRING_ID unitId);

   ~OfflineMeteringItem();
   void allocate_resources (OSWindow& parent, int data);
   void deallocate_resources ();

   void init (); // Reset all the bitmap ids so that y coordinates are where they started.
   void bumpYcoord (int yCoordDelta);
   void changeLblId (TEXT_STRING_ID newLblId) { _txtLbl.set_string_id(newLblId); }

private:
   // Disable the default constructor.  Use the public one.
   OfflineMeteringItem();
};
// end of class OfflineMeteringItem
/////////////////////////


/////////////////////////
// class Screen_AAS_RUN_SUMMARY
// **********************
// The actual meat of the screen
class Screen_AAS_RUN_SUMMARY
   : public Base_Apheresis_Window
{
private:
   OfflineMeteringItem
            _pasMeteringItem,
            _rasMeteringItem1,
            _rasMeteringItem2;

   // The display text object
   Display_Text
      _txtMainInstructions,
      _txtDataLbl;

   Bitmap
      _bmpTextAreaTop,
      _bmpTextAreaBottom,
      _bmpInstructionalGraphic;

   Message<RUN_SUMMARY_STRUCT> _msgRunStats;
   void processRunsummData ();
   int  _passes;
   int  _pass;
   int  _rasSalvageVol1, _rasSalvageVol2, _pasSalvageVol;
   bool _haveBeenRas;

public:
   // CONSTRUCTOR
   Screen_AAS_RUN_SUMMARY();


   // DESTRUCTOR
   virtual ~Screen_AAS_RUN_SUMMARY();


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

private:
   void setStatLineAndBar ();
   void selectGraphicAndInstruction (bool setIsPtf, bool setIsDrbc);
   void selectRASGraphicAndInstruction (bool setIsPtf, bool setIsDrbc);
   void showOfflineAdditionData (int pasSalvageVol, int rasSalvageVolProd1, int rasSalvageVolProd2, bool targetedAsAutoSplit);

#if (CPU==SIMNT)
protected:
   virtual void handleTraverseMessage ();
#endif // #if (CPU==SIMNT)

};


#endif

/* FORMAT HASH 80206508342c2c2521ee75c6f36b9952 */
