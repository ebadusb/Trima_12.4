/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995 - 1998 COBE BCT, Inc. All rights reserved

*     Author:      Michael Cobb

*     Class name:  Screen_CONFIRM_DON_INFO

*     File name:   confirmDonInfo.hpp

*     Contents:    Definition of class methods and attributes

*     Description: Class header file for confirmDonInfo.cpp

*     Note: This file is ported, with a few changes, from donordl.hpp.  The screen name
      has changed as well.  That's because this is now being used not just for vista
      downloads, but also for confirming donor data before connecting the donor.
      See IT 8446 for more details.

*************************< FOCUSSED GUI DOMAIN >****************************

*/

// Single include define guardian
#ifndef DONORDL_HPP
#define DONORDL_HPP

#include <vxworks.h> // Needed for CPU compiler directives in header.

// GUI global element include
#include "guiglobs.hpp"

// GUI class includes
#include "aphwind.hpp"
#include "display_text.h"

// GUI <-> safety screen include
#include "safproc.h"


// CLASS DECLARES

class Screen_CONFIRM_DON_INFO
   : public Base_Apheresis_Window
{
public:
   // METHODS

   // CONSTRUCTOR
   Screen_CONFIRM_DON_INFO();


   // DESTRUCTOR
   virtual ~Screen_CONFIRM_DON_INFO();


   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();


   // RESET_INTERNAL_SCREEN_STATE
   // This method will reset the DONVITAL internal screen state
   virtual void reset_internal_screen_state ();


   // PROCESS_LOWLEFT_BUTTON
   // This method is called from the virtual C callback stub and executes
   // "lower left"
   virtual void process_lowleft_button ();

   // PROCESS_CONTINUE_BUTTON
   // This method is called from the virtual C callback stub and executes
   // "continue" button.
   virtual void process_continue_button ();

   // PROCESS_CANCEL_BUTTON
   // This method is called from the virtual C callback stub and executes
   // "cancel" button.
   virtual void process_goback_button ();

   void gotoDonInfo ();

private:
   // METHODS

   // DONOR_VITALS_GET_STRUCT_MANAGER
   // This method manages all structs recieved via messaging
   void donor_vitals_get_struct_manager ();


   void shutdown ();


   void screen_notify_handler ();

   // DONOR_VITALS_STRUCT_INITIALZATION
   // This method initializes donor vitals struct values as follows
   void donor_vitals_struct_initialization ();


   // SCREEN_INITIALZATION
   // This method initializes screen components
   void screen_initialization ();


   // SCREEN_INPUT_FORMAT
   // This method formats and outputs all data to be displayed on the screen
   void screen_input_format ();

   // This method will take the data in SDonorVitals struct (as returned from
   //  safety and display it to the operator, for their "commit" or "rejection".
   void display_donor_download_data ();


   // This method gets the donor picture from Vista, validates it, and set up the
   // appropriate donor picture member variables.
   void get_donor_picture ();


   // This method calculates the CRC value of a bitmap file (donor picture).
   // This value is compared to the one sent by VIP.
   int calculate_donor_picture_crc (const char* filename, unsigned long* finalCrc);

private:
   // ATTRIBUTES
   bool donor_data_confirmed;              // flag to determine previous

   // access to the safety structure for parameter passing
   SDonorVitals gui_donor_vitals_struct;

   // The text objects
   Display_Text

      _txtDonorName,
      _txtDonorDob,
      _txtDonorGender,
      _txtDonorHeight,
      _txtDonorWeight,
      _txtDonorBloodType,
      _txtDonorHct,
      _txtDonorPltPrecount,
      _txtDonorTbv,

      _txtDonorNameAndDobLbl,
      _txtDonorGenderLbl,
      _txtDonorHeightLbl,
      _txtDonorWeightLbl,
      _txtDonorBloodTypeLbl,
      _txtDonorHctLbl,
      _txtDonorPltPrecountLbl,
      _txtDonorTbvLbl;

   // The placard bitmap objects
   Bitmap
      _bmpNameAndDobBox,
      _bmpDonorStats,
   * _bmpPtrDonorPhoto;

   Button_Bitmap _btnDonorInfo;

   // This will contain the bitmap ID of either the donor photo or the default bitmaps for male/female
   BITMAP_ID donor_bitmap_id;

   bool      _usingHgbUnits; // Are we using hemoglobin units (as opposed to crit)?


   // Incoming messages
   DonorDataStatusMsg donorDataUpdateMsg;

   // Shutdown state
   bool _exiting;
   bool _goback;                // true if the goback button was pressed (tells
                                // us what screen to exit to)

   anAlarmMsg _unexpectedErrorAlarm;

#if (CPU==SIMNT) // This stuff is for unit testing and screenshots.
public:
   enum  CONFIRM_DON_INFO_SCREEN_VERSION
   {
      CONFIRM_DON_INFO_VERSION_MALE,
      CONFIRM_DON_INFO_VERSION_FEMALE
   };

private:
   static CONFIRM_DON_INFO_SCREEN_VERSION _versionToShow;

   void toggleVersionToShow () { _versionToShow = ((_versionToShow==CONFIRM_DON_INFO_VERSION_MALE) ? CONFIRM_DON_INFO_VERSION_FEMALE : CONFIRM_DON_INFO_VERSION_MALE); }

protected:
   virtual void handleTraverseMessage ();

#endif // #if (CPU==SIMNT)
};

#endif

/* FORMAT HASH 9477b06faa7b07a4e76d48f14a5b996f */
