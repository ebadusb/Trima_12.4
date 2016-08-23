/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2000 Gambro BCT Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Barcode_Repository

*     File name:    barcode_repository.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This contains the class declaration for the GUI barcode repository.
         The repository will manage incoming barcode scans placing them into
         a FIFO; it will invoke the barcode GUI screen as an operator info alarm
         if the screen is not already invoked; it will store all the categorized
         barcodes into a PFR file and use this file as the category store when
         the operator wishes to review or delete barcode scans.

*************************< FOCUSSED GUI DOMAIN >***************************/


// Single include define guardian
#ifndef BARCODE_REPOSITORY
#define BARCODE_REPOSITORY


#define  SUCCESS_REPO 1
#define  FAILURE_REPO -1


// composed classes includes
//
#include "gui_fifo.hpp"
#include "scan_structure.h"
#include "trimamessages.h"

// other Trima classes
#include "an_alarm.h"
#include "pfrmsg.h"


#define  CAT_REC_QUAN  200
#define  FIFO_FREE_POOL_SIZE 10
#define  HISTORICAL_HIGH_SEQ  -1066

enum INIT_MODE
{
   INIT_MODE_NORMAL,
   INIT_MODE_VERIFY
};


// redefine the data record with the historical high water mark
typedef union HHWM_UNION
{
   int  historical_high_water;
   char scan_buffer_dopple [MAX_SCAN_LENGTH];
};

class Random_file_storage;

class Barcode_Repository
{
private:
   // Unimplemented methods - no null constructor; copies; assignments
   // Barcode_Repository ();
   Barcode_Repository(Barcode_Repository const&);
   Barcode_Repository& operator = (Barcode_Repository const&);

   // Repository is initialize flag
   bool is_initialized;

   // Pointer to the FIFO
   GUI_FIFO<SCAN_STRUCTURE>* repo_fifo;

   // Pointer to the random file storage of categorized barcode scans
   Random_file_storage* category_storage;

   // Key building structure for accessing random file storage
   CAT_SEQ_KEY RFS_key;

   // Blank scan structure for a "deleted" placeholder
   SCAN_STRUCTURE blank_scan;


   // LOG_BARCODE
   // This method writes out a log record to the Everest log, Trima Data log and sends out a
   // router message for Vista to be apprised of the barcode action.
   void log_barcode (const SCAN_STRUCTURE* callers_scan_buffer);


public:
   // CONSTRUCTOR
   //
   Barcode_Repository ();


   // DESTRUCTOR
   //
   virtual ~Barcode_Repository ();


   // INITIALIZE_REPOSITORY
   // This method will open the Random File used to store categorized barcode scans. It will
   // initialize the FIFO for in-coming barcode scans (from the Sentinel). It will create the
   // message objects needed to communicate with the Sentinel. It will spawn the barcode
   // sentinel. If anything fails to properly initialize, this routine will return a FAILURE_REPO,
   // otherwise, it will return a SUCCESS_REPO.
   // Parameters: The first parameter determines whether the initialization is to just verify
   //             the category disk file or to actually set up the repository -- either call must be
   //             shutdown with the "shut_down_repository" call.
   bool initialize_repository (INIT_MODE init_type);


   // GET_NEXT_BARCODE_SCAN_FROM_FIFO
   // This method will dequeue the next barcode scan from the FIFO and return it to the caller.
   // If there are no more items in the FIFO, it will return a FAILURE_REPO return code. Otherwise,
   // it will return a SUCCESS_REPO.
   bool get_next_barcode_scan_from_fifo (SCAN_STRUCTURE* callers_scan_buffer);


   // CLEAR_FIFO
   // This method will clear the FIFO. This would be used when a series of scans
   // contains a "bad" scan and the repository client wants to purge the whole
   // waiting series.
   void clear_fifo ();


   // READ_CATEGORY_ITEM
   // This method will read an item in the category specified in the category and the
   // sequence number. If the category/sequence is not found, an error will be returned.
   int read_category_item (SCAN_CATEGORY category, int sequence_number, SCAN_STRUCTURE* callers_scan_buffer);


   // DELETE_CATEGORY_ITEM
   // This method will delete the category item specified by the category and sequence parameter.
   // If the specified item is not found, it will return failure, otherwise success.
   int delete_category_item (SCAN_CATEGORY category, int sequence_number, int log_item = 1);


   // WRITE_CATEGORY_ITEM
   // This method will write out the category scan using the category and the sequence numbers
   // as the "key" with which to subsequently retrieve the item.
   int write_category_item (SCAN_CATEGORY category, int sequence_number, const SCAN_STRUCTURE* callers_scan_buffer, int log_item = 1);


   // UPDATE_CATEGORY_ITEM
   // This method will update the category scan using the category and the sequence numbers
   // as the "key" with which to subsequently update the item.
   int update_category_item (SCAN_CATEGORY category, int sequence_number, const SCAN_STRUCTURE* callers_scan_buffer);


   // RETURN_CATEGORY_COUNT
   // This method is passed a category type and it will search the entire random storage
   // allocation table counting the total number of records in this category and keep track
   // of the highest sequence number for this category.
   void return_category_count (SCAN_CATEGORY category, int* total, int* high_water_mark);


   // RETURN_HISTORICAL_HIGH_WATERMARK
   // This will return the highest sequence number used by a category since the repository
   // has been cleaned. This number may be different from the high water mark returned in the
   // return_category_count method above in that all the records for a category may be
   // deleted and yet this call will return the highest sequence number ever used since the
   // last "clean". In contrast the above mentioned method will return the highest used sequence
   // number "still" in existence. If an invalid category is passed or no record has ever been
   // allocated for this category, this method will return a -1. This method requires access to the
   // disk, so this should be a performance consideration.
   int return_historical_high_watermark (SCAN_CATEGORY category);


   // TOTAL_SCANS
   // This method will return the number of scans stored in the category storage
   int total_scans ();

   // CLEAN_CATEGORY_STORAGE
   // This method will delete ALL the items in the category storage. This will most
   // commonly be called when it is determined that the machine has been rebooted and
   // we are NOT going to perform a powerfail recovery.
   void clean_category_storage ();


   // SHUT_DOWN_REPOSITORY
   // This method closes down the functions of the repository.
   void shut_down_repository ();


   // INVOKE_BARCODE_SCREEN
   // This method actually raises the operator info alarm for the barcode
   void invoke_barcode_screen ();

protected:
   Message<BARCODE_STRUCT> sentinel_msg;
   Message<PFR_Status>     pfr_msg;


   // MANAGE_SENTINEL_MESSAGE
   // This method will receive the barcode scan from the Sentinel. It will place the message in the
   // scan FIFO and then check to see if the GUI barcode screen is currently displayed. If it is
   // this routine will send an event message out notifying the screen that a new FIFO element is
   // waiting. If the screen is not displayed or in the process of being displayed, it will notify
   // Proc to raise the "operator info" alarm for that screen.
   void manage_sentinel_message ();

   void pfr_message_received ();

}; // End of the Barcode_Repository class


// End of single include define guardian
#endif

/* FORMAT HASH 024c2b637a421047d7dfea57bdf7c9dd */
