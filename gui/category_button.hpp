/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2000 Gambro BCT Inc. All rights reserved

*     Author:       Terry Wahl

*     Class names:  Category_Button

*     File name:    category_button.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This file contains the header for the button type used to categorized
         (store under a grouping like saline solution) the input scans.

*************************< FOCUSSED GUI DOMAIN >***************************/

// Single include define guardian
#ifndef CATEGORY_BUTTON
#define CATEGORY_BUTTON


// composed classes includes
//
#include "button_bitmap.h"
#include "display_text.h"
#include "scan_structure.h"
#include "metadirectory.hpp"
#include "barcode_cartouche.hpp"
#include "category_button_parms.h" // enums / manifests etc.
#include "barcode_object_list.h"
#include "audiofb.hpp"


#define  CAT_BTN_MARGIN 8


class Category_Button_Collection;  // Ensure to the compiler that this REALLY IS a class
class MetaDirectory;


class Category_Button
   : public Button_Bitmap
{
public:
   enum CATEGORY_BUTTON_STATE
   {
      CBS_EMPTY,
      CBS_FULL_INACTIVE,
      CBS_FULL_ACTIVE,
      CBS_UNKNOWN
   };

   enum BARCODE_CAT_BTN_ACTIONS
   {
      BC_ACTION_UNDEFINED           = -1,
      BC_ACTION_DO_NOTHING          =  0,
      BC_ACTION_FULL_FACE           =  1,
      BC_ACTION_EMPTY_FACE          =  2,
      BC_ACTION_ADD_TEXT_WRITE_CART =  4,
      BC_ACTION_DELETE_TEXT         =  8,
      BC_ACTION_ERASE_CART          = 16
   };

private:

   struct BTN_STATES
   {
      CATEGORY_BUTTON_STATE tab_current_state;
      CATEGORY_BUTTON_STATE tab_requested_state;
      bool                  tab_categories_exist;
      CATEGORY_BUTTON_STATE tab_resultant_state;
      int                   tab_processing_section_id;
   };


   static BTN_STATES btnStates [];

   // This text field is a suplemental label (for NaCl, Misc num, etc).
   // The text field from the Button_Bitmap base class is used to
   // display the scan count.
   Display_Text _btnLblTxt;

   // The _category this button represents
   const SCAN_CATEGORY _category;

   // Pointer to the metadirectory that will let me get addressability to peer objects
   MetaDirectory* const _metaDirectoryPtr;

   // The display _rank in which this button is displayed
   const CATEGORY_RANK _rank;

   // Button state
   CATEGORY_BUTTON_STATE _catBtnState;

   BITMAP_ID
      _bmpIdUpEmpty,
      _bmpIdUpFull,
      _bmpIdDownEmpty,
      _bmpIdDownFull;

   // Bitmap ID's for the Vista feedback bitmaps
   Bitmap _bmpVistaFeedback;

   // The maximum number of scans that this _category can concurrently hold. If the operator
   // attempts to exceed this, he/she will receive an error and the scan will be tossed.
   const int _maxScansAllowed;

   // The _category sequence number of the last record displayed by this button.
   int
      _lastDisplayedSeqnum,
      _highWaterSequence;

   // A flag that permits/prohibits the operator to/from deleted an already scanned item.
   //  This will allow a _category to have "audit"-like qualities.
   bool                  _allowDeletions;

   static Audio_Feedback _feedbackSound;

   // Unimplemented methods - no null constructor; copies; assignments
   Category_Button ();
   Category_Button(Category_Button const&);
   Category_Button& operator = (Category_Button const&);

   TEXT_COLOR     _txtColorEmpty, _txtColorFull;

   TEXT_STRING_ID _lblId;

   friend DataLog_Stream& operator << (DataLog_Stream& os, BARCODE_CAT_BTN_ACTIONS action);
   friend DataLog_Stream& operator << (DataLog_Stream& os, const Category_Button::BTN_STATES& state);
   friend DataLog_Stream& operator << (DataLog_Stream& os, CATEGORY_BUTTON_STATE state);

public:


   // CONSTRUCTOR
   //
   Category_Button (MetaDirectory* wgt_directory,
                    SCAN_CATEGORY category_prm,
                    CATEGORY_RANK rank_prm,
                    GUI_BUTTON_ID btn_id,
                    short row,
                    short col,
                    BITMAP_ID btn_up_bitmap_name_empty,
                    BITMAP_ID btn_up_bitmap_name_full,
                    BITMAP_ID btn_dn_bitmap_name_empty,
                    BITMAP_ID btn_dn_bitmap_name_full,
                    int max_entries = UNLIMITED_SCANS,
                    bool allowDeletions = true,
                    TEXT_STRING_ID text_id = textEmpty,
                    TEXT_COLOR emptyTextColor = LAST_TEXT_COLOR,
                    TEXT_COLOR fullTextColor = LAST_TEXT_COLOR);


   // DESTRUCTOR
   virtual ~Category_Button ();


   // ALLOCATE_RESOURCES
   // This method will allocate a widget and attach it under the parent
   // (container) widget passed as a parameter. If the filename was specified
   // at constructor time, the bitmap will be allocated and attached to the
   // widget.
   virtual void allocate_resources (OSWindow& parent);


   // DEALLOCATE_RESOURCES
   // This method will delete the photon widget and any other manageable
   // resources.
   virtual void deallocate_resources ();


   // PROCESS_BUTTON_PRESS
   // This is called when the button is pressed. It will attempt to read whatever
   // is in the input cartouche:
   //    if the input cartouche is not empty,
   //       *it will store the scan value in the barcode_repository;
   //       *ask the Category_Button_Collection to reset any other CBS_FULL_ACTIVE buttons
   //         to CBS_FULL_INACTIVE;
   //       *set itself as CBS_FULL_ACTIVE; display its new content count on the button face.
   //
   //    if the input cartouche is empty and the _category button has stored scans,
   //       *it will determine what the next stored _category value is in the repository and
   //         send it to the _category cartouche for display.
   //       *it will ask the Category_Button_Collection to reset any other CBS_FULL_ACTIVE
   //         buttons to CBS_FULL_INACTIVE;
   //       *it will set itself as CBS_FULL_ACTIVE
   //
   //    if the input cartouche is empty and the _category button has no stored scans,
   //       * it will do nothing.
   virtual void process_button_press ();


   // SET_BUTTON_STATE
   // This will set the button to the state passed as a parameter. It will not assume
   // an illogical state, i.e. if the button _category has stored scans, it will not
   // assume the CBS_EMPTY state or if the button has no scans, it will not assume the
   // CBS_FULL_INACTIVE state. If it is asked to set the button to the CBS_FULL_ACTIVE
   // state, it will first ask the button collection to set the rest of the buttons
   // to the inactive state. The second parameter is only used by the method when a
   // request is made to set the state to CBS_FULL_ACTIVE. The second parameter will
   // be used as the sequence number of the stored scan value that is to be displayed
   // in the _category cartouche and the number to be displayed on the button face.
   virtual void set_button_state (CATEGORY_BUTTON_STATE button_state, int sequence_number);


   // GET_BUTTON_STATE
   // This will return the current state of the button
   virtual CATEGORY_BUTTON_STATE get_button_state () const {return _catBtnState; }


   // DELETE_ACTIVE_BARCODE
   // This will delete the stored scan that matches the _category ID and sequence number.
   virtual void delete_active_barcode (SCAN_CATEGORY _category, int category_sequence);

   // GET_CATEGORY_ID
   // This will return the _category that this button represents
   virtual SCAN_CATEGORY get_category_id () const { return _category; }


   // GET_RANK_NUMBER
   // This will return the _rank that this button belongs to.
   virtual CATEGORY_RANK get_rank_number () const { return _rank; }


   virtual BITMAP_ID get_vista_summary_status (int category_high_water) const;


   virtual void display_vista_status_symbol (BITMAP_ID bitmap);

protected:

   Message<VISTA_BARCODE_FEEDBACK> _barcode_msg;
   void vista_barcode_feedback_notify ();
};  // End of the Category_Button class

// End of single include define guardian
#endif

/* FORMAT HASH 05ff64a836da1e006be8f29614c55837 */
