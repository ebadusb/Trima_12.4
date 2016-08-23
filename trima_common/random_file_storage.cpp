/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2000 Gambro BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Random_file_storage

*     File name:   Random_file_storage.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: I:/BCT_Development/vxWorks/Trima/gui/rcs/random_file_storage.cpp 1.10 2003/05/14 22:34:08Z rm70006 Exp $
$Log: random_file_storage.cpp $
Revision 1.10  2003/05/14 22:34:08Z  rm70006
IT 5815.

Add conditional compile for SIMNT.  O_SYNC doesn't work on simulator.
Revision 1.9  2003/05/01 21:44:35Z  rm70006
IT 5815.

Add barcode reader.
Revision 1.8  2003/04/25 16:50:28Z  rm70006
IT 5841.

Update code so that it will work for CFR 11 file.
Revision 1.7  2003/04/23 16:37:43Z  rm70006
IT 5841.

Clean up file.  Replace constants with bools.
Revision 1.6  2003/04/04 17:59:07Z  rm10919
Datalog changes. IT 5818
Revision 1.5  2002/12/13 15:02:57Z  rm70006
Change allocation status to use char * instead of void *.  T2.2 compiler complained about the conversion.
Revision 1.4  2002/09/06 21:49:37Z  sb07663
file open for write parameters to match target server
Revision 1.3  2002/08/15 22:16:45  sb07663
Compiling version for Vx
Revision 1.2  2001/08/14 17:17:20  ms10234
5159 - Open files as read only all the time, open for write only when needed.
Revision 1.1  2000/11/06 19:31:36  tw09453
Initial revision
*/
#include <vxworks.h>
#include <fcntl.h>      // File IO

#define DEBUG 1

// CLASS INCLUDE
#include "random_file_storage.hpp"
#include "errnoLib.h"
#include "an_alarm.h"


// For some reason the VxSim doesnt work with the synch option.
#if CPU == SIMNT
#define  OPEN_READ_OPTIONS O_RDONLY
#define  OPEN_WRITE_OPTIONS (O_RDWR | O_CREAT)
#else
#define  OPEN_READ_OPTIONS (O_RDONLY | O_SYNC)
#define  OPEN_WRITE_OPTIONS (O_RDWR | O_CREAT | O_SYNC)
#endif




////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR for new files

Random_file_storage::Random_file_storage(const char* prm_file_name_ptr,
                                         RECORD_NUMBER_TYPE prm_number_of_records,
                                         int prm_record_size,
                                         int prm_key_size)
   : new_old_file (NEW_FILE),
     file_handle (BADOPEN),
     file_handle_back (BADOPEN),
     number_of_records(prm_number_of_records),
     record_size(prm_record_size),
     user_key_field_size (prm_key_size),
     record_cursor(0),
     physical_rec_buff_ptr(NULL),
     user_key_buff_ptr(NULL),
     header_size(sizeof(FILE_HDR)),
     allocation_status_field_size(ALLOCATION_STATUS_FIXED_SIZE),
     allocation_array_size(0),
     alloc_array_ptr(NULL)
{
   // Save a local copy of the file name

   if ((strlen(prm_file_name_ptr) + 1) <= FILE_NAME_SIZE)
   {
      strcpy (file_name, prm_file_name_ptr);
   }
   else
   {
      anAlarmMsg alarm(RANDOM_FILE_STORAGE_ERROR);
      alarm.setAlarm(file_name);

      DataLog(log_level_gui_error) << "RANDOM FILE: random record file name ["
                                   << prm_file_name_ptr << "](" << strlen(prm_file_name_ptr) << ") too long.  Should not have been more than "
                                   << FILE_NAME_SIZE << endmsg;
   }

   if ((strlen(prm_file_name_ptr) + strlen(BACK_FILE_EXT) + 1) <= FILE_NAME_SIZE)
   {
      strcpy (file_name_back, prm_file_name_ptr);
      strcat (file_name_back, BACK_FILE_EXT);
   }
   else
   {
      anAlarmMsg alarm(RANDOM_FILE_STORAGE_ERROR);
      alarm.setAlarm(file_name);

      DataLog(log_level_gui_error) << "RANDOM FILE: random record file name ["
                                   << prm_file_name_ptr << BACK_FILE_EXT << "](" << strlen(prm_file_name_ptr) + strlen(BACK_FILE_EXT) << ") too long.  Should not have been more than "
                                   << FILE_NAME_SIZE << endmsg;
   }

   // "new" the allocation array
   alloc_array_ptr = new  Allocation_array (number_of_records,
                                            allocation_status_field_size,
                                            user_key_field_size);

   // initialize it to all "free" records
   alloc_array_ptr -> initialize_allocation_status (ALLOCATION_STATUS_FREE);


   // get the length of the allocation array from the array object.
   unsigned char* dont_care = NULL;
   alloc_array_ptr -> get_serialized_array_pointer (&allocation_array_size, &dont_care);

}   // END of Random_file_storage CONSTRUCTOR




////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR for existing files

Random_file_storage::Random_file_storage(const char* prm_file_name_ptr)
   : new_old_file (OLD_FILE),
     file_handle (BADOPEN),
     file_handle_back (BADOPEN),
     number_of_records(0),    // must read from file
     record_size(0),          // must read from file
     user_key_field_size (0), // must read from file
     record_cursor(0),        // must read from file
     physical_rec_buff_ptr(NULL),
     user_key_buff_ptr(NULL),
     header_size(0),        // must read from file
     allocation_status_field_size(ALLOCATION_STATUS_FIXED_SIZE),
     allocation_array_size(0),
     alloc_array_ptr(NULL)
{
   // Save a local copy of the file name
   if ((strlen(prm_file_name_ptr) + 1) <= FILE_NAME_SIZE)
   {
      strcpy (file_name, prm_file_name_ptr);
   }
   else
   {
      anAlarmMsg alarm(RANDOM_FILE_STORAGE_ERROR);
      alarm.setAlarm(file_name);

      DataLog(log_level_gui_error) << "RANDOM FILE: random record file name ["
                                   << prm_file_name_ptr << "](" << strlen(prm_file_name_ptr) << ") too long.  Should not have been more than "
                                   << FILE_NAME_SIZE << endmsg;
   }

   if ((strlen(prm_file_name_ptr) + strlen(BACK_FILE_EXT) + 1) <= FILE_NAME_SIZE)
   {
      strcpy (file_name_back, prm_file_name_ptr);
      strcat (file_name_back, BACK_FILE_EXT);
   }
   else
   {
      anAlarmMsg alarm(RANDOM_FILE_STORAGE_ERROR);
      alarm.setAlarm(file_name);

      DataLog(log_level_gui_error) << "RANDOM FILE: random record file name ["
                                   << prm_file_name_ptr << BACK_FILE_EXT << "](" << strlen(prm_file_name_ptr) + strlen(BACK_FILE_EXT) << ") too long.  Should not have been more than "
                                   << FILE_NAME_SIZE << endmsg;
   }

}   // END of Random_file_storage CONSTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Random_file_storage::~Random_file_storage()
{
   // Close the main and backup files if they're open
   Random_file_storage::close_file();

   // Free the physical I/O buffer
   delete [] physical_rec_buff_ptr;
   delete [] user_key_buff_ptr;

   // Free the Allocation_array object
   delete alloc_array_ptr;

}   // END of Random_file_storage DESTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// CLOSE_FILE
void Random_file_storage::close_file ()
{
   // Close the backup file if it is open
   close_file(file_handle_back);

   // Close the main file if it is open
   close_file(file_handle);
}   // END of Random_file_storage CLOSE_FILE



void Random_file_storage::close_file (FILE_raw& fh)
{
   // Close the file if it is open
   if (fh != BADOPEN)
   {
      close (fh);
      fh = BADOPEN;
   }
}



////////////////////////////////////////////////////////////////////////////////
// OPEN_FILE
int Random_file_storage::open_file ()
{
   // If the file(s) is open, just return, noop it.
   if (file_handle != BADOPEN)
   {
      return 0;
   }


   // If this is a new file open, we need to:
   //   open the file (new)
   //   open the backup file (new)
   //   write out the header
   //   initialize the allocation array
   //   write out the allocation array record
   //   calculate the extent and write out the last byte
   //   write out the header and allocation array record to the backup file

   if (new_old_file == NEW_FILE)
   {
      // Open main file
      file_handle = open_filename_for_write(file_name);
      open_for_read(file_handle);
      if (file_handle == BADOPEN)
         return(errnoGet());

      // Open backup file
      file_handle_back = open_filename_for_write(file_name_back);
      open_for_read(file_handle_back);
      if (file_handle_back == BADOPEN)
         return(errnoGet());

      // Initialize and write out the header
      local_file_header.hdr_size_inclusive                = sizeof(FILE_HDR);
      memcpy(local_file_header.hdr_ID, HEADER_ID, HDR_ID_SIZE);
      local_file_header.hdr_allocation_element_fixed_size = allocation_status_field_size;
      local_file_header.hdr_user_key_field_size           = user_key_field_size;
      local_file_header.hdr_number_of_records             = number_of_records;
      local_file_header.hdr_record_size                   = record_size;
      local_file_header.hdr_CRC_of_allocation_array       = alloc_array_ptr -> get_array_crc();


      //// Write the headers to both files

      // Write this image to the backup file
      write_file_header (file_handle_back, &local_file_header);
      // Write this image to the main file
      write_file_header (file_handle,      &local_file_header);


      //// Write the allocation arrays to both files

      // Write the allocation array to the backup file
      write_file_allocation_array (file_handle_back,  &local_file_header);
      // Write the allocation array to the main file
      write_file_allocation_array (file_handle,       &local_file_header);


      //// Attempt to allocate the entire main file by writing the last byte of the last physical record

      //                                        byte location of beginning of last record
      RECORD_NUMBER_TYPE seek_to_last_byte = return_record_seek_position (number_of_records - 1)
                                             //                                        bump 1 byte past the end of the last physical record (record size + CRC suffix)
                                             + (return_record_size () + CRC_SIZE)
                                             //                                        backup 1 byte to land on the last byte of the last record.
                                             - 1;
      unsigned char last_byte = '\xff';

      write_bytes (file_handle, seek_to_last_byte, &last_byte, 1);


      // success
      return 0;


   } // End of: "if (new_old_file == NEW_FILE)
   else
   {
      // If this is an existing file open, we need to:
      //   open the file
      //   open the backup file
      //   read in the main file header
      //   initialize all the file related data members from the file header
      //   allocate the virtual array object
      //   read in the main allocation array
      //   set the allocation array in the virtual allocation array object
      //   check the header CRC of the allocation array with the virtual allocation array
      //   if the CRC fails, recover the allocation array from the backup file

      // Open main file
      file_handle = open_filename_for_read(file_name);
      if (file_handle == BADOPEN)
         return(errnoGet());

      // Open backup file
      file_handle_back = open_filename_for_read(file_name_back);
      if (file_handle_back == BADOPEN)
         return(errnoGet());

      // Read-in and check header CRC, size and file ID for consistency
      if (read_file_header (file_handle, &local_file_header))
      {
         // Success with primary file, don't do anything more...
      }
      else
      {
         if (read_file_header (file_handle_back, &local_file_header))
         {
            // Success with secondary file, don't do anything more...
         }
         else
         {
            DataLog(log_level_gui_error) << "RANDOM FILE: unable to read in a valid file header for " << file_name << ".  Msg" << errnoMsg << endmsg;
            return(errnoGet());
         }
      }

      // Assign all the file characteristics to local data members
      allocation_status_field_size = local_file_header.hdr_allocation_element_fixed_size;
      user_key_field_size          = local_file_header.hdr_user_key_field_size;
      number_of_records            = local_file_header.hdr_number_of_records;
      record_size                  = local_file_header.hdr_record_size;
      header_size                  = local_file_header.hdr_size_inclusive;

      // Reset record cursor
      rewind_cursor ();

      // Allocate the virtual array object
      alloc_array_ptr = new  Allocation_array (number_of_records,
                                               allocation_status_field_size,
                                               user_key_field_size);

      // get the length of the allocation array from the array object.
      unsigned char* dont_care = NULL;
      alloc_array_ptr -> get_serialized_array_pointer (&allocation_array_size, &dont_care);


      if (!load_and_check_allocation_array())
      {
         // Recover by freeing all allocation at this point... live with loss... for now... rewrite both headers and allocation arrays.
         free_all_records ();

         // Write the allocation array for the backup which will write out the header too.
         write_file_allocation_array (file_handle_back, &local_file_header);

         // Write the allocation array for the main which will write out the header too.
         write_file_allocation_array (file_handle, &local_file_header);


         DataLog(log_level_gui_error) << "RANDOM FILE: unable to recover allocation array - resetting to null." << endmsg;
      }

      // success
      return 0;


   }  // End of ELSE for: "if (new_old_file == NEW_FILE)


}  // END of Random_file_storage OPEN_FILE



FILE_raw Random_file_storage::open_for_read (FILE_raw fh)
{
   if (file_handle == fh)
   {
      close_file(file_handle);
      fh = file_handle = open_filename_for_read(file_name);
   }
   else // file_handle_back == fh
   {
      close_file(file_handle_back);
      fh = file_handle_back = open_filename_for_read(file_name_back);
   }

   if (fh == BADOPEN && errnoGet() != 0)
   {
      anAlarmMsg alarm(RANDOM_FILE_STORAGE_ERROR);
      alarm.setAlarm(file_name);

      DataLog(log_level_gui_error) << "RANDOM FILE: failed to open the random file storage " << file_name << ".  Msg " << errnoMsg << "." << endmsg;
   }

   return fh;
}



FILE_raw Random_file_storage::open_filename_for_read (const char* fn)
{
   FILE_raw fh = open (fn, OPEN_READ_OPTIONS, 0);

   if (fh == BADOPEN)
   {
      DataLog(log_level_gui_error) << "RANDOM FILE: unable to open file " << fn << "for read.  Msg " << errnoMsg << endmsg;
   }

   return fh;
}



FILE_raw Random_file_storage::open_for_write (FILE_raw fh)
{
   if (file_handle == fh)
   {
      close_file(file_handle);
      fh = file_handle = open_filename_for_write(file_name);
   }
   else // file_handle_back == fh
   {
      close_file(file_handle_back);
      fh = file_handle_back = open_filename_for_write(file_name_back);
   }

   if (fh == BADOPEN && errnoGet() != 0)
   {
      anAlarmMsg alarm(RANDOM_FILE_STORAGE_ERROR);
      alarm.setAlarm(file_name);

      DataLog(log_level_gui_error) << "RANDOM FILE: failed to open the random file storage " << file_name << ".  Msg " << errnoMsg << "." << endmsg;
   }

   return fh;
}



FILE_raw Random_file_storage::open_filename_for_write (const char* fn)
{
   FILE_raw fh = open (fn, OPEN_WRITE_OPTIONS, 0666);

   if (fh == BADOPEN)
   {
      DataLog(log_level_gui_error) << "RANDOM FILE: unable to open file " << fn << " for write.  Msg " << errnoMsg << endmsg;
   }

   return fh;
}



////////////////////////////////////////////////////////////////////////////////
// WRITE_FILE_HEADER
void Random_file_storage::write_file_header (FILE_raw file_handle,
                                             FILE_HDR* file_hdr_ptr)
{
   // Don't write to a closed/unopened file
   if (file_handle == BADOPEN)
   {
      return;
   }

   // Zero the CRC field "in" the header; calculate the CRC of the whole header; put it back in the header and write
   file_hdr_ptr->hdr_CRC_header = 0L;

   crcgen32(&file_hdr_ptr->hdr_CRC_header, (unsigned char*)file_hdr_ptr, sizeof(FILE_HDR));

   // Write the header out to the file
   write_bytes (file_handle,
                HEADER_SEEK_ADDRESS,
                (unsigned char*)file_hdr_ptr,
                file_hdr_ptr -> hdr_size_inclusive);

}  // END of Random_file_storage WRITE_FILE_HEADER



////////////////////////////////////////////////////////////////////////////////
// READ_FILE_HEADER

bool Random_file_storage::read_file_header (FILE_raw file_handle,
                                            FILE_HDR* file_hdr_ptr)
{
   unsigned long header_CRC_from_file;

   // Don't read from a closed/unopened file
   if (file_handle == BADOPEN)
   {
      return false;
   }

   // Read the header from the file
   if (!read_bytes (file_handle, HEADER_SEEK_ADDRESS, (unsigned char*)file_hdr_ptr, sizeof(FILE_HDR)))
   {
      return false;
   }

   // Compute the header's CRC and compare to the CRC read in.
   //
   //   save off CRC
   header_CRC_from_file = file_hdr_ptr -> hdr_CRC_header;

   //   zero it (same as when it was calculated)
   file_hdr_ptr -> hdr_CRC_header = 0L;

   //   recompute the CRC
   crcgen32(&file_hdr_ptr->hdr_CRC_header, (unsigned char*)file_hdr_ptr, sizeof(FILE_HDR));

   //   check if the computed CRC is the same as the CRC read from the file
   if (file_hdr_ptr -> hdr_CRC_header != header_CRC_from_file)
   {
      return false;
   }

   //   check the file header size
   if (file_hdr_ptr -> hdr_size_inclusive != sizeof(FILE_HDR))
   {
      return false;
   }

   //   check the file header file ID
   if (memcmp(local_file_header.hdr_ID, HEADER_ID, HDR_ID_SIZE) != 0)
   {
      return false;
   }

   // If we pass all the tests, return success
   return true;
}  // END of Random_file_storage READ_FILE_HEADER



////////////////////////////////////////////////////////////////////////////////
// WRITE_FILE_ALLOCATION_ARRAY

void Random_file_storage::write_file_allocation_array (FILE_raw file_handle,
                                                       FILE_HDR* file_hdr_ptr)
{
   // Don't write to a closed/unopened file
   if (file_handle == BADOPEN)
   {
      return;
   }

   // Calculate the allocation array CRC, move it into the file header image; update the header
   write_allocation_array_crc (file_handle, file_hdr_ptr);

   // Get the pointer to the array image and the length of that image
   unsigned long  alloc_array_size = 0;
   unsigned char* array_buffer_ptr = 0;
   alloc_array_ptr -> get_serialized_array_pointer (&alloc_array_size, &array_buffer_ptr);

   // Now write out the allocation array image itself
   write_bytes (file_handle,
                file_hdr_ptr -> hdr_size_inclusive, // just past the file header
                array_buffer_ptr,                   // copy the allocation array as bytes
                alloc_array_size);                  // for the length of the array

}  // END of Random_file_storage WRITE_FILE_ALLOCATION_ARRAY



////////////////////////////////////////////////////////////////////////////////
// READ_FILE_ALLOCATION_ARRAY

bool Random_file_storage::read_file_allocation_array (FILE_raw file_handle,
                                                      FILE_HDR* file_hdr_ptr)
{
   // Don't read from a closed/unopened file
   if (file_handle == BADOPEN)
   {
      return false;
   }

   // First read in the file header
   if (!read_file_header (file_handle, file_hdr_ptr))
   {
      return false;
   }

   // Create a temporary buffer to contain the allocation array.
   unsigned char* local_array_buffer_ptr = new unsigned char [allocation_array_size];

   // Read in the allocation array image itself
   if (!read_bytes (file_handle, file_hdr_ptr->hdr_size_inclusive, local_array_buffer_ptr, allocation_array_size))
   {
      delete [] local_array_buffer_ptr;
      return false;
   }

   // Move the allocation array into the allocation array object.
   alloc_array_ptr -> load_array_from_file(local_array_buffer_ptr);

   // Delete the buffer
   delete [] local_array_buffer_ptr;
   local_array_buffer_ptr = 0;

   // Check its integrity
   if (file_hdr_ptr -> hdr_CRC_of_allocation_array != alloc_array_ptr->get_array_crc())
   {
      return false;
   }

   // Return success
   return true;
}  // END of Random_file_storage READ_FILE_ALLOCATION_ARRAY




////////////////////////////////////////////////////////////////////////////////
// WRITE_ALLOCATION_ARRAY_CRC

void Random_file_storage::write_allocation_array_crc (FILE_raw file_handle,
                                                      FILE_HDR* file_hdr_ptr)
{

   // First calculate the CRC on the current allocation array and put it in the local copy of
   // the file header
   file_hdr_ptr -> hdr_CRC_of_allocation_array = alloc_array_ptr -> get_array_crc();

   // Write out the header first
   write_file_header (file_handle, file_hdr_ptr);


}  // END of Random_file_storage WRITE_ALLOCATION_ARRAY_CRC



////////////////////////////////////////////////////////////////////////////////
// MARK_RECORD_POSITION_AS_FREE

void Random_file_storage::mark_record_position_as_free (RECORD_NUMBER_TYPE record_location)
{
   if (alloc_array_ptr->set_allocation_status (record_location, ALLOCATION_STATUS_FREE) == ARR_FAILURE)
   {
      anAlarmMsg alarm(RANDOM_FILE_STORAGE_ERROR);
      alarm.setAlarm(file_name);

      DataLog(log_level_gui_error) << "RANDOM FILE: free: record number [" << record_location << "] out of bounds" << endmsg;
   }

}  // END of Random_file_storage MARK_RECORD_POSITION_AS_FREE



////////////////////////////////////////////////////////////////////////////////
// MARK_RECORD_POSITION_AS_USED

void Random_file_storage::mark_record_position_as_used (RECORD_NUMBER_TYPE record_location, const KEY_TYPE* record_key)
{
   if (alloc_array_ptr->set_allocation_status (record_location, ALLOCATION_STATUS_ALLOC) == ARR_FAILURE)
   {
      anAlarmMsg alarm(RANDOM_FILE_STORAGE_ERROR);
      alarm.setAlarm(file_name);

      DataLog(log_level_gui_error) << "RANDOM FILE: allocate: record [" << record_location << "] number out of bounds for key " << (int)*record_key << endmsg;
   }

   if (alloc_array_ptr->set_user_key(record_location, user_key_field_size, record_key) == ARR_FAILURE)
   {
      anAlarmMsg alarm(RANDOM_FILE_STORAGE_ERROR);
      alarm.setAlarm(file_name);

      DataLog(log_level_gui_error) << "RANDOM FILE: error while writing user key " << (int)*record_key << " to location " << record_location << endmsg;
   }

}  // END of Random_file_storage MARK_RECORD_POSITION_AS_USED



////////////////////////////////////////////////////////////////////////////////
// RETURN_NEXT_FREE_RECORD_NUMBER
RECORD_NUMBER_TYPE Random_file_storage::return_next_free_record_number ()
{
   RECORD_NUMBER_TYPE record_index;
   char               alloc_status[ALLOCATION_STATUS_FIXED_SIZE_MAX];

   for (record_index = 0; record_index < number_of_records; record_index++)
   {

      if ((alloc_array_ptr->get_allocation_status(record_index, alloc_status)) == ARR_FAILURE)
      {
         anAlarmMsg alarm(RANDOM_FILE_STORAGE_ERROR);
         alarm.setAlarm(file_name);

         DataLog(log_level_gui_error) << "RANDOM FILE: virtual array bounds inconsistency at " << record_index << endmsg;
         return(IO_ERR);
      }

      // If this is "free" or unallocated, return the index to the caller
      if (memcmp(alloc_status, ALLOCATION_STATUS_FREE, allocation_status_field_size) == 0)
      {
         return(record_index);
      }
   } // End of: "for (record_index = 0; record_index < number_of_records; record_index++)

   // We've fallen through, so we return a -1 saying there is no more space in the file.
   return(RECORD_NUMBER_TYPE(-1));

}  // END of Random_file_storage RETURN_NEXT_FREE_RECORD_NUMBER





////////////////////////////////////////////////////////////////////////////////
// LOAD_AND_CHECK_ALLOCATION_ARRAY

bool Random_file_storage::load_and_check_allocation_array ()
{

   // Make sure the virtual array is allocated
   if (alloc_array_ptr == NULL)
   {
      anAlarmMsg alarm(RANDOM_FILE_STORAGE_ERROR);
      alarm.setAlarm(file_name);

      DataLog(log_level_gui_error) << "RANDOM FILE: virtual allocation array has not been allocated" << endmsg;
      return false;
   }

   // Load in the main file's allocation array (and file header)
   if (!read_file_allocation_array  (file_handle, &local_file_header))
   {
      // Try the backup file's allocation array (and file header)
      if (!read_file_allocation_array  (file_handle_back, &local_file_header))
      {
         return false;
      }
   }

   // If we got here, one of the file's had a consistent allocation array
   return true;

}  // END of Random_file_storage LOAD_AND_CHECK_ALLOCATION_ARRAY




////////////////////////////////////////////////////////////////////////////////
// FREE_ALL_RECORDS

void Random_file_storage::free_all_records ()
{
   RECORD_NUMBER_TYPE record_index;

   for (record_index = 0; record_index < number_of_records; record_index++)
   {
      mark_record_position_as_free (record_index);

   } // End of: "for (record_index = 0; record_index < number_of_records; record_index++)


}  // END of Random_file_storage FREE_ALL_RECORDS




////////////////////////////////////////////////////////////////////////////////
// RETURN_NUMBER_OF_FREE_RECORDS
RECORD_NUMBER_TYPE Random_file_storage::return_number_of_free_records ()
{
   RECORD_NUMBER_TYPE record_index;
   RECORD_NUMBER_TYPE total_free = 0;
   char               alloc_status[ALLOCATION_STATUS_FIXED_SIZE_MAX];

   for (record_index = 0; record_index < number_of_records; record_index++)
   {

      if ((alloc_array_ptr->get_allocation_status(record_index, alloc_status)) == ARR_FAILURE)
      {
         anAlarmMsg alarm(RANDOM_FILE_STORAGE_ERROR);
         alarm.setAlarm(file_name);

         DataLog(log_level_gui_error) << "RANDOM FILE: virtual array bounds inconsistency at record index " << record_index << endmsg;
         return(IO_ERR);
      }

      // If this is "free" or unallocated, return the index to the caller
      if (memcmp(alloc_status, ALLOCATION_STATUS_FREE, allocation_status_field_size) == 0)
      {
         total_free++;
      }
   } // End of: "for (record_index = 0; record_index < number_of_records; record_index++)

   // We've fallen through, so we return a -1 saying there is no more space in the file.
   return(total_free);

}  // END of Random_file_storage RETURN_NUMBER_OF_FREE_RECORDS



////////////////////////////////////////////////////////////////////////////////
// RETURN_USER_KEY_FIELD_SIZE

int Random_file_storage::return_user_key_field_size ()
{
   if (alloc_array_ptr != NULL)
   {
      return(user_key_field_size);
   }
   else
   {
      return(0);
   }

}  // END of Random_file_storage RETURN_USER_KEY_FIELD_SIZE




////////////////////////////////////////////////////////////////////////////////
// RETURN_RECORD_SIZE

int Random_file_storage::return_record_size ()
{
   if (alloc_array_ptr != NULL)
   {
      return(record_size);
   }
   else
   {
      return(0);
   }

}  // END of Random_file_storage RETURN_RECORD_SIZE




////////////////////////////////////////////////////////////////////////////////
// REWIND_CURSOR

void Random_file_storage::rewind_cursor ()
{
   record_cursor = 0L;
}



////////////////////////////////////////////////////////////////////////////////
// RECORD_NUMBER_IS_IN_BOUNDS
bool Random_file_storage::record_number_is_in_bounds (RECORD_NUMBER_TYPE rec_number)
{
   if ((rec_number < 0) || (rec_number > (number_of_records - 1)))
   {
      return false;
   }

   return true;
}




////////////////////////////////////////////////////////////////////////////////
// RETURN_RECORD_SEEK_POSITION

RECORD_NUMBER_TYPE Random_file_storage::return_record_seek_position (RECORD_NUMBER_TYPE rec_number)
{
   RECORD_NUMBER_TYPE seek_address = -1;

   if (record_number_is_in_bounds(rec_number))
   {
      // Calculate the position in the file of this record offsetting the header and allocation array.
      seek_address =  header_size +  allocation_array_size + (rec_number * (record_size + CRC_SIZE));

      return seek_address;
   }
   else
   {
      return RECORD_NUMBER_TYPE(-1);
   }

}  // END of Random_file_storage RETURN_RECORD_SEEK_POSITION




////////////////////////////////////////////////////////////////////////////////
// WRITE_DIRECT

RECORD_NUMBER_TYPE Random_file_storage::write_direct (const REC_TYPE* record_src, const KEY_TYPE* key_src, bool commit)
{
   RECORD_NUMBER_TYPE free_rec;
   RECORD_NUMBER_TYPE record_seek_position;

   // Don't write to a closed/unopened file
   if (file_handle == BADOPEN)
   {
      return(IO_ERR);
   }

   // Check to see if the physical buffer has been allocated yet
   if (physical_rec_buff_ptr == NULL)
   {
      physical_rec_buff_ptr = new REC_TYPE [record_size + CRC_SIZE];
   }

   // Get a record allocation
   if ((free_rec = return_next_free_record_number()) == -1)
   {
      return(IO_ERR);
   }

   // Get the record seek position
   if ((record_seek_position = return_record_seek_position (free_rec)) == -1)
   {
      return(IO_ERR);
   }

   // Calculate the CRC
   CRC record_crc = 0L;
   crcgen32(&record_crc, record_src, record_size);

   // Move the record into the physical buffer
   memmove (physical_rec_buff_ptr, record_src, record_size);

   // Append the CRC to the record
   memmove (physical_rec_buff_ptr + record_size, (REC_TYPE*)&record_crc, CRC_SIZE);

   // Write out the physical record
   write_bytes (file_handle,
                record_seek_position,          // just past the file header
                physical_rec_buff_ptr,         // copy the record
                record_size + CRC_SIZE);       // for the length of a standard record

   // Update the allocation array element with the user key value
   mark_record_position_as_used (free_rec, key_src);

   if (commit)
   {
      commit_update();
   }
   // Return the record number used
   return(free_rec);

}  // END of Random_file_storage WRITE_DIRECT



////////////////////////////////////////////////////////////////////////////////
// READ_DIRECT
RECORD_NUMBER_TYPE Random_file_storage::read_direct (RECORD_NUMBER_TYPE record_number,
                                                     REC_TYPE* record_dst,
                                                     KEY_TYPE* key_dst)
{
   RECORD_NUMBER_TYPE record_seek_position;
   char               alloc_status[ALLOCATION_STATUS_FIXED_SIZE_MAX];

   // Don't read from a closed/unopened file
   if (file_handle == BADOPEN)
   {
      return(IO_ERR);
   }


   // Check to see if the physical buffer has been allocated yet
   if (physical_rec_buff_ptr == NULL)
   {
      physical_rec_buff_ptr = new REC_TYPE [record_size + CRC_SIZE];
   }

   // Check if the user key buffer has been allocated yet
   if (user_key_buff_ptr == NULL)
   {
      user_key_buff_ptr = new KEY_TYPE [user_key_field_size];
   }


   // Check if record is allocated; if so, retrieve the user key
   if ((alloc_array_ptr -> get_allocation_status(record_number, alloc_status)) == ARR_FAILURE)
   {
      return(IO_ERR);
   }
   else
   {
      if (memcmp(alloc_status, ALLOCATION_STATUS_ALLOC, allocation_status_field_size) == 0)
      {
         // It's allocated, so get its associated user key.
         if ((alloc_array_ptr -> get_user_key(record_number, user_key_field_size, user_key_buff_ptr)) == ARR_FAILURE)
         {
            return(IO_ERR);
         }
         else
         {
            memmove (key_dst, user_key_buff_ptr, user_key_field_size);
         }

      }
   }


   // Get the record seek position
   if ((record_seek_position = return_record_seek_position (record_number)) == -1)
   {
      return(IO_ERR);
   }

   // Read the record into the physical record buffer
   if (!read_bytes (file_handle, record_seek_position, physical_rec_buff_ptr, record_size + CRC_SIZE))
   {
      return(IO_ERR);
   }

   // Recreate the CRC
   CRC record_crc = 0L;
   crcgen32(&record_crc, physical_rec_buff_ptr, record_size);

   // Check it with the CRC in the physical record -- if not equal, return the I/O error RC
   if (memcmp ((void*)&record_crc, physical_rec_buff_ptr + record_size, CRC_SIZE) != 0)
   {
      return(IO_ERR);
   }

   // Otherwise, copy the caller's data into their buffer
   memmove (record_dst, physical_rec_buff_ptr, record_size);

   // Return with laurels
   return(record_number);

}  // END of Random_file_storage READ_DIRECT




////////////////////////////////////////////////////////////////////////////////
// READ_NEXT
RECORD_NUMBER_TYPE Random_file_storage::read_next (REC_TYPE* record_dst, KEY_TYPE* key_dst)
{
   RECORD_NUMBER_TYPE record_index;
   char               alloc_status[ALLOCATION_STATUS_FIXED_SIZE_MAX];


   // Don't continue searching if the cursor is out of bounds
   if (!record_number_is_in_bounds (record_cursor))
   {
      return(EOF);
   }

   for (record_index = record_cursor; record_index < number_of_records; record_index++)
   {

      if ((alloc_array_ptr -> get_allocation_status(record_index, alloc_status)) == ARR_FAILURE)
      {
         DataLog(log_level_gui_error) << "RANDOM FILE: virtual array bounds inconsistency" << endmsg;
         return(IO_ERR);
      }

      // If this is an allocated record return the key and record data to the caller
      if (memcmp(alloc_status, ALLOCATION_STATUS_ALLOC, allocation_status_field_size) == 0)
      {
         // Use read_direct to actually retrieve the data and key
         if (read_direct (record_index, record_dst, key_dst) == IO_ERR)
         {
            DataLog(log_level_gui_error) << "RANDOM FILE: read inconsistency in 'read direct'." << endmsg;
            return(IO_ERR);
         }

         // Bump the record_cursor past this record
         record_cursor = record_index + 1;

         // Return the record number of the record just read
         return(record_index);

      }
   } // End of: "for (record_index = record_cursor; record_index < number_of_records; record_index++)

   // We've fallen through, so we return EOF saying that we've exhausted the file.
   return(EOF);

}  // END of Random_file_storage READ_NEXT



////////////////////////////////////////////////////////////////////////////////
// READ_NEXT_KEY
RECORD_NUMBER_TYPE Random_file_storage::read_next_key (REC_TYPE* record_dst, KEY_TYPE* key_dst)
{
   RECORD_NUMBER_TYPE record_index;
   char               alloc_status[ALLOCATION_STATUS_FIXED_SIZE_MAX];

   // Don't continue searching if the cursor is out of bounds
   if (!record_number_is_in_bounds (record_cursor))
   {
      return(EOF);
   }

   // Check if the user key buffer has been allocated yet
   if (user_key_buff_ptr == NULL)
   {
      user_key_buff_ptr = new KEY_TYPE [user_key_field_size];
   }


   for (record_index = record_cursor; record_index < number_of_records; record_index++)
   {
      if ((alloc_array_ptr -> get_allocation_status(record_index, alloc_status)) == ARR_FAILURE)
      {
         DataLog(log_level_gui_error) << "RANDOM FILE: virtual array bounds inconsistency" << endmsg;
         return IO_ERR;
      }

      // If this is an allocated record and the key matches that of the caller,
      //  return the key and record data to the caller
      if (memcmp(alloc_status, ALLOCATION_STATUS_ALLOC, allocation_status_field_size) == 0)
      {
         if ((alloc_array_ptr -> get_user_key(record_index, user_key_field_size, user_key_buff_ptr)) == ARR_FAILURE)
         {
            DataLog(log_level_gui_error) << "RANDOM FILE: virtual array bounds inconsistency" << endmsg;
            return(IO_ERR);
         }
         else
         {
            // If the keys match, return the key and record
            if (memcmp(user_key_buff_ptr, key_dst, user_key_field_size) == 0)
            {
               // Use read_direct to actually retrieve the data and key
               if (read_direct (record_index, record_dst, key_dst) == IO_ERR)
               {
                  DataLog(log_level_gui_error) << "RANDOM FILE: read inconsistency in 'read direct'." << endmsg;
                  return(IO_ERR);
               }

               // Bump the record_cursor past this record
               record_cursor = record_index + 1;

               // Return the record number of the record just read
               return(record_index);

            }
         }

      } // End of: "if (memcmp(alloc_status,...
   }    // End of: "for (record_index = record_cursor; record_index < number_of_records; record_index++)

   // We've fallen through, so we return EOF saying that we've exhausted the file.
   return(EOF);

}  // END of Random_file_storage READ_NEXT_KEY




////////////////////////////////////////////////////////////////////////////////
// DELETE_RECORD
bool Random_file_storage::delete_record (RECORD_NUMBER_TYPE record_number)
{
   // Don't continue searching if the record number is out of bounds
   if (!record_number_is_in_bounds (record_number))
   {
      return false;
   }

   // Free this record
   mark_record_position_as_free (record_number);

   // Return successful
   return true;
}  // END of Random_file_storage DELETE_RECORD



////////////////////////////////////////////////////////////////////////////////
// READ_BYTES
bool Random_file_storage::read_bytes (FILE_raw file_handle,
                                      RECORD_NUMBER_TYPE record_seek_position,
                                      REC_TYPE* record_buff,
                                      int record_size)
{
   // Position into the file
   if (lseek(file_handle, record_seek_position, SEEK_SET) == -1)
   {
      return false;
   }

   // Read the data
   if (read(file_handle, (char*)record_buff, (size_t)record_size) != record_size)
   {
      return false;
   }

   // Successful read
   return true;

}  // END of Random_file_storage READ_BYTES




////////////////////////////////////////////////////////////////////////////////
// WRITE_BYTES
void Random_file_storage::write_bytes (FILE_raw fh,
                                       RECORD_NUMBER_TYPE record_seek_position,
                                       const REC_TYPE* record_buff,
                                       int record_size)
{
   fh = open_for_write(fh);

   // Position into the file
   if (lseek(fh, record_seek_position, SEEK_SET) == -1)
   {
      anAlarmMsg alarm(RANDOM_FILE_STORAGE_ERROR);
      alarm.setAlarm(file_name);

      DataLog(log_level_gui_error) << "RANDOM FILE: write, seek failure at position "
                                   << record_seek_position << ".  Msg " << errnoMsg << "." << endmsg;
   }

   // Write the data
   int byteswritten = write(fh, (char*)record_buff, record_size);

   if (byteswritten != record_size)
   {
      anAlarmMsg alarm(RANDOM_FILE_STORAGE_ERROR);
      alarm.setAlarm(file_name);

      DataLog(log_level_gui_error) << "RANDOM FILE: write, short write or I/O error["
                                   << byteswritten << "/" << record_size << "].  Msg " << errnoMsg << endmsg;
   }

   open_for_read(fh);
}  // END of Random_file_storage WRITE_BYTES



////////////////////////////////////////////////////////////////////////////////
// KEY_EXISTS

RECORD_NUMBER_TYPE Random_file_storage::key_exists (const KEY_TYPE* key_dst)
{
   RECORD_NUMBER_TYPE record_index;
   char               alloc_status[ALLOCATION_STATUS_FIXED_SIZE_MAX];

   // Don't continue searching if the cursor is out of bounds
   if (!record_number_is_in_bounds (record_cursor))
   {
      return(EOF);
   }

   // Check if the user key buffer has been allocated yet
   if (user_key_buff_ptr == NULL)
   {
      user_key_buff_ptr = new KEY_TYPE [user_key_field_size];
   }


   for (record_index = record_cursor; record_index < number_of_records; record_index++)
   {
      if ((alloc_array_ptr->get_allocation_status(record_index, alloc_status)) == ARR_FAILURE)
      {
         DataLog(log_level_gui_error) << "RANDOM FILE: virtual array bounds inconsistency" << endmsg;
         return(IO_ERR);
      }

      // If this is an allocated record and the key matches that of the caller,
      //  return the key and record data to the caller
      if (memcmp(alloc_status, ALLOCATION_STATUS_ALLOC, allocation_status_field_size) == 0)
      {
         if ((alloc_array_ptr->get_user_key(record_index, user_key_field_size, user_key_buff_ptr)) == ARR_FAILURE)
         {
            DataLog(log_level_gui_error) << "RANDOM FILE: virtual array bounds inconsistency" << endmsg;
            return(IO_ERR);
         }
         else
         {
            // If the keys match, return the index
            if (memcmp(user_key_buff_ptr, key_dst, user_key_field_size) == 0)
            {

               // Bump the record_cursor past this record
               record_cursor = record_index + 1;

               // Return the record number of the record just found in the allocation array
               return(record_index);

            }
         }

      } // End of: "if (memcmp(alloc_status,...
   }    // End of: "for (record_index = record_cursor; record_index < number_of_records; record_index++)

   // We've fallen through, so we return EOF saying that we've all allocated records.
   return(EOF);

}  // END of Random_file_storage KEY_EXISTS



////////////////////////////////////////////////////////////////////////////////
// COMMIT_UPDATE

bool Random_file_storage::commit_update ()
{
   // *** This should not be necessary with non-file-system I/O to a O_DSYNC file. Each write should be
   //    completed to the disk synchronously.

   // Flush the records in the primary file
   // if (fsync(file_handle) != 0)
   // {
   //   _FATAL_ERROR (__FILE__, __LINE__, TRACE_GUI, errno, "RANDOM FILE: unable to sync primary record file.");
   // }


   // Write the backup array
   write_file_allocation_array (file_handle_back, &local_file_header);

   // Flush it
   // if (fsync(file_handle_back) != 0)
   // {
   //   _FATAL_ERROR (__FILE__, __LINE__, "RANDOM FILE: unable to sync (flush) backup allocation file.");
   // }

   // Write the primary file's allocation array
   write_file_allocation_array (file_handle, &local_file_header);

   // Flush it
   // if (fsync(file_handle) != 0)
   // {
   //   _FATAL_ERROR (__FILE__, __LINE__, "RANDOM FILE: unable to sync (flush) primary allocation file.");
   // }

   return true;
}  // END of Random_file_storage COMMIT_UPDATE




////////////////////////////////////////////////////////////////////////////////
// READ_KEY

RECORD_NUMBER_TYPE Random_file_storage::read_key (KEY_TYPE* key_dst)
{
   RECORD_NUMBER_TYPE record_index;
   char               alloc_status[ALLOCATION_STATUS_FIXED_SIZE_MAX];

   // Don't continue searching if the cursor is out of bounds
   if (!record_number_is_in_bounds (record_cursor))
   {
      return(EOF);
   }

   // Check if the user key buffer has been allocated yet
   if (user_key_buff_ptr == NULL)
   {
      user_key_buff_ptr = new KEY_TYPE [user_key_field_size];
   }

   for (record_index = record_cursor; record_index < number_of_records; record_index++)
   {

      if ((alloc_array_ptr -> get_allocation_status(record_index, alloc_status)) == ARR_FAILURE)
      {
         DataLog(log_level_gui_error) << "RANDOM FILE: virtual array bounds inconsistency" << endmsg;
         return(IO_ERR);
      }

      // If this is an allocated record
      //  return the key and the record number to the caller
      if (memcmp(alloc_status, ALLOCATION_STATUS_ALLOC, allocation_status_field_size) == 0)
      {
         if ((alloc_array_ptr -> get_user_key(record_index, user_key_field_size, user_key_buff_ptr)) == ARR_FAILURE)
         {
            DataLog(log_level_gui_error) << "RANDOM FILE: virtual array bounds inconsistency" << endmsg;
            return(IO_ERR);
         }
         else
         {
            // Copy the key to the caller's buffer
            memmove (key_dst, user_key_buff_ptr, user_key_field_size);

            // Bump the record_cursor past this record
            record_cursor = record_index + 1;

            // Return the record number of the record just found in the allocation array
            return(record_index);

         }

      } // End of: "if (memcmp(alloc_status,...
   }    // End of: "for (record_index = record_cursor; record_index < number_of_records; record_index++)

   // We've fallen through, so we return EOF saying that we've read all the allocated keys.
   return(EOF);

}  // END of Random_file_storage READ_KEY

/* FORMAT HASH d7482ad0ebf9d72ec14a15a4f3692ddf */
