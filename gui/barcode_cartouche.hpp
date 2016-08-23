/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2000 Gambro BCT Inc. All rights reserved

*     Author:       Terry Wahl

*     Class names:   Barcode_Cartouche, Barcode_Input_Cartouche, Barcode_Category_Cartouche

*     File name:    barcode_cartouche.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This file contains the header for the generic barcode cartouche
         class and the two cartouche classes derived from the generic class.

         Generic class: defines the cartouche bitmap drawing; the means of
           placing displayed scan data in the cartouche; retrieving scan
           data; concatenating scan data onto the end of the displayed data.

         Input class: handles the notification of a new scan; handles the
           validation of the scan.

         Category class: resets currently displayed barcode data.

*************************< FOCUSSED GUI DOMAIN >***************************/


// Single include define guardian
#ifndef BARCODE_CARTOUCHE
#define BARCODE_CARTOUCHE


#define  SUCCESS_CART 1
#define  FAILURE_CART 0
#define  ALREADY_CONCATENATED -1
#define  TOO_LONG -2

#define  CARTOUCHE_DISPLAY_WIDTH 400

#define  TINY_FONT_SIZE 10
#define  NORM_FONT_SIZE 12


// composed classes includes
//
#include "bitmap.h"
#include "display_text.h"
#include "scan_structure.h"
#include "trimamessages.h"
// #include "metadirectory.hpp"
// #include "category_button_collection.hpp"


class MetaDirectory;


/// CLASS /////////////////////////////////////////////////////////////////////////////////////

class Barcode_Cartouche
{

private:

   // Unimplemented methods - no null constructor; copies; assignments
   // Barcode_Cartouche ();
   Barcode_Cartouche(Barcode_Cartouche const&);
   Barcode_Cartouche& operator = (Barcode_Cartouche const&);



protected:
   // Pointer to the metadirectory that will let me get addressability to peer objects
   MetaDirectory* widget_directory;

   char           scan_hold[MAX_SCAN_LENGTH + 1];
   int            scan_length;
   char           scan_symbology[MAX_SYMB_LENGTH];

   Display_Text   text_scan_buffer;  // text widget containing the displayed scan text

   Bitmap         bitmap_vista_feedback;  // the bitmap image of the vista response

   // CONCATENATE_AND_DISPLAY_BARCODE
   // This will concatenate the string passed to it to the currently displayed
   // scan text. It will also check if the argument passed is already concatenated
   // in the display string. If it is, it will return a ALREADY_CONCATENATED return
   // code. Otherwise it will return a SUCCESS_CART return code. As this method
   // concatenates the argument, it will delimit it with the delimiter defined in
   // the scan_structure.h file.
   virtual int concatenate_and_display_barcode (char* scan_data, int scan_data_length);


public:
   // CONSTRUCTOR
   // The constructor will take bitmap file name and initial X,Y coordinates
   // in anticipation of allocation.
   Barcode_Cartouche (MetaDirectory* wgt_directory,
                      int cons_x_coord,
                      int cons_y_coord,
                      int text_x_coord,
                      int text_y_coord,
                      TEXT_STRING_ID strId);


   // DESTRUCTOR
   virtual ~Barcode_Cartouche ();


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


   // CLEAR_DISPLAY
   // This will remove any text data from the cartouche display and reset the
   // ghost text buffer to a null string.
   virtual void clear_display ();


   // RETURN_DISPLAY_DATA_AND_CLEAR
   // This will return the displayed scan data found in the cartouche and then
   // clear the scan data from the cartouche.
   virtual void return_display_data_and_clear (SCAN_STRUCTURE* scan_info);


   // RETURN_DISPLAY_DATA
   // This will return the displayed scan data found in the cartouche.
   virtual void return_display_data (SCAN_STRUCTURE* scan_info);


   // DISPLAY_BARCODE
   // This method will replace whatever barcode scan is being displayed with the passed argument.
   virtual void display_barcode (char* scan_data, int scan_data_length);





}; // End of the barcode_cartouche class



/// CLASS /////////////////////////////////////////////////////////////////////////////////////
class Barcode_Input_Cartouche
   : public Barcode_Cartouche
{
private:
   // Unimplemented methods - no null constructor; copies; assignments
   // Barcode_Input_Cartouche ();
   Barcode_Input_Cartouche(Barcode_Input_Cartouche const&);
   Barcode_Input_Cartouche& operator = (Barcode_Input_Cartouche const&);

   BarcodeScannedMsg* scan_notify;  // ptr to the notify message for a new scan.

public:
   // BARCODE_NOTIFICATION_HANDLER
   // This method will register for the event message sent by the repository that
   // indicates a new barcode scan was received and placed in the FIFO. This method
   // will then drain the FIFO and concatenate the new scans into the cartouche. If
   // one of the new scans is a duplicate of one in the cartouche buffer, it will
   // display a warning in the status line. Message_data is currently unused.
   virtual void barcode_notification_handler ();


public:
   // CONSTRUCTOR
   // The constructor will take bitmap file name and initial X,Y coordinates
   // in anticipation of allocation.
   Barcode_Input_Cartouche (MetaDirectory* wgt_directory,
                            int cons_x_coord = 0,
                            int cons_y_coord = 0);


   // DESTRUCTOR
   virtual ~Barcode_Input_Cartouche ();


   // ALLOCATE_RESOURCES
   // This method will allocate a widget and attach it under the parent
   // (container) widget passed as a parameter. This routine will also allocate
   // the incoming barcode scan alert message.
   virtual void allocate_resources (OSWindow& parent);


   // DEALLOCATE_RESOURCES
   // This method will delete the photon widget and any other manageable
   // resources. This method will also deallocate the incoming barcode scan alert
   // message.
   virtual void deallocate_resources ();


   // CLEAR_DISPLAY
   // This will use the base class clear but also reset the status line
   virtual void clear_display ();
};  // End of the Barcode_Input_Cartouche class



/// CLASS /////////////////////////////////////////////////////////////////////////////////////
class Barcode_Category_Cartouche
   : public Barcode_Cartouche
{
private:

   // Unimplemented methods - no null constructor; copies; assignments
   // Barcode_Input_Cartouche ();
   Barcode_Category_Cartouche(Barcode_Category_Cartouche const&);
   Barcode_Category_Cartouche& operator = (Barcode_Category_Cartouche const&);


   SCAN_CATEGORY currently_displayed_category;
   int           currently_displayed_category_sequence;

public:

   // CONSTRUCTOR
   // The constructor will take bitmap file name and initial X,Y coordinates
   // in anticipation of allocation.
   Barcode_Category_Cartouche (MetaDirectory* wgt_directory,
                               int cons_x_coord = 0,
                               int cons_y_coord = 0);


   // DESTRUCTOR
   virtual ~Barcode_Category_Cartouche ();


   // RESET_CURRENTLY_DISPLAYED_BARCODE
   // This method will communicate with the category button collection to
   // delete the barcode scan that is currently displayed in the cartouche.
   virtual void reset_currently_displayed_barcode ();


   // DISPLAY_BARCODE_CATEGORY
   // This will will display a stored barcode scan category and store the ID of
   // the category as well.
   virtual int display_barcode_category (char* scan_data,
                                         int scan_data_length,
                                         SCAN_CATEGORY current_category,
                                         int current_category_sequence);

   // CLEAR_DISPLAY
   // This will set the category and sequence values to a null value and
   // then call the parent classes clear method.
   virtual void clear_display ();

};  // End of the Barcode_Category_Cartouche class


// End of single include define guardian
#endif

/* FORMAT HASH 33d7cc14938231d54751caa38fa8ecca */
