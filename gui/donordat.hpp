/************************> FOCUSSED GUI DOMAIN <****************************
*     Copyright(c) 2000 COBE BCT, Inc. All rights reserved

*     Class name:  Donor_Data_Repository

*     File name:   donordat.hpp

*     Contents:    Container for donor data


*************************< FOCUSSED GUI DOMAIN >****************************


*/

// Single include define guardian
#ifndef INCLUDE_DONORDAT
#define INCLUDE_DONORDAT
#include <vxWorks.h> // Needed for SIMNT compiler directives

#include "safproc.h"
#include "trimamessages.h"
#include "donor.h"

#include "bitmap_info.h"
#include "guistring.h"

#if 0
#include "cfg_ids.h"
#include "message.h"
#endif

// From where the data comes
enum UPDATED_DATA_SOURCE
{
   RECEIVED_FROM_SAFETY   = 10,
   RECEIVED_FROM_AP2AGENT = 20
};

class Donor_Data_Repository
{
public:
   Donor_Data_Repository();
   virtual ~Donor_Data_Repository();

   // Since this is a global object, its messages must
   // be created after the dipatcher is up.
   // Call initialize in GuiStart.
   void Initialize ();

   // START_DONOR_DATA
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   void start_donor_data_repository ();


   // DONOR_VITALS_STRUCT_INITIALZATION
   // This method initializes donor vitals struct values as follows. This has
   // been made public so that it can be called by DONVITAL when it performs
   // a goback to SYSDONOR.
   void donor_vitals_struct_initialization ();


   // debug logging of the message
   // Loggs the msg first followed by each member of SDonorVitals
   void LogVitals (const SDonorVitals& donor, const char* msg) const;

   // Call this to notify registered GUI modules that the data
   // has been received from Safety or everest.
   void NotifyGUI (UPDATED_DATA_SOURCE src);

   // call this to obtain a copy of the SDonorVitals structure.
   void get_donor_vitals_struct (SDonorVitals*)   // This isn't const in SIMNT.  For everybody else, it is.

#if CPU!=SIMNT
   const
#endif // #if CPU!=SIMNT
   ;

   // GET_DONOR_NAME_LANGUAGE
   // This routine returns the language in which the AP2 donor name is
   // encoded. e.g. "JAPANESE" means that the UTF8 of the donor's name
   // is in the Japanese glyph sets, Kanji, Hiragano, and should be
   // "text-flowed" as such. The parameters contain a pointer to a buffer
   // to contain the English rep. of the language, and the maximum length
   // of that buffer.
   void get_donor_name_language (char* language_buffer, int length);


   // This routine gets the donor picture from VIP, validates it, and sets up the
   // appropriate donor picture bitmap (could be generic donor or actual bitmap).
   void get_donor_bitmap_picture_file ();


   // This routine gets the appropriate donor bitmap id.  The returned value
   // could be BITMAP_DONOR_IMAGE, BITMAP_DONOR_IMAGE_DEFAULT_MALE, or
   // BITMAP_DONOR_IMAGE_DEFAULT_FEMALE depending on the result of the
   // get_donor_bitmap_picture_file call.
   BITMAP_ID GetBitmapId () const;
   BITMAP_ID GetBitmapIdSd () const;


private:
   Donor_Data_Repository             (const Donor_Data_Repository&); // noncopyable
   Donor_Data_Repository& operator = (const Donor_Data_Repository&); // nonassignable


private:
   // The language of the donor name -- not part of the SDonorVitals struct,
   //  therefore, this will be its persistent memory and it will be accessable
   //  with a "get" method. This is only needed to determine a text object's
   //  text flow algorithm (Japanese IS different than Latin1) should the
   //  language of the donor's not be the same as the current "class" default language.
   guistring donor_name_language_local;

   // possible donor message avenues
   DVitalsFromSafety  _msgFromSafety;    // from safety at the end of round robin
   DVitalsFromVista   _msgFromVista;     // sent in from Vista
   DonorDataStatusMsg _msgDataReceived;  // sent GUI to GUI notifying data received

   //  Callbacks from donor vitals messages
   void FromSafety ();
   void FromVista ();

   // This is the final donor used by GUI
   CDonor _Donor;

   // This flag is set when donor comes from Vista
   bool      _isFromVista;

   BITMAP_ID _DonorBitmap;
   BITMAP_ID _SDonorBitmap;

public:
   bool        isFromVista () { return _isFromVista; }
   BLOODTYPE   bloodType () { return _Donor.BloodType(); }
   float       tbv () { return _Donor.TBV(); }
   const char* dob () { return _Donor.DOB(); }
   const char* name () { return _Donor.Name(); }

   char donInfoStatus () { return _Donor.DonorInfoStatus(); }
   bool donInfoDefault () { return _Donor.DonorInfoDefault(); }
   bool donInfoComplete () { return _Donor.DonorInfoComplete(); }
   bool donInfoConfirmed () { return _Donor.DonorInfoConfirmed(); }

   char donVitalsStatus () { return _Donor.DonorVitalsStatus(); }
   bool donVitalsIncomplete () { return _Donor.DonorVitalsIncomplete(); }
   bool donVitalsComplete () { return _Donor.DonorVitalsComplete(); }
   bool donVitalsConfirmed () { return _Donor.DonorVitalsConfirmed(); }

   char donDlStatus () { return _Donor.DonorDownloadStatus(); }
   bool donDlComplete () { return _Donor.DonorDownloadComplete(); }
   bool donDlConfirmed () { return _Donor.DonorDownloadConfirmed(); }
   bool donDlDefault () { return _Donor.DonorDownloadDefault(); }
   bool donDlRejected () { return _Donor.DonorDownloadRejected(); }

   bool female () { return _Donor.Female(); }
   bool male () { return _Donor.Male(); }

   float height () { return _Donor.Height(); }
   int   heightInInches () { return _Donor.HeightInInches(); }
   int   heightInCm () { return _Donor.HeightInCm(); }
   bool  IsHeightInCm () const { return _Donor.HeightUnitsMetric(); }
   bool  IsHeightInIn () const { return !_Donor.HeightUnitsMetric(); }

   float weight () { return _Donor.Weight(); }
   int   weightInLb () { return _Donor.WeightInLb(); }
   int   weightInKg () { return _Donor.WeightInKg(); }
   bool  IsWeightInKg () const { return _Donor.WeightUnitsMetric(); }
   bool  IsWeightInLb () const { return !_Donor.WeightUnitsMetric(); }

   float Hct () { return _Donor.Hct(); }
   float Hgb () { return _Donor.Hgb(); }
   bool  RbcUnitsHct () { return _Donor.RbcUnitsHct(); }
   bool  RbcUnitsHgb () { return _Donor.RbcUnitsHgb(); }

   float precount () { return _Donor.PreCnt(); }
};


#endif

/* FORMAT HASH 3b464fcc57483273021df10800e12a4d */
