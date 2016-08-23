/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1999 COBE BCT, Inc. All rights reserved

*     Author:       Dan Slauson
    (based largely on code copied and pasted from Terry Wahl)

*     Class name:   runsum_product

*     File name:    runsum_product.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class will receive a run summary record from Proc.
         This record contains a set of flags that indicate whether a
         specific product should be measured. This screen will display
         a button for each of those warning flags (currently up to 6).
         The buttons are labeled with a terse phrase about the
         measurement required. If the operator pushes one of the buttons,
         an extended message will appear to the right of the button array.
         The pressed button will remain in a pressed state while its
         extended message is displayed.


*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/runsum_product.hpp 1.24 2008/10/03 16:19:57Z dslausb Exp dslausb $
$Log: runsum_product.hpp $
Revision 1.24  2008/10/03 16:19:57Z  dslausb
Move away from dynamic memory where possible
Revision 1.23  2008/08/16 19:12:16Z  dslausb
Mark's Screenshot Code
Revision 1.23  2008/08/15 20:37:05Z  mereusb
IT 8107: Change for screenshot scripts.
Revision 1.22  2008/07/14 14:30:10  dslausb
More stringId changes requested by localization
Revision 1.21  2008/07/07 18:27:16Z  dslausb
More localization string id standardization
Revision 1.20  2008/06/10 20:45:32Z  mereusb
IT 8107: Lots of little foundational changes to help ease writing of screenshot scripts.
Revision 1.19  2008/04/01 14:59:05  dslausb
IT 8077 - Implement separate end run screens for RAS and PAS
Revision 1.18  2007/09/11 21:35:18Z  dslausb
Update to run summary and flagging functionality
Revision 1.17  2007/08/15 22:44:36Z  dslausb
Separate flags for two RBC products
Revision 1.16  2007/08/10 15:26:45Z  dslausb
Screen changed for DRBC Automated Split
Revision 1.15  2007/01/16 16:06:51Z  dslausb
-- IT 7482 - Add RBC leukoreduction message and flag
-- Changed screen transitions for RBC run summary screen
-- Changed cassette listbox filtering to allow for PTF and RAS
Revision 1.14  2006/12/07 15:58:16Z  dslausb
Ported over late GUI changes to 5.2
Revision 1.13  2006/11/08 17:55:19Z  dslausb
More slight tweaking to allow localized reason strings to fit
Revision 1.12  2006/11/07 23:25:07Z  dslausb
Slight tweak of extended textbox layout to ease localization
Revision 1.11  2006/11/06 21:04:10Z  dslausb
Reverted from dual guistring classes back to one (no longer necessary since we drastically increased the buffer size in IT 7165)
Revision 1.10  2006/10/06 23:04:18Z  dslausb
Added SIMNT-only message for switching stuff in runsummaries.
Revision 1.9  2006/09/11 19:23:38Z  dslausb
Revision 1.8  2006/06/29 20:52:31Z  dslausb
IT 7607 - Reformatting on end run summary screens
Revision 1.7  2006/06/29 18:46:30Z  dslausb
IT 7607 - Fixed up labeling in end run summary screens
Revision 1.6  2006/06/13 18:31:11Z  dslausb
IT 7592 - Deallocated string so that it doesn't show up on the next screen.
Revision 1.5  2006/06/09 19:35:33Z  dslausb
Added a function to deallocate labeling
Revision 1.4  2006/06/08 20:25:04Z  dslausb
Changed layout according to gui review on 6/5
Revision 1.3  2006/05/08 15:50:05Z  dslausb
IT 7151 - Changed new screens so that only necessary stuff shows up.
Revision 1.2  2006/05/05 21:45:58Z  dslausb
IT 7212 - Separated up flag and labeling run summary screens into their respective products
Revision 1.1  2006/05/02 18:55:26Z  dslausb
Initial revision
*/

// Single include define guardian
#ifndef __RUNSUM_PRODUCT_HPP__
#define __RUNSUM_PRODUCT_HPP__

// GUI <-> procedure run summary msg
#include "runsummary.h"
#include "pqinformation.h"

// max number of extended text items + 1 for header text.  Note that there may be
// more reasons for a given button, so the lowest priority reasons will not be shown.
#define EXTENDED_TEXT_MAXITEMS 7

// CLASS INCLUDE
#include "gui.h"
#include "guiglobs.hpp"
#include "string_info.h"
#include "aphwind.hpp"
#include "btncanc.hpp"
#include "btngobk.hpp"
#include "btncont.hpp"
#include "btnlowl.hpp"
#include "bitmap.h"
#include "display_text.h"

#include "tablecol.hpp"
#include <list.h>

// MESSAGE DECLARES
#include "message.h"
#include "an_alarm.h"

// Define the margin for the text on the Verification buttons.
#define  VERIF_BTNS_TEXT_MARGIN 10

#define BTN_PRD_WRN_Y 212
#define BITMAP_BUTTON_PRODUCT_WARNING_BTN_INC 60

// Prototypes for functions in reasonToString.cpp
TEXT_STRING_ID getStringIdFromReasonCode (ENUMERATED_REASON_TO_SHOW reasonToShow);
TEXT_STRING_ID getReasonHeaderIdFromVerifyFlag (ENUMERATED_FLAG verifyFlag);
TEXT_STRING_ID getProdVerifyCategory (GUI_SCREEN_ID screenId);
TEXT_STRING_ID getProdVerifyDoLabel (GUI_SCREEN_ID screenId);
TEXT_STRING_ID getProdVerifyDontLabel (GUI_SCREEN_ID screenId);
TEXT_STRING_ID getProdVerifyStatusLine (GUI_SCREEN_ID screenId);

enum LOGICAL_BUTTON
{
   LB_COUNT_PLATELET_PRODUCT,
   LB_COUNT_PLASMA_PRODUCT,
   LB_VERIFY_PLATELET_YIELD_AND_VOLUME,
   LB_VERIFY_PLASMA_VOLUME,
   LB_VERIFY_RBC_PRODUCT,
   LB_VERIFY_RBC_WBCS,
   LB_VERIFY_RBC_QUALITY,
   LB_PLATELET_CONC_OUT_OF_RANGE
};

enum LABEL_TOGGLE
{
   // Note: The code counts on DONT_LABEL_PRODUCT being zero
   // and LABEL_PRODUCT being 1 (for conversion from bool).
   DONT_LABEL_PRODUCT=0,
   LABEL_PRODUCT     =1,
   DONT_SHOW_LABEL_BOX
};

enum BUTTON_PRESSED_STATUS
{
   UP   = 1,
   DOWN = 2
};

// CLASS DECLARES
class runsum_product
   : public Base_Apheresis_Window
{
public:
   runsum_product(GUI_SCREEN_ID screen_id);

   virtual ~runsum_product();

protected:
   // Avoid forward referencing bickering
   class LogicalButtonCollection;
   class PhysicalButtonElement;
   class LogicalButtonElement;
   class PhysicalButtonCollection;

   /////////////////////////////////////////////////////////////////////
   //// Local classes of runsum_product
   /////////////////////////////////////////////////////////////////////

   ////PhysicalButtonElement/////////////////////////////////////////////////////////////////
   class PhysicalButtonElement
   {
   private:

      LogicalButtonElement* ptrLogicalButtonElement;
      Button_Bitmap*        ptrPhysicalButton;
      BUTTON_PRESSED_STATUS toggleState;
      BITMAP_ID             upBitmap;
      BITMAP_ID             downBitmap;

   public:
      PhysicalButtonElement ()
         : ptrLogicalButtonElement(NULL), ptrPhysicalButton(NULL),
           toggleState(UP), upBitmap(BITMAP_NULL), downBitmap(BITMAP_NULL)
      {}

      void setPhysBtnPtr (Button_Bitmap* newPhysicalBtnPtr)
      { ptrPhysicalButton = newPhysicalBtnPtr; }

      Button_Bitmap* getPhysBtnPtr () const
      { return ptrPhysicalButton; }

      void setLogicalBtnPtr (LogicalButtonElement* newLogicalBtnPtr)
      { ptrLogicalButtonElement = newLogicalBtnPtr; }

      LogicalButtonElement* getLogicalBtnPtr () const
      { return ptrLogicalButtonElement; }

      void setBtnToggleState (BUTTON_PRESSED_STATUS btnState)
      { toggleState = btnState; }

      BUTTON_PRESSED_STATUS getBtnToggleState () const
      { return toggleState; }

      const BITMAP_ID btnUpBmp () const { return upBitmap; }
      void            btnUpBmp (const BITMAP_ID id) {upBitmap = id; }
      const BITMAP_ID btnDownBmp () const { return downBitmap; }
      void            btnDownBmp (const BITMAP_ID id) {downBitmap = id; }
   };  // End of class PhysicalButtonElement

   ////LogicalButtonElement/////////////////////////////////////////////////////////////////
   class LogicalButtonElement
   {
   public:
      typedef list<TEXT_STRING_ID> EXTENDED_TEXT_LIST;

   private:
      LOGICAL_BUTTON const logicalId;
      TEXT_STRING_ID const logicalBtnTxt;
      EXTENDED_TEXT_LIST   extendedList;

   public:
      LogicalButtonElement (LOGICAL_BUTTON newLogicalId, TEXT_STRING_ID newLogicalBtnTxt)
         : logicalId (newLogicalId),
           logicalBtnTxt(newLogicalBtnTxt)
      { }

      LOGICAL_BUTTON getLogicalId () const { return logicalId; }

      TEXT_STRING_ID getLogicalBtnTxt () const { return logicalBtnTxt; }


      void clearExtendedElements ()
      {
         while (extendedList.size() > 0) extendedList.pop_front();
      }

      void addExtendedElement (TEXT_STRING_ID newTxtId)
      {
         extendedList.push_back(newTxtId);
      }

      EXTENDED_TEXT_LIST& extendedTxt () { return extendedList; }
   };  // End of class LogicalButtonElement

   ////PhysicalButtonCollection/////////////////////////////////////////////////////////////////
   class PhysicalButtonCollection
      : public Table_Collection<PhysicalButtonElement>
   {
   private:
      runsum_product* parent;

      // Button allocation index
      int physBtnIndex;

      // First button ID
      GUI_BUTTON_ID firstBtnId;

      // X position for all "stacked" buttons
      int xPos;

      // Y initial -- first (or top) button's Y location
      int yPosInit;

      // Y increment between the "stacked" buttons
      int yPosIncrement;

      // make empty constructor illegal
      PhysicalButtonCollection();

   public:
      //// CONSTRUCTOR
      PhysicalButtonCollection (runsum_product* parent,
                                short newXpos,
                                short newYposInit,
                                short newYposIncrement,
                                GUI_BUTTON_ID newFirstBtnId)
         : parent(parent),
           physBtnIndex(0),
           xPos(newXpos),
           yPosInit(newYposInit),
           yPosIncrement(newYposIncrement),
           firstBtnId(newFirstBtnId)
      {}     // end of constructor

      ////DESTRUCTOR
      virtual ~PhysicalButtonCollection()
      { freeBtnCollection (); }    // end of destructor

      //// ALLOCATE_NEXT_PHYSICAL_BUTTON
      void allocNextPhysBtn (LogicalButtonElement* newLogicalBtnPtr,
                             void (* callback_routine_ptr) (void*),
                             BITMAP_ID newUpBmpId,
                             BITMAP_ID newDownBmpId)
      {
         if (!newLogicalBtnPtr || !callback_routine_ptr)
         {
            DataLog (log_level_gui_error) << "Unable to allocate physical button due to bad pointers." << endmsg;
            _FATAL_ERROR(__FILE__, __LINE__, "Bad Pointer");
         }

         // button text buffer
         guistring btnTxtBuf;

         // create a physical button element
         PhysicalButtonElement* currentPhysBtn = new PhysicalButtonElement();

         // assign the pointer to the logical button element
         currentPhysBtn -> setLogicalBtnPtr (newLogicalBtnPtr);

         // set the newly allocated button in the UP state
         currentPhysBtn -> setBtnToggleState (UP);

         // save off supplied bitmap IDs
         currentPhysBtn -> btnUpBmp(newUpBmpId);
         currentPhysBtn -> btnDownBmp(newDownBmpId);

         // calculate the next Y coordinate for the button
         // int next_Y = BITMAP_BUTTON_PRODUCT_WARNING1_Y +
         //             (BITMAP_BUTTON_PRODUCT_WARNING2_Y - BITMAP_BUTTON_PRODUCT_WARNING1_Y) * physBtnIndex;
         int nextBtnY = yPosInit + (yPosIncrement * physBtnIndex);

         // new and allocate the button - use our own text id to get consistent text styling
         Button_Bitmap* btnPtr =  new Button_Bitmap((GUI_BUTTON_ID)((int)firstBtnId + physBtnIndex),
                                                    (short)xPos, (short)nextBtnY,
                                                    newLogicalBtnPtr->getLogicalBtnTxt(),
                                                    newUpBmpId, newDownBmpId,
                                                    false, false);

         // do the Photon allocation and parent it to the calling screen
         btnPtr -> allocate_resources(*parent);

         // set the callback pointer
         btnPtr -> set_callback (callback_routine_ptr,  btnPtr);

         // set its button pointer
         currentPhysBtn -> setPhysBtnPtr (btnPtr);

         // add it to the collection
         add_table_entry (currentPhysBtn);

         // bump the physical index
         physBtnIndex++;
      }    // End of allocNextPhysBtn


     #if (CPU==SIMNT)
      void setRadioBtnsUp ()
      {
         // Set the collection cursor to the top of the collection
         set_cursor_to_top ();

         // Get first physical button
         PhysicalButtonElement* physBtn = get_next_entry ();

         while (physBtn != NULL)
         {
            // Assign button to a pointer
            Button_Bitmap* btnPtr = physBtn -> getPhysBtnPtr();

            // If this is not THE button set it to an "up" position
            if ((physBtn -> getBtnToggleState()) == DOWN)
            {
               // set the up bitmap to "up"
               btnPtr -> set_up_bitmap (physBtn->btnUpBmp());

               // set the button text color to a "up" color
               btnPtr -> set_button_text_color (WHITE);      // GREY_DARK

               // make sure that it is active
               btnPtr -> enable_callbacks();

               // set the button status to "up"
               physBtn -> setBtnToggleState(UP);
            }
            // Get next physical button
            physBtn = get_next_entry ();
         }    // End of "while (physBtn != NULL)
      }
     #endif /* CPU==SIMNT */

      //// SET_RADIO_BUTTON_DOWN
      void setRadioBtnDown (Button_Bitmap* selectedButtonWidgetPtr)
      {
         // Set the collection cursor to the top of the collection
         set_cursor_to_top ();

         // Get first physical button
         PhysicalButtonElement* physBtn = get_next_entry ();

         // Radio button found flag
         int radioBtnFound = 0;

         while (physBtn != NULL)
         {
            // Assign button to a pointer
            Button_Bitmap* btnPtr = physBtn -> getPhysBtnPtr();

            // If this is not THE button set it to an "up" position
            if (btnPtr != selectedButtonWidgetPtr)
            {
               if ((physBtn -> getBtnToggleState()) == DOWN)
               {
                  // set the up bitmap to "up"
                  btnPtr -> set_up_bitmap (physBtn->btnUpBmp());

                  // set the button text color to a "up" color
                  btnPtr -> set_button_text_color (WHITE);       // GREY_DARK

                  // make sure that it is active
                  btnPtr -> enable_callbacks();

                  // set the button status to "up"
                  physBtn -> setBtnToggleState(UP);
               }

            }
            else       // set THE button to a "down" position
            {
               // the widget parameter passed is one of the valid physical buttons,
               //  remember that we found it.
               radioBtnFound = 1;

               // set the up bitmap to "down" pictorial (confusing, isn't it?)
               btnPtr -> set_up_bitmap (physBtn->btnDownBmp());

               // set the button text color to a "down" color
               btnPtr -> set_button_text_color (YELLOW);      // WHITE

               // make sure that it is INactive
               btnPtr -> disable_callbacks();

               // set the button status to "down"
               physBtn -> setBtnToggleState(DOWN);
            }

            // Get next physical button
            physBtn = get_next_entry ();
         }    // End of "while (physBtn != NULL)

         if (!radioBtnFound)
         {
            _FATAL_ERROR (__FILE__, __LINE__, "No physical button matches the pushed-button widget pointer during radio button lookup");
         }
      }    // End of SET_RADIO_BUTTON_DOWN

      //// GET_EXTENDED_TEXT_ID
      LogicalButtonElement::EXTENDED_TEXT_LIST& getExtendedTxt (Button_Bitmap* selectedButtonWidgetPtr)
      {
         // Set the collection cursor to the top of the collection
         set_cursor_to_top ();

         // Get first physical button
         PhysicalButtonElement* physBtn = get_next_entry ();

         // If this is called while no physical buttons have been allocated
         //  return an innocuous null string ID.
         if (physBtn == NULL)
         {
            anAlarmMsg alarm(GUI_VERIFPRD_NO_EXTENDED_TEXT);
            alarm.setAlarm();
         }

         while (physBtn != NULL)
         {
            // Assign button to a pointer
            Button_Bitmap* btnPtr = physBtn -> getPhysBtnPtr();

            // If this is THE button, get its logical button's extended text and return it.
            if (btnPtr == selectedButtonWidgetPtr)
            {
               return physBtn->getLogicalBtnPtr()->extendedTxt();
            }

            // Get next physical button
            physBtn = get_next_entry();

         }    // End of "while (physBtn != NULL)

         // we shouldn't fall through -- if we did, it means that we didn't find the
         //   physical button (as ID'ed by the widget pointer).
         _FATAL_ERROR (__FILE__, __LINE__, "No physical button matches the pushed-button widget pointer during extended text lookup");

         // Satisfy AR compiler
         static LogicalButtonElement::EXTENDED_TEXT_LIST invalid;
         return(invalid);
      }    // End of GET_EXTENDED_TEXT_ID

      //// FREE_BUTTON_COLLECTION
      void freeBtnCollection ()
      {
         // Set the collection cursor to the top of the collection
         set_cursor_to_top();

         // Get first physical button
         PhysicalButtonElement* physBtn = get_next_entry ();

         while (physBtn != NULL)
         {
            // if a button is allocated and pointed-to, free its resources and
            // free the button instance.
            Button_Bitmap* btnPtr = physBtn -> getPhysBtnPtr ();

            if (btnPtr != NULL)
            {
               // Free up any Photon resources
               btnPtr->deallocate_resources();

               // Free up the bitmap button instance
               delete btnPtr;

               // Set the GUI button pointer in the physical button element to null
               //  in case this is called twice before the collect is destroyed.
               physBtn->setPhysBtnPtr(NULL);
            }

            // NOTE: the actual physical button elements will be freed when
            //      the physical button collection is destroyed in the table collection base class.

            // Get next physical button
            physBtn = get_next_entry();
         }    // End of "while (physBtn != NULL)

      }    // End of FREE_BUTTON_COLLECTION

   };  // End of class PhysicalButtonCollection

   ////LogicalButtonCollection/////////////////////////////////////////////////////////////////

   class LogicalButtonCollection
      : public Table_Collection<LogicalButtonElement>
   {
   public:
      //// CONSTRUCTOR
      LogicalButtonCollection () {}

      //// Defines the mapping of text for all logical buttons
      static TEXT_STRING_ID getTextForLogicalButton (LOGICAL_BUTTON button_id);

      //// CREATE_LOGICAL_BUTTON
      void createLogicalButton (LOGICAL_BUTTON button_id);

      //// GET_LOGICAL_BUTTON
      LogicalButtonElement* getLogicalButton (LOGICAL_BUTTON logical_button_id)
      {
         // Set the cursor to the beginning of the table
         set_cursor_to_top();

         // Get first logical button pointer
         LogicalButtonElement* log_btn_ptr = get_next_entry ();

         while (log_btn_ptr != NULL)
         {
            if ((log_btn_ptr -> getLogicalId ()) == logical_button_id)
            {
               // we found it, return the pointer to the caller
               return(log_btn_ptr);
            }

            // Get next logical button
            log_btn_ptr = get_next_entry();

         }    // End of "while (log_btn_ptr != NULL)

         // return NULL because we didn't find the logical button in question.
         DataLog (log_level_gui_error) << "Unable to find logical button." << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "Logical button not found");
         return((LogicalButtonElement*)NULL);
      } // End of GET_LOGICAL_BUTTON
   };   // End of class LogicalButtonCollection

   ////////////////////////////////////////////////////////////////////
   //// End of Local classes
   ////////////////////////////////////////////////////////////////////
public:
   virtual void allocate_resources (const char* allocation_parameter);
   virtual void deallocate_resources ();

   // PROCESS_BUTTON_ARRAY_PRESS
   // This method will be called whenever one of the product warning buttons is
   // pressed. It will be passed the widget pointer of the button that has just
   // been pressed. Using this along with the physical button collection, it will
   // cause that button to remain pressed/inactive (a la radio button),
   // and all others to be raised and active. The extended text for the pressed
   // button will also be displayed in the extended text cartouche.
   virtual void processButtonPress (Button_Bitmap* ptrPressedButtonWidget);

   // INITIALIZE_STATUS_LINE
   // This is to initialize the status line one time
   virtual void initialize_status_line ();

  #if (CPU==SIMNT)
   virtual void switchReasons ();
  #endif

protected:
   Message<RUN_SUMMARY_STRUCT> _msgRunSummaryStats;

   // Product verification (run summary) message latch - only one message processed per allocation.
   bool _runsumMsgAlreadyProcessed;

   Bitmap
      _extendedDataCartouche,  // The product verification extended data cartouche bitmap object
      _sectionLine,
      _bitmapLblTextBox;

   // The display text label object: for the product verification title over the button array.
   Display_Text
      _txtProductVerifLbl,
      _textLblProdBody,
      _textDontLblProdBody,
   * _extendedheader,
      _extendedText[EXTENDED_TEXT_MAXITEMS];

   // The logical button collection (allocated once at screen construction)
   LogicalButtonCollection _logicalButtons;

   // The pointer to hold the physical button collection (allocated after receipt of the runsummary message -- once per allocation)
   PhysicalButtonCollection* _ptrPhysicalButtons;

   // The Product Quality data access class
   PQInformation PQA;

   // The status line string id
   TEXT_STRING_ID _statLineStrId;

   // SET_EXTENDED_TEXT
   // This method takes a text ID and sets the text in the extended text area
   // with the text represented by that ID
   virtual void setExtendedText (LogicalButtonElement::EXTENDED_TEXT_LIST& textlist);

   virtual void process_continue_button ();

   virtual void setLabel (LABEL_TOGGLE shouldLabel);

private:
   virtual void processProductWarningData () = 0;

   void requestRunsumStats ();

protected:
    #if (CPU==SIMNT)
   virtual void handleTraverseMessage ();
    #endif // #if (CPU==SIMNT)

   void addReasonToButtonIfNeeded (LogicalButtonElement* btnElement,
                                   ENUMERATED_FLAG flagType,
                                   ENUMERATED_REASON_TO_SHOW reasonToShow);

}; // End of runsum_product declaration

// End of single include define guardian
#endif

/* FORMAT HASH ca823256053228f753316a5d093c5ee8 */
