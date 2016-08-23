/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      donordat.cpp
 *             Donor Data container for GUI land
 *
 *
 ******************************************************************************/
#include <vxworks.h>

// header file include
#include "donordat.hpp"
#include "donorDataItems.hpp"

// GUI global element include
#define  NO_EXTERN_DONOR_DATA_REPOSITORY
#include "guiglobs.hpp"

#include "filenames.h"
#include "crcgen.h"
#include "usrLib.h"

const int PIX_FILENAME_LEN = 200;


Donor_Data_Repository::Donor_Data_Repository()
   : _msgFromSafety   (),
     _msgFromVista    (),
     _Donor           (),
     _isFromVista     (false),
     _DonorBitmap     (BITMAP_NULL),
     _SDonorBitmap    (BITMAP_NULL)

{
   donor_name_language_local.initialize();
}



Donor_Data_Repository::~Donor_Data_Repository()
{}



void Donor_Data_Repository::Initialize ()
{
   // Register for donor messages
   _msgFromSafety.init(Callback<Donor_Data_Repository>(this, &Donor_Data_Repository::FromSafety), MessageBase::SNDRCV_RECEIVE_ONLY);
   _msgFromVista.init(Callback<Donor_Data_Repository>(this, &Donor_Data_Repository::FromVista), MessageBase::SNDRCV_RECEIVE_ONLY);
   _msgDataReceived.init(MessageBase::SEND_LOCAL);
}



void Donor_Data_Repository::FromSafety ()
{
   _Donor = _msgFromSafety.getData();

   // Populate the data items for string substitution
   DonorDataItems safetyDonorItems(_Donor);

   NotifyGUI(RECEIVED_FROM_SAFETY);
   LogVitals(_Donor.cData(), "Received from Safety");
}



void Donor_Data_Repository::FromVista ()
{
   _isFromVista = true;
   guiglobs::donor_platelet_precount_entered = true; // always true when info entered from Vista

   _Donor = _msgFromVista.getData();

   // Populate the data items for string substitution
   DonorDataItems vistaDonorItems(_Donor);

   NotifyGUI(RECEIVED_FROM_AP2AGENT);
   LogVitals(_Donor.cData(), "Received from Vista");
   get_donor_bitmap_picture_file();
}


void Donor_Data_Repository::start_donor_data_repository ()
{
   // initialize the donor vitals structure
   donor_vitals_struct_initialization ();
}


void Donor_Data_Repository::donor_vitals_struct_initialization ()
{
   _Donor.ResetDonor();
}


void Donor_Data_Repository::LogVitals (const SDonorVitals& donor,
                                       const char* msg
                                       ) const
{
   DataLog(log_level_gui_info) << msg << " Status -- "
                               << " Vitals: " << (int)donor.cDonorVitalsStatus
                               << " Info:" << (int)donor.cDonorInfoStatus
                               << " Download:" << (int)donor.cDonorDownloadStatus
                               << " Sex:" << (int)donor.cDonorSex
                               << " Ht:" << donor.fDonorHeight
                               << " HtUnit:" << (int)donor.cDonorHeightUnits
                               << " Wt:" << donor.fDonorWeight
                               << " WtUnit:" << (int)donor.cDonorWeightUnits
                               << " SV:" << donor.fDonorSampleVolume
                               << " BT:" << donor.DonorBloodType
                               << " Hct:" << donor.fDonorHct
                               << " HctUnit:" << (int)donor.cDonorRbcCount
                               << " PreCnt:" << donor.fDonorPltPrecount
                               << " pTBV:" << donor.fProcDonorTBV
                               << " sTBV:" << donor.fSafetyDonorTBV
                               << endmsg;
}

// Obtain the SDonorVitals data from the golden copy
void Donor_Data_Repository::get_donor_vitals_struct (SDonorVitals* gui_donor_vitals_struct_ext_ptr)   // This isn't const in SIMNT.  For everybody else, it is.
#if CPU!=SIMNT
const
#endif // #if CPU!=SIMNT
{
   memcpy (gui_donor_vitals_struct_ext_ptr, &_Donor.cData(), sizeof (SDonorVitals));
}


void Donor_Data_Repository::NotifyGUI (UPDATED_DATA_SOURCE src)
{
   _msgDataReceived.send((int)src);
}



// This routine gets the donor picture from VIP, validates it, and sets up the
// appropriate donor picture bitmap (could be generic donor or actual bitmap).
void Donor_Data_Repository::get_donor_bitmap_picture_file ()
{
   //// Get the CRC for the bitmap image, turn it into a string, create the donor picture file name.
   char                str_CRC[20] = {};
   unsigned long       fileCrc = 0;
   const unsigned long dataCrc = _Donor.cData().DonorDataCRC;

   // The following is used for retrieving the file name of the donor picture
   char str_src_donor_pix_filename[PIX_FILENAME_LEN] = "";

   // Convert the binary to printable
   sprintf (str_CRC, "%08X", (unsigned int)dataCrc);

   // Create the donor photo filename (it may or may not exist) using the bitmap CRC value.
   strcat(str_src_donor_pix_filename, UPDATE_PATH);
   strcat(str_src_donor_pix_filename, "/");
   strcat(str_src_donor_pix_filename, str_CRC);
   strcat(str_src_donor_pix_filename, ".BMP");

   DataLog(log_level_gui_info) << "Attempting to open donor photo file name" << str_src_donor_pix_filename << endmsg;

   // Make sure that the returned file is marked as "absolute" and therefore subject to over-writing
   if ( !bitmap_data_table[BITMAP_DONOR_IMAGE].absolutePath )
   {
      _FATAL_ERROR (__FILE__, __LINE__, "Donor bitmap file is not an absolute path filename");
   }

   char str_dest_donor_pix_filename[PIX_FILENAME_LEN];
   strncpy(str_dest_donor_pix_filename, (const char*)bitmap_data_table[BITMAP_DONOR_IMAGE].data, PIX_FILENAME_LEN);

   // Attempt to remove any residual (old) donor photos from the "dest" directory.
   FILE* pixFile = fopen(str_dest_donor_pix_filename, "r");

   if (pixFile)
   {
      DataLog(log_level_gui_error) << "Destination file " << str_dest_donor_pix_filename << " exists. Cleanup necessary." << endmsg;
      fclose(pixFile);

      STATUS returnStatus = attrib(str_dest_donor_pix_filename, "-r");

      if (returnStatus != 0)
         DataLog(log_level_gui_error) << "attrib("
                                      << str_dest_donor_pix_filename
                                      << "; -r) returned status " << returnStatus << endmsg;

      returnStatus = unlink(str_dest_donor_pix_filename);

      if (returnStatus != 0)
         DataLog(log_level_gui_error) << "unlink("
                                      << str_dest_donor_pix_filename
                                      << ") returned status " << returnStatus << endmsg;
   }

   STATUS returnStatus = rename(str_src_donor_pix_filename, str_dest_donor_pix_filename);

   if (returnStatus != 0)
      DataLog(log_level_gui_error) << "rename("
                                   << str_src_donor_pix_filename << "; " << str_dest_donor_pix_filename
                                   << ") returned status " << returnStatus << endmsg;

   // Calculate the CRC for the donor bitmap file.
   int status = file_crcgen32 (str_dest_donor_pix_filename, &fileCrc);

   if ( (status == 0) && (fileCrc == dataCrc) )  // 0 is success in CRC-land
   {
      // picture has been renamed to the donor factotum file name and moved into the donor dest directory -- no worries
      _DonorBitmap  = BITMAP_DONOR_IMAGE ;
      _SDonorBitmap = BITMAP_DONOR_IMAGE;
   }
   else // We either don't have a valid donor photo, or no photo, therefore we'll use generic defaults
   {
      DataLog(log_level_gui_error) << "Invalid CRC in donor photo(" << status
                                   << "), (" << hex << dataCrc << " != " << fileCrc << ")" << dec
                                   << endmsg;

      if (_Donor.cData().cDonorSex == DONOR_SEX_IS_MALE)
      {
         _DonorBitmap  = BITMAP_DONOR_IMAGE_DEFAULT_MALE ;
         _SDonorBitmap = BITMAP_SYSDONOR_IMAGE_DEFAULT_MALE ;
      }
      else
      {
         _DonorBitmap  = BITMAP_DONOR_IMAGE_DEFAULT_FEMALE ;
         _SDonorBitmap = BITMAP_SYSDONOR_IMAGE_DEFAULT_FEMALE ;
      }
   }

   FILE* newPixFile = fopen(str_dest_donor_pix_filename, "r");

   if (newPixFile)
   {
      DataLog(log_level_gui_info) << "Destination face file " << str_dest_donor_pix_filename << " creation successful." << endmsg;
      fclose(newPixFile);
   }
}



// This routine gets the appropriate donor bitmap id.  The returned value
// could be BITMAP_DONOR_IMAGE, BITMAP_DONOR_IMAGE_DEFAULT_MALE, or
// BITMAP_DONOR_IMAGE_DEFAULT_FEMALE depending on the result of the
// get_donor_bitmap_picture_file call. Default for donordl screen.
BITMAP_ID Donor_Data_Repository::GetBitmapId () const
{
   return _DonorBitmap;
}

// This routine gets the appropriate donor bitmap id.  The returned value
// could be BITMAP_DONOR_IMAGE, BITMAP_DONOR_IMAGE_DEFAULT_MALE, or
// BITMAP_DONOR_IMAGE_DEFAULT_FEMALE depending on the result of the
// get_donor_bitmap_picture_file call. Default for sysdonor screen
BITMAP_ID Donor_Data_Repository::GetBitmapIdSd () const
{
   return _SDonorBitmap;
}

/* FORMAT HASH c085e79be6bb6c736dc2048ee7ef2a91 */
