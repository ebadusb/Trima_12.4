/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  Screen_CONFIG_MACHINE

*     File name:   cfg_mach.hpp

*     Contents:    See prototype header descriptions below...

*     Description:


*************************< FOCUSSED GUI DOMAIN >****************************

Revision 1.25  2008/04/08 17:02:32Z  dslausb
IT 8451 - Implement Time-Only Config Mode
Revision 1.24  2006/09/13 22:23:56Z  dslausb
Minor tweaks to RBC screen layout and run summary composition.
Revision 1.23  2006/08/17 20:42:42Z  dslausb
- Machine screen no longer has barcode button.  Scanned barcodes automagically go into the barcode field when you're on that screen.
- Confirm single deletion box show the REF number you're confirming deletion for
Revision 1.22  2006/08/09 16:52:19Z  dslausb
IT 7682 - Changed to 2-file cassette entry functionality
Revision 1.21  2006/08/03 22:52:43Z  dslausb
Fixed cassette deletion
Revision 1.20  2006/07/28 23:28:59Z  dslausb
Redesigned cassette entry
Revision 1.19  2006/05/30 15:02:41Z  dslausb
Machine Screen shows number for digits needed for set / admin codes.
Revision 1.18  2006/04/21 21:41:05Z  dslausb
7272 - Fixed cassette entry in machine screen.
Revision 1.17  2006/04/21 20:41:32Z  dslausb
IT 7272 - Smoothed out functionality for admin and operator cassette entry.
Revision 1.16  2006/04/19 13:30:50Z  dslausb
Changed format of cassette.dat file and hooked it up with Jerry's CRC code.
Revision 1.15  2006/04/07 19:18:15Z  dslausb
First draft of cassette.dat file i/o stuff
Revision 1.14  2006/03/27 22:25:09Z  dslausb
Revision 1.13  2002/11/11 05:19:41Z  sb07663
replaced char[] constructs with guistring equivalents
Revision 1.12  2002/07/19 15:16:47  rm70006
VXWorks Port
Revision 1.11  2002/05/07 13:55:46Z  sb07663
First cut at VxWorks compatibility
Revision 1.10  2000/10/08 04:02:14  sb07663
User configuration changes for single stage clinicals
Revision 1.9  2000/08/14 18:33:54  sb07663
IT4564: consolidation of config.dat parameter tables
Revision 1.8  2000/02/28 21:54:26  SB07663
IT4612: initial checkin for single stage user configuration
Revision 1.7  1999/07/14 22:55:26  BS04481
Clock setting method changed.  See IT 1905.  Clock setting is no
longer vulnerable to time shifts of less than 10 seconds.  Time can
now be set from Everest/Sherpa.
Revision 1.6  1996/12/06 02:19:20  Barry_Fulkerson
- removed printf's
Revision 1.5  1996/12/05 19:29:53  Barry_Fulkerson
- finished adding system time setting to the time button
Revision 1.4  1996/10/24 18:16:37  Barry_Fulkerson
- added system time/date to button face
Revision 1.3  1996/10/24 07:43:28  Barry_Fulkerson
- added MACHINE Screen
Revision 1.2  1996/10/07 19:52:42  Barry_Fulkerson
added File Title Block comment section and MKS keywords

*/

// Single include define guardian
#ifndef SYSCONFIGMACH_HPP
#define SYSCONFIGMACH_HPP

/** include files **/

// CLASS INCLUDE
#include "screen_config.hpp"
#include "cfg_butt.hpp"
#include "set_listbox.hpp"

//
///////////////////////////////////////////////////////////////////
//
class Screen_CONFIG_MACHINE
   : public Screen_Config
{
private:

   Config_button_with_text
            _cfgBtnDateFormat,
            _cfgBtnTimeFormat,
            _cfgBtnSetTime,
            _cfgBtnSetDate,
            _cfgBtnSetAudio;

   Button_Bitmap
      _btnRefNum,   // The 3 or 5 digit set ID
      _btnSetCode,  // The CRC'ed functionality code
      _btnAddSet,   // Check and add the entered set ID and functionality code.
      _btnRemoveSet;

   Display_Text
      _txtRefNum,
      _txtSetCode,
      _txtBarcode,
      _txtBarcodeLbl,
      _txtNumDigits;

   Bitmap    _bmpCassetteBoxBg;

   guistring _catalogNumber,
             _adminCode,
             _barcodeString,
             _setIdAllocationString,
             _confirmBoxOneAllocationString,
             _confirmBoxAllAllocationString;

   bool _haveCatalogNumber, _haveAdminCode;

   void setAddButtonIsActive (bool isActive);
   void setRemoveButtonIsActive (bool isActive);
   void checkAddButtonStatus ();
   void displayStatusAttention (TEXT_STRING_ID warningMsg);
   void clearStatusAttention ();
   void clearCassetteBox ();
   void setMessage (TEXT_STRING_ID stringID);
   void clearMessage ();
   void initListboxStrings ();
   void HandleBarcodeMsg ();

   SetListboxItemChosen       _msgListboxData;
   ConfirmBoxConfirmMsg       _msgDeletionConfirmed;

   Message<BADGE_BARCODE_MSG> _msgBarcode; // get the barcode message.

protected:

   virtual void listboxCB ();
   virtual void confirmCB ();
   void         deleteAll ();
   void         deleteOne ();

   void checkForTimeOnlyConfig ();

public:
   // CONSTRUCTOR
   Screen_CONFIG_MACHINE();

   // DESTRUCTOR
   virtual ~Screen_CONFIG_MACHINE();

   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);

   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

   // CONFIG BUTTONS
   virtual void processSetTimeBtn ();
   virtual void processSetDateBtn ();
   virtual void processSetAudioBtn ();
   virtual void processDataFormatBtn ();
   virtual void processTimeFormatBtn ();
   virtual void processSetCodeBtn ();
   virtual void processRefNumBtn ();
   virtual void processAddSetBtn ();
   virtual void processRemoveSetBtn ();
   virtual void callConfirmDeletionBox (Screen_SET_LISTBOX::LISTBOX_CALLBACK_VAL callbackVal);


   // MESSAGE NOTIFIES

   virtual void update_screen_with_keypad_data (const KEYPAD_RETURN_STRUCT& return_keypad_buffer);
   virtual void update_screen_with_listbox_data (const GUI_LISTBOX_STRUCT& rtn_listbox_buffer);
}; // END of Screen_CONFIG_MACHINE Class Declaration


// End of single include define guardian
#endif


//
///////////////////////////////////////////////////////////////////
// end of cfg_mach.hpp file

/* FORMAT HASH ab17e8679b2b61730073987520d7ba23 */
