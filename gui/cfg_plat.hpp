/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  Screen_CONFIG_PLATELET

*     File name:   cfg_plat.hpp

*     Contents:    See prototype header descriptions below...

*     Description:


*************************< FOCUSSED GUI DOMAIN >****************************


Revision 1.32  2008/07/09 16:29:25Z  dslausb
IT 8735 - Fix a procedure bug
Revision 1.31  2008/07/08 18:51:26Z  dslausb
IT 8735 - Add four more plt procedures
Revision 1.30  2008/06/25 15:56:21Z  dslausb
Tweak plt cfg screen to gain localization room
Revision 1.29  2008/04/08 17:03:07Z  dslausb
IT 8451 - Implement Time-Only Config Mode
Revision 1.28  2008/02/19 23:41:32Z  dslausb
First batch of layout fixes for string.info files
Revision 1.27  2007/09/17 21:41:58Z  dslausb
IT 7992 - Add Male-Only PLT and Male-Only Plasma config options for TRALI.
Revision 1.26  2007/03/16 22:20:55Z  dslausb
IT 7642 - Rearranged PLT config screen buttons and text fields, and made concentration change less frequently.
Revision 1.25  2006/08/02 16:33:57Z  dslausb
IT 7675 - Show magnifying glass icon on PLT config screen if conc > cobeConfig threshold (currently 4200)
Revision 1.24  2006/06/07 23:50:27Z  dslausb
- Added soft lower limit for percent carryover value
- Added additional error messages for concentration and bag volume out of range
Revision 1.23  2006/06/07 21:12:07Z  dslausb
Updated PLT Config Screen to show both collect concentration and final concentration.
Revision 1.22  2006/06/06 18:23:58Z  jheiusb
7559 -- cleanup header problem
Revision 1.21  2006/06/05 22:48:25Z  dslausb
Removed Fixed Volume
Revision 1.20  2006/05/26 16:40:29Z  dslausb
Formatting on config_plt screen
Revision 1.19  2006/04/27 15:52:56Z  dslausb
IT 7468 - Percent carryover is now configurable for each procedure if turned on.  Toggle is now on PLT screen.  Also, text changes.
Revision 1.18  2006/04/11 15:59:07Z  dslausb
IT 7474 - Changed formula for calculating lower limit for concentration.
Revision 1.17  2006/03/24 21:04:28Z  dslausb
Updated alarms and added MSS status box on Platelet Config screen.
Revision 1.16  2006/03/16 18:33:21Z  dslausb
Changed plt mss vol type to use enum values instead of 0 or 1 for readablity
Revision 1.15  2006/03/03 17:29:01Z  dslausb
First attempt at bringing over MSS stuff
Revision 1.24  2005/12/20 23:12:23Z  dslausb
IT 7306 - Fixed wiggy platelet config screen
Revision 1.23  2005/12/13 22:00:20Z  dslausb
IT 7306 - Fixed possible negative values in PLT config because of changing SS vol.
Revision 1.22  2005/11/14 18:17:52Z  dslausb
IT 7202 - Metered Storage - Added label to RBC and Plat config screens to display status of MSS (On/Off, plus Volume/%Carryover for plat).
Revision 1.21  2005/11/11 20:56:20Z  dslausb
IT 7202 - Metered Storage - Changed platelet config screen
- % Carryover is calculated and displayed when on fixed volume.
- % unit is in label rather than field.
Revision 1.20  2005/11/08 17:38:23Z  dslausb
IT 7202 - Metered Storage - On plt config screen, changed layout and functionality of percent carryover field.
Revision 1.19  2005/10/31 19:37:22Z  dslausb
IT 7202 - Metered Storage - Added functionality for % plas carryover
Revision 1.18  2005/10/27 23:00:07Z  dslausb
IT 7202 - Metered Storage - Added code to check concentration levels when SS is toggled, and to make sure volume doesn't drop below 50 when concentration is changed with MSS turned on.
Revision 1.17  2005/10/27 19:05:04Z  dslausb
IT 7202 - Metered Storage - Added labels and functionality for % plasma carryover
Revision 1.16  2005/10/26 19:58:49Z  dslausb
IT 7202 - Metered Storage - Updated RBC and Platelet config screens with stuff for metered storage and press through filter.
Revision 1.15  2005/09/30 15:10:22Z  jheiusb
mss config.dat additions
Revision 1.14  2003/12/18 22:17:35Z  rm10919
Fix text string placement and delete non-used strings. IT5911
Revision 1.13  2003/02/26 17:13:42Z  rm10919
IT 5889 - Made changes for YSF button value and entering YSF value from keypad.
Revision 1.12  2002/11/11 05:21:17Z  sb07663
replaced char[] constructs with guistring equivalents
Revision 1.11  2002/07/19 17:15:29  rm70006
VXWorks Port
Revision 1.10  2000/08/14 18:34:12Z  sb07663
IT4564: consolidation of config.dat parameter tables
Revision 1.9  2000/02/28 21:54:35  SB07663
IT4612: initial checkin for single stage user configuration
Revision 1.8  1999/04/22 22:34:37  SB07663
V3.3a fixes: added multiply/equal signs, concentration limit fixes
Revision 1.7  1999/04/16 14:59:29  SB07663
V3.3a versions for new screen definitions
Revision 1.16  1999/04/07 22:13:23  SB07663
Initial version (incomplete) for V3.3a
Revision 1.15  1998/10/14 14:51:05  BF02858
- IT 3605.  Added 'height' amount to widget call.  To keep the labels
 from 'moving' downward when the down button was pressed.
Revision 1.14  1997/06/02 20:12:54  Barry_Fulkerson
- changed the allocation of the up/down buttons so that the up button
  is grayed out when at the top of the list and the down button is grayed
  out when at the bottom of the list
Revision 1.13  1997/03/15 00:02:21  Barry_Fulkerson
- reduced label size so text box doesn't run off screen
Revision 1.12  1997/03/13 22:35:12  Barry_Fulkerson
- changed reference from min/max string to min/max text id's for easier translations
Revision 1.11  1997/03/13 19:11:49  Barry_Fulkerson
- changed width for new procedure #
Revision 1.10  1997/03/13 03:50:42  Barry_Fulkerson
- display the procedure for the button pressed, per IT 1412
Revision 1.9  1997/03/10 16:02:39  Barry_Fulkerson
- moved scroll buttons to align with PROCEDURE labels, per Julie C's
  recommendation.
Revision 1.8  1997/03/07 17:57:10  Barry_Fulkerson
Revision 1.7  1997/03/06 04:50:52  Barry_Fulkerson
- change #defines so not to conflict with changes in cfg_look.h
Revision 1.6  1997/03/05 19:28:29  Barry_Fulkerson
- added methods for displaying textual limit info when time button is pressed
Revision 1.5  1997/03/05 01:59:33  Barry_Fulkerson
- added keypad limit labeling to the Priority screen
Revision 1.4  1997/03/04 20:50:00  Barry_Fulkerson
- added new code to extend data from 5 to 15 procedures in the Procedure Priority screen
Revision 1.4  1997/02/28 02:44:26  Barry_Fulkerson
- TEMP checkin of PRIORITY screen, so that procedure selection is available
   only 3 procedures are displayed because the up/down scroll is not yet implemented
  THIS IS NOT FINAL!!!!
Revision 1.2  1997/02/26 03:17:54  Barry_Fulkerson
- add Header & Log key words for revision control
*/

// Single include define guardian
#ifndef SYSCONFIG_PLATELET_HPP
#define SYSCONFIG_PLATELET_HPP


// CLASS INCLUDE


#include "screen_config.hpp"
#include "cfg_butt.hpp"
#include "bitmapTextBox.h"


static const int MAX_ROWS = 3;

//
///////////////////////////////////////////////////////////////////
//

class Screen_CONFIG_PLATELET
   : public Screen_Config
{

public:

   enum BUTTON_TYPE
   {
      INACTIVE,
      YIELD,
      VOLUME,
      FINAL_CONC,
      YSF,
      PLASMA_PERCENT,
   };

   struct BTN_INFO
   {
      int                     xPos;
      int                     yPos;
      void*                   btnPtr;
      Button_Bitmap*          btnBmpPtr;
      Screen_CONFIG_PLATELET* parentScreenPtr;
      BUTTON_TYPE             btnType;
      int                     rowNum;
   };

   static bool _btnLock;

protected:


   int         _topProduct, _callbackProduct;
   BUTTON_TYPE _callbackBtnType;

   // The display text objects
   Display_Text
      _txtYieldLbl,
      _txtVolLbl,
      _txtFinalConcLbl,
      _txtCollectConcLbl,
      _txtPasVolLbl,
      _txtPlasmaPercentLbl,
      _toThe3rdCollectConc,
      _toThe3rdFinalConc,
      _toThe11thYield,
      _txtYsfLbl,
      _txtDonorsAllowedLbl,
   * _productIndicatorPtr;

   Button_Bitmap
   * _btnPtrUp,
   * _btnPtrUpGray,
   * _btnPtrDown,
   * _btnPtrDownGray;

   ConfigButton
      _btnYsf,
      _btnDonorsAllowed;

   // This is the set of
   struct PRODUCTS
   {
      // The two string IDs are probably the same text with different styling records.
      TEXT_STRING_ID   productIndicatorString;
      TEXT_STRING_ID   productLblString;
      CONFIG_DATA_KEYS keyYield;
      CONFIG_DATA_KEYS keyProductVol;
      CONFIG_DATA_KEYS keyPasToggle;
      CONFIG_DATA_KEYS keyPasPlasmaPercent;
      float            finalConc;
   } _products[NUM_YIELD_VOLUME];

   struct GRAPHICAL_ROW
   {
      // The buttons
      BTN_INFO
         btnRowBoxAndPasToggle,
         btnYield,
         btnPasPlasmaPercent,
         btnProductVol,
         btnFinalConc;

      // The bitmaps
      Bitmap
      * rowLine,
      * bmpMagGlass;

      // The text labels
      Display_Text
      * txtCollectConc,
      * txtPasVol;


   } _graphicalRows[MAX_ROWS];

   // ALLOCATE_RESOURCES
   // This method allocates any btnBmpPtr resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);

   // DEALLOCATE_RESOURCES
   // This method deallocates any btnBmpPtr resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

   // LABEL THE BUTTONS
   // This method calls labelButtonRow() for each rowNum, which updates
   // all of the rows to make sure they are up-to-date with changes.
   virtual void  labelAllButtons ();
   virtual float labelButtonRow (const int rowNum, const int product_num); // returns collect concentration

   // DISPLAY ALERT
   // Make sure that the concentration is in range, otherwise display an alert.
   virtual void displayAlert (float collect_conc, float final_conc);

   virtual void initRowInfo ();
   virtual void initProcedureInfo ();

   void checkForTimeOnlyConfig ();

public:


   // CONSTRUCTOR
   Screen_CONFIG_PLATELET();


   // DESTRUCTOR
   virtual ~Screen_CONFIG_PLATELET();

   //
   //   Methods invoked by callbacks (public so they can be used by non-class
   //   callback functions)
   //
   virtual void processUpButton ();
   virtual void processDownButton ();
   virtual void processYsfButton ();
   virtual void processMssToggleButton (int rowNum);
   virtual void processMaleOnlyPltBtn ();
   // virtual void process_toggle_cv();

   virtual void  update_screen_with_keypad_data (const KEYPAD_RETURN_STRUCT& return_keypad_buffer);
   virtual void  update_screen_with_listbox_data (const GUI_LISTBOX_STRUCT& rtn_listbox_buffer);
   virtual void  setRowIndicator (int rowNum);
   virtual void  awaitingCallback (const BUTTON_TYPE type, int rowNum);
   virtual float getYield (int rowNum);
   virtual float getMinConc (int btn_row);
   virtual float getMaxConc (int btn_row); // Added so that event handler can know the upper limit.
   virtual float getPercentCoMultiplier (int rowNum);
   virtual float getMinPasPlasmaPercent (int btn_row);
   virtual float getMaxYield (int btn_row);
   virtual float getMinYield (int btn_row);

};
// End of single include define guardian
#endif

/* FORMAT HASH ad2094553a45f7f1b93a11bd0615b9a6 */
