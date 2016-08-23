/***************************************************************************
* Copyright(c) 2003 Gambro BCT Inc. All rights reserved
* Author:       Terry Wahl
* Class name:   Scan_structure
* File name:    Scan_structure.h
* Contents:     See prototype header descriptions below...
* Description:
     This contains the various definitions for barcode messages
*
***************************************************************************/



#ifndef SCAN_STRUCTURE_H
#define SCAN_STRUCTURE_H


enum { MAX_SCAN_LENGTH     = 120 };   // Number should be coordinated with Vista.
enum { MAX_RESPONSE_LENGTH = 129 };   // Number should be coordinated with Vista.



#define MAX_SYMB_LENGTH 1
#define SEGMENT_DELIM '>'



// Message structure used to transmit the barcode scan from barcode reader to the GUI
struct BARCODE_STRUCT
{
   char symbology_code;                 // if NOT '\0' then this contains the Welsh Allyn imageteam 3800 Code ID
   bool barcode_read_error;             // error code
   int  barcode_len;                    // length of the barcode data (in case function codes are embedded including NUL)
   char barcode_data[MAX_SCAN_LENGTH];  // scan data
};





enum SCAN_STATUS
{
   BC_STATUS_ADDED,
   BC_STATUS_DELETED,
   BC_STATUS_BAD_SCAN,
   BC_STATUS_GOOD_SCAN,
   BC_STATUS_NA
};



// Note:
//    This struct is used by Vista.  The order is important.
//    Changing this list may require a change to the Vista interface
//
enum SCAN_CATEGORY
{
   BC_CAT_OPERATOR,
   BC_CAT_DONOR,
   BC_CAT_TUBING_SET,
   BC_CAT_DONATION_ID,
   BC_CAT_AC,
   BC_CAT_REPLACEMENT_FLUID,
   BC_CAT_SAMPLE_BAG,
   BC_CAT_NOTE,
   BC_CAT_MISC1,
   BC_CAT_MISC2,
   BC_CAT_MISC3,
   BC_CAT_MISC4, // This is no longer used. It's saved here so all enum items maintiain their same value.
   BC_CAT_RBC_STORAGE_SOL,
   BC_CAT_PLT_STORAGE_SOL,
   BC_CAT_DATA,
   BC_CAT_MISC5, // Added at end to preserve ordering
   BC_CAT_NO_ASSIGNMENT
};



enum VISTA_STATUS
{
   BC_VISTA_PENDING,
   BC_VISTA_OK,
   BC_VISTA_INVALID,
   BC_VISTA_INCORRECT_CATEGORY,
   BC_VISTA_INVALID_DATA
};



//
// Message between Barcode Reader and GUI, Vista for barcoded items on Trima
//
struct SCAN_STRUCTURE
{
   time_t        scan_time;
   SCAN_CATEGORY scan_category;
   int           scan_sequence;
   SCAN_STATUS   status;
   VISTA_STATUS  vista_response;
   int           scan_strlen;
   char          scan_string [MAX_SCAN_LENGTH];
   char          scan_symbology_char [MAX_SYMB_LENGTH];

   SCAN_STRUCTURE()
      : scan_time(0),
        scan_category(BC_CAT_NO_ASSIGNMENT),
        scan_sequence(0),
        status(BC_STATUS_NA),
        vista_response(BC_VISTA_INVALID_DATA),
        scan_strlen(0)
   {
      memset(scan_string, '\0', MAX_SCAN_LENGTH);
      memset(scan_symbology_char, '\0', MAX_SYMB_LENGTH);
   }

};



// Key structure
struct CAT_SEQ_KEY
{
   SCAN_CATEGORY category;
   int           sequence_number;
};



//
// Message between Vista Interface and GUI for Vista feedback to barcode messages.
//
struct VISTA_BARCODE_FEEDBACK
{
   SCAN_CATEGORY scan_category;
   int           scan_sequence;
   VISTA_STATUS  vista_status;
   int           vista_response_length;
   char          vista_response[MAX_RESPONSE_LENGTH];

   VISTA_BARCODE_FEEDBACK()
      : scan_category(BC_CAT_NO_ASSIGNMENT),
        scan_sequence(0),
        vista_status(BC_VISTA_INVALID_DATA),
        vista_response_length(0)
   {
      memset(vista_response, '\0', MAX_RESPONSE_LENGTH);
   }
};



//
// Message between Barcode Reader and CFR11 Screen passing on Operator Badge ID.
//
struct BADGE_BARCODE_MSG
{
   char badgeString[MAX_SCAN_LENGTH];
   int  badgeLength;
};

//
// Message between Barcode Reader and PAS Confirmation pop-up
//
struct PAS_BAG_BARCODE_MSG
{
   char pasBagString[MAX_SCAN_LENGTH];
   int  pasBagLength;
};

// End of single include define guardian
#endif

/* FORMAT HASH b826fa8b132f707c42922a2340663b4b */
