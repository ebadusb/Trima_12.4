/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2000 Gambro BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Barcode_Repository

*     File name:   barcode_Repository.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Log: barcode_repository.cpp $
Revision 1.23  2009/03/25 15:51:03Z  dslausb
IT 8718 - Get rid of spurious barcode alarm during shutdown.
Revision 1.22  2008/10/17 17:19:27Z  dslausb
IT 8593 - Fix barcode screen string allocation
Revision 1.21  2007/01/17 22:52:08Z  rm70006
IT 7459.  Change log level for barcode error messages.
Revision 1.20  2006/09/20 20:51:44Z  rm70006
IT 7722.  Move VistaBinaryRecord writing to the Vista Interface task.
Revision 1.19  2006/08/17 20:42:22Z  dslausb
- Machine screen no longer has barcode button.  Scanned barcodes automagically go into the barcode field when you're on that screen.
- Confirm single deletion box show the REF number you're confirming deletion for
Revision 1.18  2006/08/09 16:51:47Z  dslausb
IT 7682 - Changed to 2-file cassette entry functionality
Revision 1.17  2004/01/20 22:52:04Z  rm70006
IT 6790.  Fixed bad code where barcode string wasn't being null terminated.
Revision 1.16  2003/12/29 15:42:54Z  rm70006
IT 5858.  Move type to common location.  Add time to barcode record.
Revision 1.15  2003/11/26 17:11:52Z  rm70006
IT 6054.  Improve logging.
Revision 1.14  2003/08/14 15:13:36Z  rm70006
IT 6362.  Add barcode PFR restore functionality.
Revision 1.13  2003/05/12 22:25:56Z  rm70006
IT 5841.

Add badge barcode logic for CFR11.
Merge barcode_message.h types into scan_structure.h
Revision 1.12  2003/05/12 17:12:16Z  rm70006
IT 5815.

Finish porting barcode stuff.
Revision 1.11  2003/05/02 18:19:03Z  rm70006
IT 5815.

Port barcode reader.
Revision 1.10  2003/04/23 20:05:44Z  rm70006
IT 5829.

Fix PFR logic.
Revision 1.9  2003/04/23 16:37:53Z  rm70006
IT 5841.

Clean up file.  Replace constants with bools.
Revision 1.8  2003/04/23 16:07:53Z  jl11312
- added binary logging of barcode information
Revision 1.7  2003/04/04 17:55:34Z  rm10919
Datalog changes. IT 5818
Revision 1.6  2002/09/06 21:53:23Z  sb07663
fixed stat return to be vx compatible
Revision 1.5  2002/08/23 16:57:31  sb07663
fix glitch for conflicting Free() declarations
Revision 1.4  2002/08/15 22:14:18  sb07663
Compiling Vx version
Revision 1.3  2001/12/07 21:17:40  rm70006
IT 5151.
Add update category item methods to class.  Needed for Vista
Barcode Feedback.
Revision 1.2  2001/05/03 13:24:38  rm70006
Added barcode code from V4.2.  IT 5083.
Revision 1.1  2000/11/06 19:30:56  tw09453
Initial revision
*/

#include <vxworks.h>

// CLASS INCLUDE
#include "guiglobs.hpp"
#include "barcode_repository.hpp"
#include "filenames.h"
#include "pfrmsg.h"
#include "ScreenKeyboard.h"
#include "random_file_storage.hpp"

#include <sys/stat.h>

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Barcode_Repository::Barcode_Repository()
   : sentinel_msg     (),
     pfr_msg          (),
     repo_fifo        (0),
     category_storage (0),
     is_initialized   (false)
{}  // End of Barcode_Repository constructor




////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Barcode_Repository::~Barcode_Repository()
{
   // Delete the variously allocated objects
   if (category_storage != NULL)
   {
      delete category_storage;
   }

   if (repo_fifo != NULL)
   {
      delete repo_fifo;
   }
}



////////////////////////////////////////////////////////////////////////////////
// INITIALIZE_REPOSITORY
bool Barcode_Repository::initialize_repository (INIT_MODE init_type)
{
   // Don't initialize an already initialized repository
   if (is_initialized)
   {
      return false;
   }

   // First determine if the category PFR file exists
   struct stat pstat;

   if (stat(CATEGORY_FILE_NAME,  &pstat) != OK)
   {
      // File doesn't exist, so open a new one
      category_storage = new Random_file_storage(CATEGORY_FILE_NAME,
                                                 CAT_REC_QUAN,
                                                 sizeof(SCAN_STRUCTURE),
                                                 sizeof(CAT_SEQ_KEY));
   }
   else
   {
      // Else open the existing one
      category_storage = new Random_file_storage(CATEGORY_FILE_NAME);
   } // End of else

   // Open the file storage
   if (category_storage->open_file() != 0)
   {
      DataLog(log_level_gui_barcode_error) << "BARCODE REPOSITORY: failed to open the random file storage" << endmsg;

      anAlarmMsg alarm(BARCODE_INITIALIZATION_ALARM);
      alarm.setAlarm("BARCODE REPOSITORY: failed to open the random file storage");

      return FAILURE_REPO;
   }

   // Create the pfr message that informs us to reset the category_storage
   pfr_msg.init(Callback<Barcode_Repository>(this, &Barcode_Repository::pfr_message_received), MessageBase::SNDRCV_RECEIVE_ONLY);

   // Create the scan message FIFO and pre-allocate some free pool elements
   repo_fifo = new GUI_FIFO<SCAN_STRUCTURE>;
   repo_fifo->preallocate_free_pool (FIFO_FREE_POOL_SIZE);

   // Create the buffer message to receive the sentinel scan messages
   sentinel_msg.init(Callback<Barcode_Repository>(this, &Barcode_Repository::manage_sentinel_message), MessageBase::SNDRCV_RECEIVE_ONLY);

   // Now spawn the barcode sentinel process if this is a normal initialization
   if (init_type == INIT_MODE_NORMAL)
   {
      // V5 spawned the barcode reader task here
   }

   if (init_type == INIT_MODE_VERIFY)
   {
      // Read all the allocated records -- this act will check the CRC of the header, allocation
      // table and all the allocated records. If any I/O errors occur, this will return a
      // failure indication.

      RECORD_NUMBER_TYPE read_rc;

      CAT_SEQ_KEY        buff_key;

      SCAN_STRUCTURE     buff_rec;

      // Ensure we're at the beginning of the file.
      category_storage->rewind_cursor();

      do
      {
         read_rc = category_storage->read_next ((REC_TYPE*)&buff_rec, (KEY_TYPE*)&buff_key);

      } while (read_rc != EOF && read_rc != IO_ERR);

      if (read_rc == IO_ERR)
      {
         return false;
      }
   }

   // Successful return
   is_initialized = true; // note that we're initialized

   return true;
}



////////////////////////////////////////////////////////////////////////////////
// MANAGE_SENTINEL_MESSAGE
void Barcode_Repository::manage_sentinel_message ()
{
   const BARCODE_STRUCT& new_barcode = sentinel_msg.getData();
   SCAN_STRUCTURE        local_scan_buff;

   if (!is_initialized)
   {
      DataLog(log_level_gui_barcode_error) << "BARCODE REPOSITORY: Method call to uninitialized repository" << endmsg;
      return;
   }

   local_scan_buff.scan_category = BC_CAT_NO_ASSIGNMENT;

   // Check for errors and copy the data over to a SCAN_STRUCTURE
   if (new_barcode.barcode_read_error)
   {
      // zero the entire buffer
      memset (&local_scan_buff, '\0', sizeof(SCAN_STRUCTURE));

      // Mark this as a bad scan.
      local_scan_buff.status = BC_STATUS_BAD_SCAN;
   }
   else
   {
      // Mark status as good scan
      local_scan_buff.status = BC_STATUS_GOOD_SCAN;

      // Move over the symbology code
      memmove (local_scan_buff.scan_symbology_char, (void const*)&new_barcode.symbology_code, MAX_SYMB_LENGTH);

      // Calculate the minimum size of the barcode scan data
      const int max_barcode_data = (new_barcode.barcode_len < MAX_SCAN_LENGTH) ? new_barcode.barcode_len : MAX_SCAN_LENGTH - 1;

      local_scan_buff.scan_strlen = max_barcode_data;

      memmove (local_scan_buff.scan_string, new_barcode.barcode_data, max_barcode_data);

      local_scan_buff.scan_string[local_scan_buff.scan_strlen] = '\0';  // null terminate the string

      time(&local_scan_buff.scan_time);
   }

   // If the CFR 11 screen is already up, then this barcode is the Operator Badge.
   // If the machine config screen is up, then this barcode is a REF number.
   if ( guiglobs::screen_status.get_current_displayed_screen() == GUI_SCREEN_CONFIG_MACHINE ||
        guiglobs::screen_status.get_current_displayed_screen() == GUI_SCREEN_CFR11          ||
        ScreenKeyboard::IsShown())
   {
      BADGE_BARCODE_MSG          badgeBarcode;
      Message<BADGE_BARCODE_MSG> msg(MessageBase::SEND_LOCAL);

      memcpy(badgeBarcode.badgeString, local_scan_buff.scan_string, local_scan_buff.scan_strlen);
      badgeBarcode.badgeLength = local_scan_buff.scan_strlen;

      // Send the message
      msg.send(badgeBarcode);
   }
   // If the PAS Barcode Confirmation screen is up, assume it's a PAS bag barcode (IT-13316)
   else if (guiglobs::screen_status.get_current_displayed_screen() == GUI_SCREEN_PAS_BARCODE_CONFIRM_BOX)
   {
      PAS_BAG_BARCODE_MSG          pasBagBarcode;
      Message<PAS_BAG_BARCODE_MSG> msg(MessageBase::SEND_LOCAL);

      memcpy(pasBagBarcode.pasBagString, local_scan_buff.scan_string, local_scan_buff.scan_strlen);
      pasBagBarcode.pasBagLength = local_scan_buff.scan_strlen;

      // Send the message
      msg.send(pasBagBarcode);
   }
   else
   {
      // Put this scan on the FIFO
      repo_fifo->add_data_to_FIFO (&local_scan_buff);

      // Notify the Alarm Handler to bring up the barcode screen (if not already).
      invoke_barcode_screen();

      // Send the scan event message in case the alarm screen is already displayed
      //  this will tell it to read the next item in the FIFO.
      BarcodeScannedMsg scanned(MessageBase::SEND_LOCAL);
      scanned.send(0);
   }
}



////////////////////////////////////////////////////////////////////////////////
// GET_NEXT_BARCODE_SCAN_FROM_FIFO
bool Barcode_Repository::get_next_barcode_scan_from_fifo (SCAN_STRUCTURE* callers_scan_buffer)
{
   if (repo_fifo == NULL)
   {
      DataLog(log_level_gui_barcode_error) << "BARCODE REPOSITORY: FIFO call made before initialization." << endmsg;

      anAlarmMsg alarm(BARCODE_INITIALIZATION_ALARM);
      alarm.setAlarm("BARCODE REPOSITORY: FIFO call made before initialization.");

      return false;
   }

   if (repo_fifo->retrieve_data_from_FIFO(callers_scan_buffer) == FIFO_SUCCESS)
   {
      return true;
   }
   else
   {
      return false;
   }
}  // End of Barcode_Repository get_next_barcode_scan_from_fifo




////////////////////////////////////////////////////////////////////////////////
// READ_CATEGORY_ITEM
int Barcode_Repository::read_category_item (SCAN_CATEGORY category,
                                            int sequence_number,
                                            SCAN_STRUCTURE* callers_scan_buffer)
{

   RECORD_NUMBER_TYPE read_record_number;

   if (category_storage == NULL)
   {
      DataLog(log_level_gui_barcode_error) << "BARCODE REPOSITORY: Category storage call (read) made before initialization." << endmsg;

      anAlarmMsg alarm(BARCODE_INITIALIZATION_ALARM);
      alarm.setAlarm("BARCODE REPOSITORY: Category storage call (read) made before initialization.");

      return FAILURE_REPO;
   }

   // Build aggregate key
   RFS_key.category        = category;
   RFS_key.sequence_number = sequence_number;

   // Start looking for this from the beginning of the file.
   category_storage->rewind_cursor();

   // Read in that record
   read_record_number = category_storage->read_next_key ((REC_TYPE*)callers_scan_buffer,
                                                         (KEY_TYPE*)&RFS_key);

   // If we have a malformed record (PFR record) we down the machine
   if (read_record_number == IO_ERR)
   {
      DataLog(log_level_gui_barcode_error) << "BARCODE REPOSITORY: Category record read caused IO_ERR" << endmsg;

      anAlarmMsg alarm(BARCODE_INITIALIZATION_ALARM);
      alarm.setAlarm("BARCODE REPOSITORY: Category record read caused IO_ERR");

      return FAILURE_REPO;
   }

   // If we couldn't find the record asked for, we indicate failure
   if (read_record_number == EOF)
   {
      return FAILURE_REPO;
   }
   else
   {
      return SUCCESS_REPO;
   }
}



////////////////////////////////////////////////////////////////////////////////
// DELETE_CATEGORY_ITEM
int Barcode_Repository::delete_category_item (SCAN_CATEGORY category, int sequence_number, int log_item)
{
   RECORD_NUMBER_TYPE read_record_number;
   SCAN_STRUCTURE     deleted_record;

   if (category_storage == NULL)
   {
      DataLog(log_level_gui_barcode_error) << "BARCODE REPOSITORY: Category storage call (delete) made before initialization." << endmsg;

      anAlarmMsg alarm(BARCODE_INITIALIZATION_ALARM);
      alarm.setAlarm("BARCODE REPOSITORY: Category storage call (delete) made before initialization.");

      return FAILURE_REPO;
   }

   // Build aggregate key
   RFS_key.category        = category;
   RFS_key.sequence_number = sequence_number;

   // Start looking for this from the beginning of the file.
   category_storage->rewind_cursor();

   // Check for existence of that record
   read_record_number = category_storage->key_exists ((KEY_TYPE*)&RFS_key);

   // If we have a malformed file (PFR record) we down the machine
   if (read_record_number == IO_ERR)
   {
      DataLog(log_level_gui_barcode_error) << "BARCODE REPOSITORY: Category record delete caused IO_ERR" << endmsg;

      anAlarmMsg alarm(BARCODE_INITIALIZATION_ALARM);
      alarm.setAlarm("BARCODE REPOSITORY: Category record delete caused IO_ERR");

      return FAILURE_REPO;
   }

   // If we couldn't find the record asked for, we indicate failure
   if (read_record_number == EOF)
   {
      return FAILURE_REPO;
   }
   else
   {
      // Start looking for this from the beginning of the file.
      category_storage->rewind_cursor();

      // read it in for logging purposes
      read_record_number           = category_storage->read_next_key ((REC_TYPE*)&deleted_record,
                                                                      (KEY_TYPE*)&RFS_key);
      deleted_record.status        = BC_STATUS_DELETED;
      deleted_record.scan_category = category;
      deleted_record.scan_sequence = sequence_number;

      // delete that record via it's returned record number
      if (!category_storage->delete_record (read_record_number))
      {
         DataLog(log_level_gui_barcode_error) << "BARCODE REPOSITORY: Category record delete inconsistency" << endmsg;

         anAlarmMsg alarm(BARCODE_INITIALIZATION_ALARM);
         alarm.setAlarm("BARCODE REPOSITORY: Category record delete inconsistency");

         return FAILURE_REPO;
      }
      else
      {
         ///////////////////////////////// highwater mark processing //////////////////////////

         // If this is a recursion of the highwater mark processing (to delete the current hhwm record)
         // then we can return -- we'll let the calling instance perform the commit and also we'll
         // sidestep "logging" the historical high water record's deletion.
         if (sequence_number == HISTORICAL_HIGH_SEQ)
         {
            return SUCCESS_REPO;
         }

         // See if we have to update or create a historical highwater mark record.
         // First get the highest "existing" key
         int total;
         int high_water_mark;
         return_category_count (category, &total, &high_water_mark);

         if (high_water_mark >= sequence_number)
         {
            // This deletion didn't affect the overall highwater mark and we have an allocated
            //  record whose sequence is higher than this sequence number... ergo, we don't need
            //  to investigate or create the high water mark record.
         }
         else
         {
            // Let's read in the highwater mark record (if it exists) and decide if we have a
            // new highwater mark sequence number we need to record.
            int highest_known = return_historical_high_watermark (category);

            if (sequence_number > highest_known) // if true, then we need to create/update the hhwm record.
            {
               // Delete the existing hhwm record -- if the delete fails, we don't care... it may not exist yet
               delete_category_item (category, HISTORICAL_HIGH_SEQ);

               // Structure the hhwm value into the structure
               SCAN_STRUCTURE hist_high_rec;
               HHWM_UNION     hhwm_buff;

               // clean the union buffer
               memset (hhwm_buff.scan_buffer_dopple, ' ', MAX_SCAN_LENGTH);

               hhwm_buff.historical_high_water = sequence_number;

               // Move the union into the scan structure
               hist_high_rec.status                 = BC_STATUS_NA;
               hist_high_rec.scan_strlen            = MAX_SCAN_LENGTH;
               hist_high_rec.scan_symbology_char[0] = '\0';
               memmove (hist_high_rec.scan_string, hhwm_buff.scan_buffer_dopple, MAX_SCAN_LENGTH);

               // Write out a new highest
               write_category_item (category, HISTORICAL_HIGH_SEQ, &hist_high_rec);
            }

         } // End of "ELSE": if (high_water_mark >= sequence_number)

         /////////////////////////////////End of highwater mark processing //////////////////////////

         // commit delete and return
         category_storage -> commit_update ();

         if (log_item)
            log_barcode(&deleted_record);

         return SUCCESS_REPO;

      } // End of "ELSE": if (category_storage -> delete_record (read_record_number) != SUCCESS)

   } // End of: if (read_record_number == EOF)
}    // End of Barcode_Repository delete_category_item




////////////////////////////////////////////////////////////////////////////////
// WRITE_CATEGORY_ITEM
int Barcode_Repository::write_category_item (SCAN_CATEGORY category,
                                             int sequence_number,
                                             const SCAN_STRUCTURE* callers_scan_buffer,
                                             int log_item)
{
   RECORD_NUMBER_TYPE write_record_number;
   SCAN_STRUCTURE     written_record;

   if (category_storage == NULL)
   {
      DataLog(log_level_gui_barcode_error) << "BARCODE REPOSITORY: Category storage call (write) made before initialization." << endmsg;

      anAlarmMsg alarm(BARCODE_INITIALIZATION_ALARM);
      alarm.setAlarm("BARCODE REPOSITORY: Category storage call (write) made before initialization.");

      return FAILURE_REPO;
   }

   // Build aggregate key
   RFS_key.category        = category;
   RFS_key.sequence_number = sequence_number;

   // copy record to local buffer
   memmove (&written_record, callers_scan_buffer, sizeof(SCAN_STRUCTURE));
   written_record.scan_category = category;
   written_record.scan_sequence = sequence_number;

   // Write out the record
   write_record_number = category_storage -> write_direct ((unsigned char*)&written_record,
                                                           (unsigned char*)&RFS_key);

   // If we have a malformed write, bring it down
   if (write_record_number == IO_ERR)
   {
      DataLog(log_level_gui_barcode_error) << "BARCODE REPOSITORY: Category record write caused IO_ERR" << endmsg;

      anAlarmMsg alarm(BARCODE_INITIALIZATION_ALARM);
      alarm.setAlarm("BARCODE REPOSITORY: Category record write caused IO_ERR");

      return FAILURE_REPO;
   }
   else
   {
      // commit delete, log, and return
      category_storage->commit_update();

      // If this IS NOT a "historical high" write, then log it.
      if (sequence_number != HISTORICAL_HIGH_SEQ)
      {
         // log the barcode to the various logs, et al.
         written_record.status = BC_STATUS_ADDED;

         if (log_item)
         {
            log_barcode(&written_record);
         }
      }

      return SUCCESS_REPO;
   }
}



//
//
//
int Barcode_Repository::update_category_item (SCAN_CATEGORY category, int sequence_number, const SCAN_STRUCTURE* callers_scan_buffer)
{
   int status;

   status = delete_category_item (category, sequence_number, 0);

   if (status != SUCCESS_REPO)
   {
      DataLog(log_level_gui_barcode_error) << "BARCODE REPOSITORY: item could not be deleted." << endmsg;

      anAlarmMsg alarm(BARCODE_INITIALIZATION_ALARM);
      alarm.setAlarm("BARCODE REPOSITORY: item could not be deleted.");

      return FAILURE_REPO;
   }

   status = write_category_item (category, sequence_number, callers_scan_buffer, 0);

   if (status != SUCCESS_REPO)
   {
      DataLog(log_level_gui_barcode_error) << "BARCODE REPOSITORY: item could not be written." << endmsg;

      anAlarmMsg alarm(BARCODE_INITIALIZATION_ALARM);
      alarm.setAlarm("BARCODE REPOSITORY: item could not be written.");

      return FAILURE_REPO;
   }

   return SUCCESS_REPO;
}



void Barcode_Repository::pfr_message_received ()
{
   const PFR_Status msg = (PFR_Status)pfr_msg.getData();

   DataLog(log_level_gui_info) << "PFR Status is " << msg << endmsg;

   // If no PFR is to occur, then erase the old barcode data.
   if (msg == NoRestore)
      clean_category_storage();
}



////////////////////////////////////////////////////////////////////////////////
// CLEAN_CATEGORY_STORAGE
void Barcode_Repository::clean_category_storage ()
{
   if (category_storage == NULL)
   {
      DataLog(log_level_gui_barcode_error) << "BARCODE REPOSITORY: Category storage call (clean all) made before initialization." << endmsg;

      // If we're anywhere except POST_RUN on the 2nd run summary screen,
      // this is alarm-worthy.
      if (guiglobs::screen_status.get_current_system_state() != POST_RUN ||
          guiglobs::screen_status.get_current_screen() != GUI_SCREEN_RUNSUMM2)
      {
         anAlarmMsg alarm(BARCODE_INITIALIZATION_ALARM);
         alarm.setAlarm("BARCODE REPOSITORY: Category storage call (clean all) made before initialization.");
      }

      return;
   }

   category_storage->free_all_records();

   // commit free-all and return
   category_storage->commit_update();

}  // End of Barcode_Repository clean_category_storage



////////////////////////////////////////////////////////////////////////////////
// RETURN_CATEGORY_COUNT
void Barcode_Repository::return_category_count (SCAN_CATEGORY category, int* total, int* high_water_mark)
{
   int                total_category_entries = 0;
   int                high_water_measure     = 0;

   RECORD_NUMBER_TYPE category_record_number = 0;


   if (category_storage == NULL)
   {
      DataLog(log_level_gui_barcode_error) << "BARCODE REPOSITORY: Category storage call (highest category) made before initialization." << endmsg;

      anAlarmMsg alarm(BARCODE_INITIALIZATION_ALARM);
      alarm.setAlarm("BARCODE REPOSITORY: Category storage call (highest category) made before initialization.");
      return;
   }


   // Start looking for this from the beginning of the file.
   category_storage->rewind_cursor();

   while (category_record_number != EOF)
   {
      category_record_number = category_storage->read_key ((KEY_TYPE*)&RFS_key);

      // If we didn't run into the end of file (keywise) and the key we read is the same as
      // that passed as the parameter and we're not reading the "historical high" overhead record, count it.

      if ((category_record_number != EOF) &&
          (RFS_key.category == category) &&
          (RFS_key.sequence_number != HISTORICAL_HIGH_SEQ))
      {
         ++total_category_entries;

         if (RFS_key.sequence_number > high_water_measure)
         {
            high_water_measure = RFS_key.sequence_number;
         }
      }
   }

   *total           = total_category_entries;

   *high_water_mark = high_water_measure;

}



////////////////////////////////////////////////////////////////////////////////
// TOTAL_SCANS
int Barcode_Repository::total_scans ()
{
   int                total_category_entries = 0;
   RECORD_NUMBER_TYPE category_record_number = 0;


   if (category_storage == NULL)
   {
      DataLog(log_level_gui_barcode_error) << "BARCODE REPOSITORY: Category storage call (highest category) made before initialization." << endmsg;

      anAlarmMsg alarm(BARCODE_INITIALIZATION_ALARM);
      alarm.setAlarm("BARCODE REPOSITORY: Category storage call (highest category) made before initialization.");

      return FAILURE_REPO;
   }

   // Start looking for this from the beginning of the file.
   category_storage->rewind_cursor();

   while (category_record_number != EOF)
   {
      category_record_number = category_storage->read_key ((KEY_TYPE*)&RFS_key);

      // If we didn't run into the end of file, count it.

      if (category_record_number != EOF)
      {
         ++total_category_entries;
      }
   }

   return(total_category_entries);

}




////////////////////////////////////////////////////////////////////////////////
// LOG_BARCODE
void Barcode_Repository::log_barcode (const SCAN_STRUCTURE* callers_scan_buffer)
{
#define MAX_BARCODE_LENGTH 256
#define MAX_DATA_LENGTH 25

   SCAN_STRUCTURE scan_msg;

   // Build message structure
   memmove (&scan_msg, callers_scan_buffer, sizeof(SCAN_STRUCTURE));

   scan_msg.scan_string[scan_msg.scan_strlen] = '\0';  // null terminate the string

   // Build the barcode log string

   // Bracket the string
   DataLog(log_level_gui_barcode_external) << "*BCSTART* "
                                           << "Barcode_category="
                                           << scan_msg.scan_category << ","
                                           << "Barcode_sequence="
                                           << scan_msg.scan_sequence << ","
                                           << "Barcode_status="
                                           << scan_msg.status << ","
                                           << "Barcode_symbology="
                                           << scan_msg.scan_symbology_char[0] << ","
                                           << "Barcode_data="
                                           << scan_msg.scan_string << " "
                                           << scan_msg.scan_strlen << ","
                                           << " *BCEND*"
                                           << endmsg;

   // Allocate and send the message
   Message<SCAN_STRUCTURE> bc_scan_msg(MessageBase::SEND_LOCAL);
   bc_scan_msg.send (scan_msg);

}  // End of Barcode_Repository log_barcode




////////////////////////////////////////////////////////////////////////////////
// SHUT_DOWN_REPOSITORY
void Barcode_Repository::shut_down_repository ()
{
   // Don't shut down an UNinitialized repository
   if (!is_initialized)
   {
      DataLog(log_level_gui_barcode_error) << "BARCODE REPOSITORY: Method call to uninitialized repository" << endmsg;
      // return();
      return;
   }

   // Mark that we're virtually shutdown right now.
   is_initialized = false;

   // Close down the category storage
   category_storage->close_file();
   delete  category_storage;
   category_storage = NULL;

   // Clean the FIFO
   repo_fifo->free_all();
   delete repo_fifo;
   repo_fifo = NULL;

   // Free the sentinel message
   sentinel_msg.deregisterMsg();

}  // End of Barcode_Repository shut_down_repository




////////////////////////////////////////////////////////////////////////////////
// CLEAR_FIFO
void Barcode_Repository::clear_fifo ()
{
   if (!is_initialized)
   {
      DataLog(log_level_gui_barcode_error) << "BARCODE REPOSITORY: Method call to uninitialized repository" << endmsg;

      return;
   }

   // Clean out all the items in the FIFO
   repo_fifo->free_all();
}



////////////////////////////////////////////////////////////////////////////////
// RETURN_HISTORICAL_HIGH_WATERMARK
int Barcode_Repository::return_historical_high_watermark (SCAN_CATEGORY category)
{
   // First get the highest "existing" key
   int total           = 0;
   int high_water_mark = 0;
   return_category_count (category, &total, &high_water_mark);

   // Read the historical high
   SCAN_STRUCTURE hist_high_rec;

   const int      read_rc = read_category_item (category, HISTORICAL_HIGH_SEQ, &hist_high_rec);

   // Did one exist or not
   if (read_rc == FAILURE_REPO)  // we hit EOF
   {
      // one didn't exist, just return the current high_water_mark
      return(high_water_mark);
   }
   else // read_rc == SUCCESS_REPO : we found the hist. high water mark record
   {
      HHWM_UNION hhwm_buff;

      // This moves the hhwm data into the union.
      memmove (hhwm_buff.scan_buffer_dopple,
               hist_high_rec.scan_string,
               hist_high_rec.scan_strlen);

      // See if this is the highest (currently allocated highest vs. historically highest)
      return(hhwm_buff.historical_high_water >= high_water_mark ? hhwm_buff.historical_high_water : high_water_mark);
   }

}



////////////////////////////////////////////////////////////////////////////////
// INVOKE_BARCODE_SCREEN
void Barcode_Repository::invoke_barcode_screen ()
{
   anAlarmMsg barcode_alarm(BARCODE_INFO_ALARM);

   barcode_alarm.setAlarm ("GUI Barcode Scan alarm invocation");

}  // End of Barcode_Repository invoke_barcode_screen

/* FORMAT HASH 7b9bc872d93314691301ed6a1a329007 */
