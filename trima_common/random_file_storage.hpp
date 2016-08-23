/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2000 Gambro BCT Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Random_file_storage

*     File name:    random_file_storage.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This contains the class declaration for an object that manages a
         fixed number of randomly accessed records in a disk file. It allows
         for sequential, sequential-keyed and random keyed access of fixed-
         sized records.

*************************< FOCUSSED GUI DOMAIN >***************************/


// Single include define guardian
#ifndef RANDOM_FILE_STORAGE_HPP
#define RANDOM_FILE_STORAGE_HPP

// SYSTEM INCLUDES
#include <sys/fcntlcom.h>


#define  NEW_FILE 1
#define  OLD_FILE 2

#define  FILE_NAME_SIZE (150)

// Header constants
#define  HEADER_ID     "RFIL"
#define  HDR_ID_SIZE   (sizeof(HEADER_ID))
#define  HEADER_SEEK_ADDRESS (0L)

#define  ALLOCATION_STATUS_ALLOC "A*"
#define  ALLOCATION_STATUS_FREE  "F*"
#define  ALLOCATION_STATUS_FIXED_SIZE_MAX 20
#define  ALLOCATION_STATUS_FIXED_SIZE (((sizeof(ALLOCATION_STATUS_ALLOC) - 1) <= ALLOCATION_STATUS_FIXED_SIZE_MAX) ? (sizeof(ALLOCATION_STATUS_ALLOC) - 1) : ALLOCATION_STATUS_FIXED_SIZE_MAX)

#define  BACK_FILE_EXT ".back"

#define  EOF (-1)
#define  IO_ERR (-2)
#define  BADOPEN (-1)


typedef  long RECORD_NUMBER_TYPE;

typedef  int FILE_raw;

typedef  unsigned long CRC;
#define  CRC_SIZE (sizeof(CRC))


// composed classes includes
//
#include "allocation_array.hpp"



typedef unsigned char KEY_TYPE;
typedef unsigned char REC_TYPE;


class Random_file_storage
{
private:
   // Unimplemented methods - no null constructor; copies; assignments
   Random_file_storage ();
   Random_file_storage(Random_file_storage const&);
   Random_file_storage& operator = (Random_file_storage const&);

   // The file header structure typedef
   struct FILE_HDR
   {
      long               hdr_size_inclusive;
      char               hdr_ID[HDR_ID_SIZE]; // "RFIL"
      long               hdr_allocation_element_fixed_size;
      long               hdr_user_key_field_size;
      RECORD_NUMBER_TYPE hdr_number_of_records;
      long               hdr_record_size;
      CRC                hdr_CRC_of_allocation_array;
      CRC                hdr_CRC_header;

   };

   // Memory image of the header
   FILE_HDR local_file_header;

   // New file / Old file variable
   int new_old_file; // This remembers the type of constructor that created this
                     //  instance so that on file open the instance knows whether
                     //  to format a new file or validate an existing one.
   // Name of the file
   char file_name[FILE_NAME_SIZE];

   // Name of the backup file for the allocation array
   char file_name_back[FILE_NAME_SIZE];

   // File handle of the opened main file
   FILE_raw file_handle;

   // File handle of the opened back file
   FILE_raw file_handle_back;

   // Size of the file header fields in aggregate (no allocation elements included here)
   int header_size;

   // Number of fixed-length records the file can hold
   RECORD_NUMBER_TYPE number_of_records;

   // Size of a single fixed length record
   int record_size;

   // Size of the allocation status field within a record allocation element within the allocation array
   int allocation_status_field_size;

   // Size of the user key field within a record allocation element within the allocation array
   int user_key_field_size;

   // Record cursor - used to keep track of logical position within the file when using the file in sequential or keyed sequential mode.
   //  note: the bounds of the file is 0 to (number_of_records - 1)
   RECORD_NUMBER_TYPE record_cursor;

   // Total size of the whole allocation array in bytes
   unsigned long allocation_array_size;

   // A pointer to the allocation array instance
   Allocation_array* alloc_array_ptr;

   // Physical record buffer pointer
   REC_TYPE* physical_rec_buff_ptr;

   // Use key buffer pointer
   KEY_TYPE* user_key_buff_ptr;

   // LOAD_AND_CHECK_ALLOCATION_ARRAY
   // This method will attempt to load the allocation array from the file into the
   // virtual allocation array. If the CRC from the header doesn't match the CRC
   // of the data in the array, this routine will then attempt to load the allocation
   // array from the backup file and check its CRC. If neither succeed in loading
   // this routine will return false otherwise it will return true.
   bool load_and_check_allocation_array ();

   // RETURN_NEXT_FREE_RECORD_NUMBER
   // This method searches from the beginning of the allocation table for the first free
   // record location. It returns this record sequence number to the caller. If there are no
   // free records left, this routine returns a -1 to the caller.
   RECORD_NUMBER_TYPE return_next_free_record_number ();

   // MARK_RECORD_POSITION_AS_USED
   // This method will mark the allocation_status_field within an allocation array element as
   // "used" so that no other records will be written into this position in the file.
   void mark_record_position_as_used (RECORD_NUMBER_TYPE record_location, const REC_TYPE* record_key);

   // MARK_RECORD_POSITION_AS_FREE
   // This method will mark the allocation_status_field within an allocation array element as
   // "free" enabling it to be used for storing another record.
   void mark_record_position_as_free (RECORD_NUMBER_TYPE record_location);


   // WRITE_FILE_HEADER
   // This method will position to the header location in the file and write out
   // the header whose address is passed as a parameter to the opened file.
   void write_file_header (FILE_raw file_handle,
                           FILE_HDR* file_hdr_ptr);

   // WRITE_FILE_ALLOCATION_ARRAY
   // This method will position to the allocation array position in the file and
   // write out a copy of the allocation array. First, however, it will calculate the CRC
   // of that image of the allocation array and rewrite the file header with that CRC.
   void write_file_allocation_array (FILE_raw file_handle,
                                     FILE_HDR* file_hdr_ptr);

   // WRITE_ALLOCATION_ARRAY_CRC
   // This method will write a CRC of the allocation array to the file header image and
   // then write the whole header out to the file.
   void write_allocation_array_crc (FILE_raw file_handle,
                                    FILE_HDR* file_hdr_ptr);

   // RETURN_RECORD_SEEK_POSITION
   // This method will be given a record number and return the seek position in the file
   // where the record is to be written. If the record number is outside the record allocation
   // bounds or any other errors, this routine will return a -1 otherwise it will return the
   // seek position (zero or positive number).
   RECORD_NUMBER_TYPE return_record_seek_position (RECORD_NUMBER_TYPE rec_number);


   // RECORD_NUMBER_IS_IN_BOUNDS
   // This method will receive a record number and check whether it is within the record
   // bounds (0 to (number_of_records-1)). If it is, it will return true, if not, it
   // will return false.
   bool record_number_is_in_bounds (RECORD_NUMBER_TYPE rec_number);


   // READ_FILE_HEADER
   // This method will read the header in from the specified file and check its CRC,
   // self-stated header length, and the file ID "RFIL". If all of these things check out,
   // the method will return true. If not, it will return false.
   bool read_file_header (FILE_raw file_handle, FILE_HDR* file_hdr_ptr);


   // READ_FILE_ALLOCATION_ARRAY
   // This method will read in the header from the specified file and the file allocation
   // array. It will check the allocation array's integrity via the CRC found in the file
   // header. If both the header and the allocation array check out, this routine will
   // return true, otherwise it will return false.
   bool read_file_allocation_array (FILE_raw file_handle, FILE_HDR* file_hdr_ptr);


protected:

   // READ_BYTES
   // This routine will seek to the position specified by the caller and read into the caller's
   // buffer the number of bytes specified by the caller. Reading outside of the file allocation
   // will cause this routine to return false. This routine expects the callers to have validated file allocation
   // ranges. Successful reads will return true.
   virtual bool read_bytes (FILE_raw file_handle,
                            RECORD_NUMBER_TYPE record_seek_position,
                            REC_TYPE* record_buff,
                            int record_size);


   // WRITE_BYTES
   // This routine will seek to the position specified by the caller and write into the file
   // the data passed by the caller for a length specified by the caller. Any I/O error
   // will cause a fatal error.  This routine expects the callers to have validated file allocation
   // ranges.
   virtual void write_bytes (FILE_raw file_handle,
                             RECORD_NUMBER_TYPE record_seek_position,
                             const REC_TYPE* record_buff,
                             int record_size);


   // Implements the open and close operations
   virtual FILE_raw open_filename_for_read (const char* filename);
   virtual FILE_raw open_filename_for_write (const char* filename);
   virtual FILE_raw open_for_read (FILE_raw fh);
   virtual FILE_raw open_for_write (FILE_raw fh);
   virtual void     close_file (FILE_raw& fh);

public:

   // CONSTRUCTOR
   //
   // New file creation constructor
   Random_file_storage (const char* file_name, RECORD_NUMBER_TYPE number_of_records, int record_size, int key_size);

   // Old (existing) file creation constructor
   Random_file_storage (const char* file_name);


   // DESTRUCTOR
   //
   virtual ~Random_file_storage();


   // OPEN_FILE
   // This method will open the random access file based on the file name and type of constructor (new file or existing).
   // If it fails, the return code will contain the errno, otherwise, it the open
   // is successful, it will return a zero. The class use must make this call, it is not
   // invoked at construction time. The open will be different depending on the type of
   // constructor that was invoked: open NEW or open EXISTING. This call will also open the
   // backup file in which a copy of the allocation table is kept.
   //
   virtual int open_file ();

   // CLOSE_FILE
   // This method will close the main file and the backup allocation array file.
   virtual void close_file ();


   // RETURN_NUMBER_OF_FREE_RECORDS
   // This method will return the number of record positions free within the file.
   RECORD_NUMBER_TYPE return_number_of_free_records ();

   // COMMIT_UPDATE
   // This will write out the backup allocation table to disk (and flush it); it will then
   // write out the allocation table to the main random file. Successful commit will return
   // a 1 and unsuccessful commit will return a zero.
   virtual bool commit_update ();

   // RETURN_USER_KEY_FIELD_SIZE
   // This method will return the length (in bytes) of the user key field found in the allocation
   // array element.
   int return_user_key_field_size ();

   // RETURN_RECORD_SIZE
   // This method will return the size of the fixed record size for this file
   int return_record_size ();

   // FREE_ALL_RECORDS
   // This method will mark all the records in the file as free i.e. available to be
   // written into.
   void free_all_records ();

   // REWIND_CURSOR
   // This method will reset the cursor to the first record in the file - i.e. 0 position.
   void rewind_cursor ();

   // WRITE_DIRECT
   // This will allocate a record, write the caller's contents into that record, copy the
   // caller's key into the allocation element and mark the record as being NOT free. This
   // does not cause the allocation array to be updated in the file. The caller must call
   // commit to effect this. If the write is successful, the routine will return the record
   // number otherwise it will return a IO_ERR.
   RECORD_NUMBER_TYPE write_direct (const REC_TYPE* record_src, const KEY_TYPE* key_src, bool commit = false);

   // READ_DIRECT
   // This will read the record found in record position n where n is the bounds of 0 to (number_of_records - 1).
   // It will return the record data and any associated key. If the read is successful it will
   // return the record's record number as passed to this routine; if the record does exist it will return an
   // IO_ERR (non-existence means not within bounds or not an allocated record).
   RECORD_NUMBER_TYPE read_direct (RECORD_NUMBER_TYPE record_number, REC_TYPE* record_dst, KEY_TYPE* key_dst);

   // READ_NEXT
   // This will read the next allocated record as pointed to by the record cursor.
   // It will return the record data and any associated key -- the return value will be the record
   // number of the record read in. If the read is successful it will
   // return a the record number (0-n); if the end-of-file (EOF) is encountered, it will return a -1.
   // After returning to the caller, the record_cursor will be pointing at the next unread
   // record or at EOF. Any errors during processing will cause this method to return with
   // an IO_ERR return code.
   RECORD_NUMBER_TYPE read_next (REC_TYPE* record_dst, KEY_TYPE* key_dst);

   // READ_NEXT_KEY
   // This will read the next allocated record as pointed to by the record cursor and whose
   // key matches the key passed to this method.
   // It will return the record data -- the return value will be the record
   // number of the record read in. If the end of file is encountered, it will return an EOF.
   // After returning to the caller, the record_cursor will be pointing just past the currently
   // read record or at EOF. Any errors during processing will cause this method to return
   // with an IO_ERR return code.
   RECORD_NUMBER_TYPE read_next_key (REC_TYPE* record_dst, KEY_TYPE* key_dst);

   // DELETE_RECORD
   // This will free a record found at a specific record position. If the record number
   // passed represents an allocated record, this routine will return true otherwise it
   // will return false.
   bool delete_record (RECORD_NUMBER_TYPE record_number);

   // READ_KEY
   // This method will return the key of the next allocated record starting at the cursor position.
   // This allows the user to read through all the existing keys and examine them for other
   // processing. If this call is made and there are no more keys to be read at the current
   // cursor position, this routine returns an EOF, otherwise it returns the record number
   // associated with the key. This reads only from the memory image of the allocation table
   // so this is memory fast. Any errors while reading returns an IO_ERR.
   RECORD_NUMBER_TYPE read_key (KEY_TYPE* key_dst);

   // KEY_EXISTS
   // This method will search the allocation array from the current position of the cursor and
   // return the record ID of the first record that has as its key the same value as passed by
   // the caller. If no key matched that passed by the caller, the routine will return EOF.
   // This method will not read from the file, but the memory image of the allocation array so
   // scanning the existence of specific records is memory fast. Any errors during processing will
   // cause this method to return with an IO_ERR return code.
   RECORD_NUMBER_TYPE key_exists (const KEY_TYPE* key_dst);
};


// End of single include define guardian
#endif

/* FORMAT HASH 85ebc4fe5221cce14987c4381e5d9f18 */
