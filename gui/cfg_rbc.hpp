/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  Screen_CONFIG_RBC

*     File name:   cfg_rbc.hpp

*     Contents:    See prototype header descriptions below...

*     Description:


*************************< FOCUSSED GUI DOMAIN >****************************


$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/cfg_rbc.hpp 1.43 2009/04/24 22:46:37Z dslausb Exp dslausb $
$Log: cfg_rbc.hpp $
Revision 1.43  2009/04/24 22:46:37Z  dslausb
IT 9113 - RAS volume min changed to 80 for a single, 160 for a double.
Revision 1.42  2009/04/24 20:55:20Z  dslausb
RAS validation vars no longer needed
Revision 1.41  2009/04/08 14:38:37Z  dslausb
IT 9061
Revision 1.40  2008/10/06 22:25:38Z  dslausb
Only one validated RAS volume
Revision 1.39  2008/08/26 22:50:42Z  dslausb
IT 8114 - Change RAS validated range behavior
Revision 1.38  2008/07/09 16:29:43Z  dslausb
IT 8735 - Fix a procedure bug
Revision 1.37  2008/06/25 19:00:46Z  dslausb
Combine RAS toggle with procedure label to buy more room for localization
Revision 1.36  2008/05/14 16:11:27Z  dslausb
IT 8088 - Fix problem with DRBC split configuration
Revision 1.35  2008/05/09 20:42:35Z  dslausb
IT 8088 - Lock down bad DRBC split configurations
Revision 1.34  2008/04/08 17:03:37Z  dslausb
IT 8451 - Implement Time-Only Config Mode
Revision 1.33  2007/09/06 22:30:09Z  dslausb
IT 8198 - Fixed CFG RBC stat line warning glitch
Revision 1.32  2007/08/21 18:56:04Z  dslausb
Changes to DRBC split config functionality
Revision 1.31  2007/08/14 18:18:09Z  dslausb
IT7986: Combined cassette bit for PTF and RAS into one bit
Revision 1.30  2007/08/13 17:00:47Z  dslausb
- Made DRBC split notification automatically on if ANY procedure is configured PTF.
- Removed references to obsolete config values
Revision 1.29  2007/08/10 22:25:10Z  dslausb
RBC config screen changes for DRBC auto split
Revision 1.28  2007/05/04 15:56:03Z  dslausb
IT 7974 - Made max rbc vol configurable by sw.dat.
Revision 1.27  2007/05/04 15:19:42Z  dslausb
IT 7974 - Fixed limits on RBC config screen
Revision 1.26  2007/04/06 16:25:47Z  dslausb
Added configurable DRBC split threshold volume
Revision 1.25  2007/02/12 20:13:32Z  dslausb
IT 7869 - Combined RAS and PTF options and icons
Revision 1.24  2006/12/07 15:12:20Z  dslausb
Updates to RAS-related config screens
Revision 1.29  2006/01/12 15:44:53Z  dslausb
IT 7223 MSS GUI - Made MSS Volume a double-dash when MSS is off or disabled.
Revision 1.28  2005/11/14 18:17:58Z  dslausb
IT 7202 - Metered Storage - Added label to RBC and Plat config screens to display status of MSS (On/Off, plus Volume/%Carryover for plat).
Revision 1.27  2005/11/09 18:02:44Z  dslausb
IT 7234 - Changed event handling for the dose, crit, and scaling factor buttons to make sure that the product collectVolTxt field is updated when changes are made.
Revision 1.26  2005/11/02 22:56:17Z  dslausb
IT 7202 - Metered Storage - Improved button logging, fixed bug in toggle event handling for PTF and MSS.
Revision 1.25  2005/10/26 19:58:57Z  dslausb
IT 7202 - Metered Storage - Updated RBC and Platelet config screens with stuff for metered storage and press through filter.
Revision 1.24  2005/09/30 15:10:31Z  jheiusb
mss config.dat additions
Revision 1.23  2002/11/11 05:22:49Z  sb07663
replaced char[] constructs with guistring equivalents
Revision 1.22  2002/09/06 21:50:06  sb07663
Fixed invalid reference initializations
Revision 1.21  2002/07/19 20:16:13  rm70006
VXWorks port
Revision 1.20  2001/05/22 18:59:35Z  sb07663
RBC Yield Scaling factor additions
Revision 1.19  2000/10/08 04:03:31  sb07663
User configuration changes for single stage clinicals
Revision 1.18  2000/08/14 18:34:26  sb07663
IT4564: consolidation of config.dat parameter tables
Revision 1.17  2000/02/28 21:54:46  SB07663
IT4612: initial checkin for single stage user configuration
Revision 1.16  1999/05/07 16:20:31  SB07663
Revised RBC screen with removal of replacement fluid buttons
(which are now their own separate config screen)
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
#ifndef SYSCONFIG_RBC_HPP
#define SYSCONFIG_RBC_HPP


// CLASS INCLUDE


#include "screen_config.hpp"
#include "cfg_butt.hpp"


const int MAX_RBC_ROWS = 3;
//
///////////////////////////////////////////////////////////////////
//

class Screen_CONFIG_RBC
   : public Screen_Config
{

public:

   static Screen_CONFIG_RBC* getInstancePtr ();

   typedef enum {INACTIVE, DOSE, CRIT, RSF, MS, SS_VOL, DRBC} BUTTON_TYPE;

   typedef struct
   {
      int                x_pos;
      int                y_pos;
      Screen_CONFIG_RBC* parent;
      BUTTON_TYPE        btn_type;
      int                row;
   } ITEM_INFO;

   typedef struct
   {
      ITEM_INFO      info;
      Button_Bitmap* btnPtr;
   } BTN_INFO;

   typedef struct
   {
      ITEM_INFO info;
      Bitmap*   bmpPtr;
   } BMP_INFO;

   static float getMinRasVol (int row);
   static float getMaxRasVol (int row);
   static float getMinCrit (int row);
   static float getMaxDose (int row);

   bool checkForBadRasConfig (int row);

protected:

   // The display text objects
   Display_Text
      _txtDoseLbl,
      _txtCritLbl,
      _txtCollectVolLbl,
      _txtSolVolLbl,
      _txtProductIndicator,
      _txtMinValLbl,
      _txtMaxValLbl,

   // Split threshold creates a situation
   // where there's hole in the dose configuration
   // range.  These text fields will show that range.
      _txtSplitRangeBottomMin,
      _txtSplitRangeBottomMax,
      _txtSplitRangeTopMin,
      _txtSplitRangeTopMax,
      _txtSplitRangeBottomLbl,
      _txtSplitRangeTopLbl;

   Button_Bitmap
   * _btnScrollUp,
   * _btnScrollDown,
   * _btnScrollUpGray,
   * _btnScrollDownGray;

   Config_button_with_text
      _btnDrbcAlert,
      _btnYsf;

   static int
      _topProductNum,
      _maxProdVol;

   BUTTON_TYPE _callbackType;
   int         _callbackProduct;

   struct DATA_INFO
   {
      TEXT_STRING_ID   indicatorLbl;
      TEXT_STRING_ID   procLbl;
      CONFIG_DATA_KEYS doseKey;
      CONFIG_DATA_KEYS critKey;
      CONFIG_DATA_KEYS solVolKey;
      CONFIG_DATA_KEYS solToggleKey;
   } _dataInfo[NUM_CRIT_DOSE];

   struct ROW_INFO
   {
      BTN_INFO      rasToggleAndProdNum;
      BTN_INFO      rowLine;
      BTN_INFO      doseBtn;
      BTN_INFO      critBtn;
      BTN_INFO      solVolBtn;
      Display_Text* collectVolTxt;
      BMP_INFO      drbcIcon;
   } _rowInfo[MAX_RBC_ROWS];

   typedef enum { DISABLED, ENABLED_OFF, ENABLED_ON } TOGGLE_STATUS;

   typedef enum
   {
      RBC_PROD_2_SMALL_4_SINGLE,
      RBC_PROD_VALID_SINGLE,
      RBC_PROD_BETWEEN_SINGLE_AND_DOUBLE,
      RBC_PROD_VALID_DOUBLE,
      RBC_PROD_2_BIG_4_DOUBLE
   } RBC_PROD_TYPE;

   float _srbcDoseMax;

   // ALLOCATE_RESOURCES
   // This method allocates any btnPtr resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);

   // DEALLOCATE_RESOURCES
   // This method deallocates any btnPtr resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();
   virtual void updateAllCollectVolumes ();
   virtual void label_the_buttons ();
   virtual void label_the_buttons_by_row (int row);

   // New new
   static float getDose (int product);
   static float getCrit (int product);
   static float getCollectVol (int product);

   // Update functions
   bool  updateRasToggle (int row);
   float updateDose (int row);
   float updateCrit (int row);
   float updateCollectVol (int row);
   int   updateRasVol (int row);
   void  updateDrbcIcon (int row);
   void  updateAllDrbcIcons ();

   // New new

   float updateDrbcButton ();

   static bool atLeastOnePtfIsOn ();

   void checkForTimeOnlyConfig ();
   bool checkForBadSplitConfiguration (int updatedProdRow = -1, float updatedProdDose = -1.0);

   void displayCustomDoubleRange (const float bottomRangeMin,
                                  const float bottomRangeMax,
                                  const float topRangeMin,
                                  const float topRangeMax,
                                  const char* format);

   void clearCustomDoubleRange ();

   static bool drbcAlertIsOn ();

   static bool isDrbcProduct (int row);

   void display_alert (const float dose);

public:


   // CONSTRUCTOR
   Screen_CONFIG_RBC();

   // DESTRUCTOR
   virtual ~Screen_CONFIG_RBC();

   //
   //   Methods invoked by callbacks (public so they can be used by non-class
   //   callback functions)
   //
   virtual void process_up_button ();
   virtual void process_down_button ();
   virtual void process_drbc_threshold_button ();
   virtual void process_rbc_ysf_button ();
   virtual void update_screen_with_keypad_data (const KEYPAD_RETURN_STRUCT& return_keypad_buffer);
   virtual void set_indicator (const int row);
   virtual void awaiting_callback (const BUTTON_TYPE type, const int row);
   virtual void process_toggle_button (const BTN_INFO* btn_info);

   void putUpDoseKeypad (int prodRow);
   void putUpDrbcTresholdKeypad ();
   void verifyDrbcBitReflectsPtfToggles ();
   bool isBadSplitConfigurationPossible (int prodRow = -1);
};
// End of single include define guardian
#endif
//
///////////////////////////////////////////////////////////////////
// end of cfg_rbc.hpp file

/* FORMAT HASH 2114accf334c7c48cb8b0c57d76143a8 */
