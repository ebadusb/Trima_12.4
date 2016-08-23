/******************************************************************************
 * FILENAME: corrupt.cpp
 *
 * PURPOSE:  Utility to allow user to corrupt Safety's and/or Control's
 *           Power Fail Recovery data, by complimenting a specified byte
 *           or explicitly setting a named field to a value.
 *
 *           Options are read from /machine/tools/corrupt.dat.
 *           See initItemMap() to add support for new fields.
 *
 * USAGE:    corrupt_main.out reads options from corrupt.dat
 ******************************************************************************/

// vxworks includes
#include <vxWorks.h>
#include <usrLib.h>
#include <taskLib.h>
#include <ioLib.h>
#include <string>
#include <map>

// common includes
#include "crcgen.h"   // for crc generation and checking
#include "trima_datalog_levels.h"

// trima includes
#include "datfile.h"
#include "filenames.h"
#include "../../safety/s_pfr.hpp"
#include "../../os/power_fail.h"

////////////////////////////////////////////////////////////////////////////////
// Local constants
//
#define PNAME_CORRUPT_DAT    TOOLS_PATH "/corrupt.dat"
#define PFR_FIELDS_SECTION   "SAFETY_PFR_FIELDS"

////////////////////////////////////////////////////////////////////////////////
// Convenience macros for logging
//
#define DLOG_INFO( ... ) \
   DataLog(log_level_spoofer_warning) << __VA_ARGS__ << endmsg;

#define DLOG_ERROR( ... ) \
   DataLog(log_level_spoofer_error) << __VA_ARGS__ << endmsg;

////////////////////////////////////////////////////////////////////////////////

//
// Carries out the vile deed.
//
class Corrupter
{
public:
   Corrupter(const char* datFilePath);
   ~Corrupter();

   bool execute();

private:
   void readDatFileOptions(const char* datFilePath);
   void initItemMap();

   bool corruptControl();
   bool loadControl();
   bool saveControl();
   bool updateControlCRC();

   bool corruptSafety();
   bool loadSafety();
   bool saveSafety();
   bool updateSafetyCRC();

   bool flipByte(void* buffer, int offset, size_t max_bytes);
   void printControl();
   void printSafety();

private:
   // options read from datFile
   bool _corruptControl;
   bool _corruptSafety;
   bool _clearData;
   bool _updateCRC;
   bool _logControl;
   bool _logSafety;
   bool _corruptByte;
   bool _corruptItems;
   int _byteOffset;

   // Support for setting named values specified in the datFile
   struct Item
   {
      enum  VType {vtNULL, vtBOOL, vtCHAR, vtINT, vtENUM, vtFLOAT, vtLONG};

      union Value
      {
         bool  bVal;
         char  cVal;
         int   iVal;
         int   eVal;  // enums are stored as int
         float fVal;
         long  lVal;
         unsigned char bytes[sizeof(long)];
      };

      VType     itemType;
      Value     itemValue;
      size_t    itemSize;
      ptrdiff_t dataOffset;

      Item(VType type=vtNULL, ptrdiff_t offset=-1, size_t size=0)
      : itemType(type), itemValue(), itemSize(0), dataOffset(offset)
      {
         switch (itemType)
         {
         case vtBOOL:  itemSize = sizeof(bool);          break;
         case vtCHAR:  itemSize = sizeof(char);          break;
         case vtINT:   itemSize = sizeof(int);           break;
         case vtENUM:  itemSize = sizeof(int);           break;
         case vtFLOAT: itemSize = sizeof(float);         break;
         case vtLONG:  itemSize = sizeof(long);          break;
         default:      itemSize = size;                  break;
         }
         // sanity check
         if ( size != itemSize )
         {
            DLOG_ERROR( "ERROR! size for type=" << itemType << " disagrees w. size=" << size );
            dataOffset = -1;
         }
      }
   };

   // The map of named items
   typedef map<const char*, Corrupter::Item>  ItemMap;
   ItemMap _pfrItems;

   // internal states
   unsigned char * _ControlPFData;
   SPFRecovery * _SafetyPFData;
   bool _okay;
};


Corrupter::Corrupter(const char* datFilePath) :
 _corruptControl(false),
 _corruptSafety(false),
 _clearData(false),
 _updateCRC(false),
 _logControl(false),
 _logSafety(false),
 _corruptByte(false),
 _corruptItems(false),
 _byteOffset(-1),
 _pfrItems(),
 _ControlPFData(NULL),
 _SafetyPFData(NULL),
 _okay(false)
{
   readDatFileOptions( datFilePath );
}


Corrupter::~Corrupter()
{
   delete []_ControlPFData;
   delete _SafetyPFData;
}


bool Corrupter::execute()
{
   // Work on Control's PFR data
   //
   if (_okay && _corruptControl)
   {
      _okay = corruptControl();

      if (_okay && _updateCRC)
      {
         _okay = updateControlCRC();
      }

      if (_okay)
      {
         _okay = saveControl();
      }
   }

   // Work on Safety's PFR data
   //
   if (_okay && _corruptSafety)
   {
      _okay = corruptSafety();

      if (_okay && _updateCRC)
      {
         _okay = updateSafetyCRC();
      }

      if (_okay)
      {
         _okay = saveSafety();
      }
   }

   // Handle print options
   //
   if (_logControl)
   {
      printControl();
   }
   if (_logSafety)
   {
      printSafety();
   }

   return _okay;
}


void Corrupter::readDatFileOptions(const char* datFilePath)
{
   DLOG_INFO( "reading options from: " << datFilePath );

   CDatFileReader datfile( datFilePath );

   _okay = ( datfile.Error() == DFR_OK );

   if ( _okay )
   {
      // Read options. Note that false is the default if not present
      const string True( "true" );

      _corruptControl = ( True == datfile.GetString( "corruptControl" ) );
      _corruptSafety  = ( True == datfile.GetString( "corruptSafety" ) );
      _clearData      = ( True == datfile.GetString( "clearData" ) );
      _updateCRC      = ( True == datfile.GetString( "updateCRC") );
      _logControl     = ( True == datfile.GetString( "logControl" ) );
      _logSafety      = ( True == datfile.GetString( "logSafety" ) );
      _corruptByte    = ( datfile.Find( "byte" ) != NULL );
      if ( _corruptByte )
      {
         _byteOffset  = datfile.GetInt( "byte" );
      }

      //
      // Read the options for setting named field=value pairs
      //
      const char sectName[] = PFR_FIELDS_SECTION;
      if ( datfile.HasSection( sectName ) )
      {
         // Initialize the supported fields
         initItemMap();

         // Search for the supported fields in the dat file
         ItemMap::iterator it;
         for ( it = _pfrItems.begin(); it != _pfrItems.end(); ++it )
         {
            float fval = 0.0;
            if ( datfile.Find( sectName, it->first ) != NULL )
            {
               // get the value as a float and then cast to correct type
               fval = datfile.GetFloat(sectName, it->first);
               switch ( it->second.itemType )
               {
                  case Item::vtBOOL:
                     it->second.itemValue.bVal = (bool)fval;
                     DLOG_INFO( "Using: field=" << it->first
                                << " value=" << it->second.itemValue.bVal
                                << " offset=" << it->second.dataOffset );
                     break;
                  case Item::vtCHAR:
                     it->second.itemValue.cVal = (char)fval;
                     DLOG_INFO( "Using: field=" << it->first
                                << " value=" << (int)it->second.itemValue.cVal
                                << " offset=" << it->second.dataOffset );
                     break;
                  case Item::vtENUM:
                     it->second.itemValue.eVal = (int) fval;
                     DLOG_INFO( "Using: field=" << it->first
                                << " value=" << it->second.itemValue.eVal
                                << " offset=" << it->second.dataOffset );
                     break;
                  case Item::vtINT:
                     it->second.itemValue.iVal = (int) fval;
                     DLOG_INFO( "Using: field=" << it->first
                                << " value=" << it->second.itemValue.iVal
                                << " offset=" << it->second.dataOffset );
                     break;
                  case Item::vtFLOAT:
                     it->second.itemValue.fVal = fval;
                     DLOG_INFO( "Using: field=" << it->first
                                << " value=" << it->second.itemValue.fVal
                                << " offset=" << it->second.dataOffset );
                     break;
                  case Item::vtLONG:
                     it->second.itemValue.lVal = (long) fval;
                     DLOG_INFO( "Using: field=" << it->first
                                << " value=" << it->second.itemValue.lVal
                                << " offset=" << it->second.dataOffset );
                     break;
                  default:
                     it->second.dataOffset = -1;
                     break;
               }

               if ( it->second.dataOffset != -1 ) _corruptItems = true;
            }
            else // field not found; turn off this field
            {
               it->second.dataOffset = -1;
            }
         }
      } // HasSection()

      // Options summary
      DLOG_INFO( "Options: corruptControl=" << _corruptControl
                 << " corruptSafety=" << _corruptSafety
                 << " clearData=" << _clearData
                 << " corruptByte=" << _corruptByte
                 << " byte=" << _byteOffset
                 << " corruptItems=" << _corruptItems
                 << " updateCRC=" << _updateCRC );
   }
   else
   {
      DLOG_ERROR( "Error reading: " << datFilePath << " err=" << datfile.Error() );
   }

}

////////////////////////////////////////////////////////////////////////////////
//
// Convenience macros for adding named fields to the ItemMap

// Compute ptrdiff_t offset from structure xPFR to xPFR.field
#define PFR_OFFSET( xPFR, field )                                   \
   (ptrdiff_t)((char *)&(xPFR.field) - (char *)&(xPFR))

// Compute ptrdiff_t offset from structure xPFR to xPFR.PFR_DonorVitals.field
#define PFR_OFFSET_DV( xPFR, field )                                \
   (ptrdiff_t)((char *)&(xPFR.PFR_DonorVitals.field) - (char *)&(xPFR))

// Add a non-SDonorVitals field to _pfrItems
#define ADD_ITEM( vtType, xPFR, field )                             \
   _pfrItems[ #field ] = Item( Item::vtType,                        \
                               PFR_OFFSET(xPFR, field),             \
                               sizeof(xPFR.field) )

// Add a SDonorVitals field to _pfrItems
#define ADD_ITEM_DV( vtType, xPFR, field )                          \
   _pfrItems[ #field ] = Item( Item::vtType,                        \
                               PFR_OFFSET_DV(xPFR, field),          \
                               sizeof(xPFR.PFR_DonorVitals.field) )

// Defines the supported fields that can be set in the DAT file.
// To add new ones, use the structure member fields defined in
// the SPFRecovery and SDonorVitals structures.
//
// Control PFR data is not supported as it does not use a single data structure,
// but the rather the CDS framework, which streams PFR data elements to/from the
// saved file image.
//
void Corrupter::initItemMap()
{
   SPFRecovery spf; // for computing offsets

   // These are the supported PFR fields from SPFRecovery.SDonorVitals
   ADD_ITEM_DV( vtCHAR,  spf, cDonorVitalsStatus );
   ADD_ITEM_DV( vtFLOAT, spf, fDonorWeight );
   ADD_ITEM_DV( vtFLOAT, spf, fDonorHct );
   ADD_ITEM_DV( vtENUM,  spf, DonorBloodType );
   ADD_ITEM_DV( vtFLOAT, spf, fProcDonorTBV );
   ADD_ITEM_DV( vtFLOAT, spf, fSafetyDonorTBV );

   // These are the supported PFR fields from SPFRecovery
   ADD_ITEM( vtBOOL,  spf, PFRSafetyAlarmInProgress );
   ADD_ITEM( vtLONG,  spf, PFR_SystemState );
   ADD_ITEM( vtCHAR,  spf, PFR_cDonorConnectedState );
   ADD_ITEM( vtFLOAT, spf, PFR_fAccumInletVolumeAtHighRatio );
   ADD_ITEM( vtBOOL,  spf, PFR_bTBVLockOut );
   ADD_ITEM( vtFLOAT, spf, PFR_fSafetyDonorTBV_liters );

}

////////////////////////////////////////////////////////////////////////////////


bool Corrupter::corruptControl()
{
   bool okay = true;

   const size_t maxBytes = trimaSysControlPFSaveArea.nSecs * 512;

   // clearData option takes precedence over any other corruption option
   if ( _clearData )
   {
      okay = loadControl();
      if ( okay )
      {
         memset( _ControlPFData, 0, maxBytes );
         DLOG_INFO( "Cleared Control PFR Data" );
      }
   }
   else if ( _corruptByte )
   {
      okay = ( loadControl() &&
               flipByte( _ControlPFData, _byteOffset, maxBytes ) );
   }

   return okay;
}


bool Corrupter::loadControl()
{
   //
   // Only load the data once
   //
   if ( ! _ControlPFData )
   {
      //
      // For Control's PFR data, we use VxWorks' RAW I/O API
      //
      const size_t nBytes = trimaSysControlPFSaveArea.nSecs * 512;
      _ControlPFData = new unsigned char[nBytes];

      trimaSysControlPFSaveArea.pBuf = (char *)_ControlPFData;
      trimaSysControlPFSaveArea.direction = 0;

      if( ataRawio( 0, 0, &trimaSysControlPFSaveArea ) == ERROR )
      {
         DLOG_ERROR( "ataRawio() failed to read Control PFR data" );
         delete _ControlPFData;
         _ControlPFData = NULL;
      }
   }

   return ( _ControlPFData != NULL );
}


bool Corrupter::saveControl()
{
   bool okay = true;

   if ( _ControlPFData )
   {
      //
      // Write corrupted data back
      //
      trimaSysControlPFSaveArea.direction = 1;
      if ( ataRawio( 0, 0, &trimaSysControlPFSaveArea ) == ERROR )
      {
         DLOG_ERROR( "ataRawio() failed to write control power fail data" );
         okay = false;
      }
      else
      {
         DLOG_INFO( "Wrote Control PFR data" );
      }
   }

   return okay;
}


bool Corrupter::updateControlCRC()
{
   // Return quietly if the Control PFR data is not loaded
   bool okay = loadControl();
   if ( ! okay ) return false;

   //
   // PFRHeader is a clone of PowerFailSaveHeader (proc/interface/procpfr.h)
   //
   struct PFRHeader
   {
      unsigned int  writeSize;
      time_t        writeTime;
   };

   // Get PFR data length by peeking at header and subtracting the CRC bytes
   unsigned long myCRC = 0;
   PFRHeader * pHeader = (PFRHeader *)_ControlPFData;
   if ( pHeader->writeSize < sizeof(myCRC) )
   {
      DLOG_ERROR( "BAD Control PFR header? writeSize=" << pHeader->writeSize );
      return false;
   }

   const long dataSize = pHeader->writeSize - sizeof(myCRC);

   // Compute the new CRC
   crcgen32( &myCRC, &_ControlPFData[sizeof(PFRHeader)], dataSize );

   // Extract the original CRC
   unsigned long * pCRC = (unsigned long*)(_ControlPFData+sizeof(PFRHeader)+dataSize);
   const unsigned long origCRC = *pCRC;

   // Compare the two
   if ( origCRC == myCRC )
   {
      DLOG_INFO( "Control PFR CRC unchanged: dataSize=" << dataSize << " origCRC=0x" << hex << origCRC << dec );
   }
   else
   {
      DLOG_INFO( "Updating Control PFR CRC: old=0x" << hex << origCRC << " new=0x" << myCRC << dec );
      *pCRC = myCRC;
   }

   return okay;
}


////////////////////////////////////////////////////////////////////////////////


bool Corrupter::corruptSafety()
{
   bool okay = true;

   // clearData option takes precedence over any other corruption option
   if ( _clearData )
   {
      // just delete the file
      if( remove( SAFETY_PFSAVE_FILE ) == ERROR )
      {
         DLOG_ERROR( "remove() failed on: " << SAFETY_PFSAVE_FILE );
         okay = false;
      }
      else
      {
         DLOG_INFO( "Safety PFR data removed." );
      }
   }
   else if ( _corruptByte )
   {
      okay = ( loadSafety() &&
               flipByte( _SafetyPFData, _byteOffset, sizeof(SPFRecovery) ) );
   }

   //
   // Check for named items to corrupt (only supported for Safety PFR)
   //
   if ( _corruptItems && loadSafety() )
   {
      unsigned char * pBeg = (unsigned char *)_SafetyPFData;
      ItemMap::iterator it;
      for ( it = _pfrItems.begin(); it != _pfrItems.end(); ++it )
      {
         // dataOffset was initialized for valid/supported items in dat file
         if ( it->second.dataOffset >= 0 )
         {
            DLOG_INFO( "Setting: field=" << it->first << " @ offset=" << it->second.dataOffset );
            memcpy( pBeg + it->second.dataOffset,
                    it->second.itemValue.bytes,
                    it->second.itemSize );
         }
      }
   }

   return okay;
}


bool Corrupter::loadSafety()
{
   //
   // Only load the data once
   //
   if ( ! _SafetyPFData )
   {
      //
      // For Safety's PFR data, we use standard file I/O API
      //
      int fd = open( SAFETY_PFSAVE_FILE, O_RDONLY, 0 );
      if ( fd == ERROR )
      {
         DLOG_ERROR( "open() failed on: " << SAFETY_PFSAVE_FILE );
      }
      else
      {
         // read the data into our data structure
         _SafetyPFData = new SPFRecovery();
         int nBytes = read( fd, (char *)_SafetyPFData, sizeof(SPFRecovery) );
         if ( nBytes != sizeof(SPFRecovery) )
         {
            DLOG_ERROR( "read() failed on: " << SAFETY_PFSAVE_FILE );
            delete _SafetyPFData;
            _SafetyPFData = NULL;
         }
         close( fd );
      }
   }

   return ( _SafetyPFData != NULL );
}


bool Corrupter::saveSafety()
{
   // Return quietly if nothing to save
   if ( ! _SafetyPFData ) return true;

   bool okay = true;

   int fd = open( SAFETY_PFSAVE_FILE, O_RDWR, 0 );
   if ( fd == ERROR )
   {
      DLOG_ERROR( "open() failed on: " << SAFETY_PFSAVE_FILE );
      okay = false;
   }
   else
   {
      int nBytes = write( fd, (char *)_SafetyPFData, sizeof(SPFRecovery) );
      if ( nBytes != sizeof(SPFRecovery) )
      {
         DLOG_ERROR( "write() failed on: " << SAFETY_PFSAVE_FILE );
         okay = false;
      }
      else
      {
         DLOG_INFO( "Wrote Safety PFR data: " << SAFETY_PFSAVE_FILE );
      }
      close( fd );
   }
   return okay;
}


bool Corrupter::updateSafetyCRC()
{
   // Return quietly if the Safety PFR data is not loaded
   bool okay = loadSafety();
   if ( ! okay ) return false;

   unsigned long myCRC = 0;
   const size_t sizePF = sizeof(SPFRecovery);
   const long dataSize = sizePF - sizeof(myCRC);  // CRC is stored at the end

   // Compute the new CRC
   crcgen32( &myCRC, (const unsigned char *)_SafetyPFData, dataSize );

   // Compare the two
   if ( myCRC == _SafetyPFData->PFRDataCRC )
   {
      DLOG_INFO( "Safety PFR CRC unchanged: old=0x" << hex << _SafetyPFData->PFRDataCRC << dec );
   }
   else
   {
      DLOG_INFO( "Updating Safety PFR CRC: old=0x" << hex << _SafetyPFData->PFRDataCRC << " new=0x" << myCRC << dec );
      _SafetyPFData->PFRDataCRC = myCRC;
   }

   return okay;
}


bool Corrupter::flipByte(void* buffer, int offset, size_t max_bytes)
{
   bool okay = (buffer != NULL);

   if ( offset < 0 || offset > max_bytes )
   {
      DLOG_ERROR( "invalid offset=" << _byteOffset << " exceeds max=" << max_bytes );
      okay = false;
   }

   if ( okay )
   {
      unsigned char *ubytes = (unsigned char*) buffer;
      unsigned char oldData = ubytes[offset];

      // flip the lsb of the specified byte
      unsigned char newData = oldData ^ 1;

      DLOG_INFO( "Flipping PFData byte=" << offset
                 << ": oldData=0x" << hex << (unsigned int) oldData
                 << " newData=0x" << (unsigned int) newData << dec );

      ubytes[offset] = newData;
   }
   return okay;
}


void Corrupter::printControl()
{
   DLOG_INFO( __FUNCTION__ << "(): Not implemented" );
}

//
// Convenience macros for logging Safety PFR fields
//
#define DLOG_SAFETY_FIELD( field ) \
   DLOG_INFO( PREFIX << #field "=" << _SafetyPFData->field )

#define DLOG_SAFETY_FIELD_HEX( field ) \
   DLOG_INFO( PREFIX << #field "=0x" << hex << (unsigned)_SafetyPFData->field << dec )

#define DLOG_SAFETY_TIMER( timespec ) \
   DLOG_INFO( PREFIX << #timespec "=" << _SafetyPFData->timespec.tv_sec << " secs" )

#define DLOG_SAFETY_DVITALS( field ) \
   DLOG_INFO( PREFIX << #field "=" << _SafetyPFData->PFR_DonorVitals.field )

#define DLOG_SAFETY_DVITALS_HEX( field ) \
   DLOG_INFO( PREFIX << #field "=0x" << hex << (unsigned)_SafetyPFData->PFR_DonorVitals.field << dec )

void Corrupter::printSafety()
{
   static const string PREFIX( "SafetyPFRData: " );  // used by DLOG_SAFETY_XXX()

   DLOG_INFO( "sizeof(SPFRecovery)=" << sizeof(SPFRecovery) << " sizeof(bool)=" << sizeof(bool) );

   if ( loadSafety() )
   {
      DLOG_SAFETY_DVITALS_HEX( cDonorVitalsStatus );
      DLOG_SAFETY_DVITALS_HEX( cDonorInfoStatus );
      DLOG_SAFETY_DVITALS_HEX( cDonorDownloadStatus );
      DLOG_SAFETY_DVITALS_HEX( cDonorSex );
      DLOG_SAFETY_DVITALS_HEX( cDonorHeightUnits );
      DLOG_SAFETY_DVITALS_HEX( cDonorWeightUnits );
      DLOG_SAFETY_DVITALS_HEX( cDonorRbcCount );
//    DLOG_SAFETY_DVITALS( cDonorDateofBirth );  // !! DO NOT LOG !!
//    DLOG_SAFETY_DVITALS( cDonorName );         // !! DO NOT LOG !!
      DLOG_SAFETY_DVITALS( fDonorWeight );
      DLOG_SAFETY_DVITALS( fDonorHeight );
      DLOG_SAFETY_DVITALS( fDonorHct );
      DLOG_SAFETY_DVITALS( fDonorPltPrecount );
      DLOG_SAFETY_DVITALS( fDonorSampleVolume );
      DLOG_SAFETY_DVITALS( fProcDonorTBVOgawa );
      DLOG_SAFETY_DVITALS( fProcDonorTBV );
      DLOG_SAFETY_DVITALS( fSafetyDonorTBV );
      DLOG_SAFETY_DVITALS_HEX( DonorDataCRC );
      DLOG_SAFETY_DVITALS( DonorBloodType );

      DLOG_SAFETY_FIELD( PFR_fSafetyDonorTBV_liters );
      DLOG_SAFETY_FIELD( PFR_fECLimit_ml );
      DLOG_SAFETY_FIELD( PFR_fDonorHct_decimal );
      DLOG_SAFETY_FIELD( PFR_fDonorPltPrecount );
      DLOG_SAFETY_FIELD( PFR_fDonorPlasmaFraction );
      DLOG_SAFETY_FIELD_HEX( PFR_cDonorConnectedState );
      DLOG_SAFETY_FIELD_HEX( PFR_cDonorVitalStatus );
	  DLOG_SAFETY_FIELD( PFR_bTBVLockOut );

      DLOG_SAFETY_FIELD( PFTimeOfFailure );
      DLOG_SAFETY_FIELD( PFRSafetyOneShot );
      DLOG_SAFETY_FIELD( PFRSafetyAlarmInProgress );
      DLOG_SAFETY_FIELD( PFRdonorConnectMode );
      DLOG_SAFETY_FIELD( PFR_SetType );
      DLOG_SAFETY_FIELD( PFR_SystemState );
      DLOG_SAFETY_FIELD( PFRanInlet_lAccumRevs );
      DLOG_SAFETY_FIELD( PFRanInlet_fDeltaVolSet );
      DLOG_SAFETY_FIELD( PFRanInlet_fSV );
      DLOG_SAFETY_FIELD( PFRanInlet_lAccumHalls );
      DLOG_SAFETY_FIELD( PFRaReturn_lAccumRevs );
      DLOG_SAFETY_FIELD( PFRaReturn_fDeltaVolSet );
      DLOG_SAFETY_FIELD( PFRaReturn_fSV );
      DLOG_SAFETY_FIELD( PFRaReturn_lAccumHalls );
      DLOG_SAFETY_FIELD( PFRanAC_lAccumRevs );
      DLOG_SAFETY_FIELD( PFRanAC_fDeltaVolSet );
      DLOG_SAFETY_FIELD( PFRanAC_fSV );
      DLOG_SAFETY_FIELD( PFRanAC_lAccumHalls );
      DLOG_SAFETY_FIELD( PFRaPlatelet_lAccumRevs );
      DLOG_SAFETY_FIELD( PFRaPlatelet_fDeltaVolSet );
      DLOG_SAFETY_FIELD( PFRaPlatelet_fSV );
      DLOG_SAFETY_FIELD( PFRaPlatelet_lAccumHalls );
      DLOG_SAFETY_FIELD( PFRaPlasma_lAccumRevs );
      DLOG_SAFETY_FIELD( PFRaPlasma_fDeltaVolSet );
      DLOG_SAFETY_FIELD( PFRaPlasma_fSV );
      DLOG_SAFETY_FIELD( PFRaPlasma_lAccumHalls );

      DLOG_SAFETY_FIELD( PFR_bLookingForFirstFluid );
      DLOG_SAFETY_FIELD( PFR_bPauseMonitoringActive );
      DLOG_SAFETY_FIELD( PFR_cCycleMon );
      DLOG_SAFETY_FIELD_HEX( PFR_cSNCycle );
      DLOG_SAFETY_FIELD( PFR_pumpTimerInletVolumeSnap );
      DLOG_SAFETY_FIELD( PFR_inMeteredStorage );
      DLOG_SAFETY_FIELD( PFRInlet_pumpErrorLogFlag );
      DLOG_SAFETY_FIELD( PFRInlet_monType );
      DLOG_SAFETY_FIELD( PFRReturn_pumpErrorLogFlag );
      DLOG_SAFETY_FIELD( PFRReturn_monType );
      DLOG_SAFETY_FIELD( PFRReturn_fReturnVolumeAwayFromDonor );
      DLOG_SAFETY_FIELD( PFRReturn_fReturnPrimeVolume );
      DLOG_SAFETY_FIELD( PFRAC_pumpErrorLogFlag );
      DLOG_SAFETY_FIELD( PFRAC_monType );
      DLOG_SAFETY_FIELD( PFRAC_fACPrimeVolume );
      DLOG_SAFETY_FIELD( PFRPlatelet_pumpErrorLogFlag );
      DLOG_SAFETY_FIELD( PFRPlatelet_monType );
      DLOG_SAFETY_FIELD( PFRPlasma_pumpErrorLogFlag );
      DLOG_SAFETY_FIELD( PFRPlasma_monType );
      DLOG_SAFETY_FIELD( PFRRBC_pumpErrorLogFlag );
      DLOG_SAFETY_FIELD( PFRRBC_monType );
      DLOG_SAFETY_FIELD( PFRInlet_initialVolume );
      DLOG_SAFETY_FIELD( PFRInlet_maximumVolume );
      DLOG_SAFETY_FIELD( PFRInlet_minimumVolume );
      DLOG_SAFETY_FIELD( PFRInlet_initialVolumeKnown );
      DLOG_SAFETY_FIELD( PFRInlet_maximumVolumeKnown );
      DLOG_SAFETY_FIELD( PFRInlet_minimumVolumeKnown );
      DLOG_SAFETY_FIELD( PFRInlet_bagflowIO );
      DLOG_SAFETY_FIELD( PFRInlet_bagvalveIO );
      DLOG_SAFETY_FIELD( PFRInlet_accumulatedVolume );
      DLOG_SAFETY_FIELD( PFRInlet_deltaVolume );
      DLOG_SAFETY_FIELD( PFRInlet_drawCycleVolume );
      DLOG_SAFETY_FIELD( PFRInlet_returnCycleVolume );
      DLOG_SAFETY_FIELD( PFRInlet_drawCycleVolumeInfusion );
      DLOG_SAFETY_FIELD( PFRInlet_returnCycleVolumeInfusion );
      DLOG_SAFETY_FIELD( PFRReturn_initialVolume );
      DLOG_SAFETY_FIELD( PFRReturn_maximumVolume );
      DLOG_SAFETY_FIELD( PFRReturn_minimumVolume );
      DLOG_SAFETY_FIELD( PFRReturn_initialVolumeKnown );
      DLOG_SAFETY_FIELD( PFRReturn_maximumVolumeKnown );
      DLOG_SAFETY_FIELD( PFRReturn_minimumVolumeKnown );
      DLOG_SAFETY_FIELD( PFRReturn_bagflowIO );
      DLOG_SAFETY_FIELD( PFRReturn_bagvalveIO );
      DLOG_SAFETY_FIELD( PFRReturn_accumulatedVolume );
      DLOG_SAFETY_FIELD( PFRReturn_deltaVolume );
      DLOG_SAFETY_FIELD( PFRReturn_drawCycleVolume );
      DLOG_SAFETY_FIELD( PFRReturn_returnCycleVolume );
      DLOG_SAFETY_FIELD( PFRReturn_drawCycleVolumeInfusion );
      DLOG_SAFETY_FIELD( PFRReturn_returnCycleVolumeInfusion );
      DLOG_SAFETY_FIELD( PFRAC_initialVolume );
      DLOG_SAFETY_FIELD( PFRAC_maximumVolume );
      DLOG_SAFETY_FIELD( PFRAC_minimumVolume );
      DLOG_SAFETY_FIELD( PFRAC_initialVolumeKnown );
      DLOG_SAFETY_FIELD( PFRAC_maximumVolumeKnown );
      DLOG_SAFETY_FIELD( PFRAC_minimumVolumeKnown );
      DLOG_SAFETY_FIELD( PFRAC_bagflowIO );
      DLOG_SAFETY_FIELD( PFRAC_bagvalveIO );
      DLOG_SAFETY_FIELD( PFRAC_accumulatedVolume );
      DLOG_SAFETY_FIELD( PFRAC_deltaVolume );
      DLOG_SAFETY_FIELD( PFRAC_drawCycleVolume );
      DLOG_SAFETY_FIELD( PFRAC_returnCycleVolume );
      DLOG_SAFETY_FIELD( PFRAC_drawCycleVolumeInfusion );
      DLOG_SAFETY_FIELD( PFRAC_returnCycleVolumeInfusion );
      DLOG_SAFETY_FIELD( PFRPlasma_initialVolume );
      DLOG_SAFETY_FIELD( PFRPlasma_maximumVolume );
      DLOG_SAFETY_FIELD( PFRPlasma_minimumVolume );
      DLOG_SAFETY_FIELD( PFRPlasma_initialVolumeKnown );
      DLOG_SAFETY_FIELD( PFRPlasma_maximumVolumeKnown );
      DLOG_SAFETY_FIELD( PFRPlasma_minimumVolumeKnown );
      DLOG_SAFETY_FIELD( PFRPlasma_bagflowIO );
      DLOG_SAFETY_FIELD( PFRPlasma_bagvalveIO );
      DLOG_SAFETY_FIELD( PFRPlasma_accumulatedVolume );
      DLOG_SAFETY_FIELD( PFRPlasma_deltaVolume );
      DLOG_SAFETY_FIELD( PFRPlasma_drawCycleVolume );
      DLOG_SAFETY_FIELD( PFRPlasma_returnCycleVolume );
      DLOG_SAFETY_FIELD( PFRPlasma_drawCycleVolumeInfusion );
      DLOG_SAFETY_FIELD( PFRPlasma_returnCycleVolumeInfusion );
      DLOG_SAFETY_FIELD( PFRPlatelet_initialVolume );
      DLOG_SAFETY_FIELD( PFRPlatelet_maximumVolume );
      DLOG_SAFETY_FIELD( PFRPlatelet_minimumVolume );
      DLOG_SAFETY_FIELD( PFRPlatelet_initialVolumeKnown );
      DLOG_SAFETY_FIELD( PFRPlatelet_maximumVolumeKnown );
      DLOG_SAFETY_FIELD( PFRPlatelet_minimumVolumeKnown );
      DLOG_SAFETY_FIELD( PFRPlatelet_bagflowIO );
      DLOG_SAFETY_FIELD( PFRPlatelet_bagvalveIO );
      DLOG_SAFETY_FIELD( PFRPlatelet_accumulatedVolume );
      DLOG_SAFETY_FIELD( PFRPlatelet_deltaVolume );
      DLOG_SAFETY_FIELD( PFRPlatelet_drawCycleVolume );
      DLOG_SAFETY_FIELD( PFRPlatelet_returnCycleVolume );
      DLOG_SAFETY_FIELD( PFRPlatelet_drawCycleVolumeInfusion );
      DLOG_SAFETY_FIELD( PFRPlatelet_returnCycleVolumeInfusion );
      DLOG_SAFETY_FIELD( PFRRBC_initialVolume );
      DLOG_SAFETY_FIELD( PFRRBC_maximumVolume );
      DLOG_SAFETY_FIELD( PFRRBC_minimumVolume );
      DLOG_SAFETY_FIELD( PFRRBC_initialVolumeKnown );
      DLOG_SAFETY_FIELD( PFRRBC_maximumVolumeKnown );
      DLOG_SAFETY_FIELD( PFRRBC_minimumVolumeKnown );
      DLOG_SAFETY_FIELD( PFRRBC_bagflowIO );
      DLOG_SAFETY_FIELD( PFRRBC_bagvalveIO );
      DLOG_SAFETY_FIELD( PFRRBC_accumulatedVolume );
      DLOG_SAFETY_FIELD( PFRRBC_deltaVolume );
      DLOG_SAFETY_FIELD( PFRRBC_drawCycleVolume );
      DLOG_SAFETY_FIELD( PFRRBC_returnCycleVolume );
      DLOG_SAFETY_FIELD( PFRRBC_drawCycleVolumeInfusion );
      DLOG_SAFETY_FIELD( PFRRBC_returnCycleVolumeInfusion );
      DLOG_SAFETY_FIELD( PFRPlasma_positionC );
      DLOG_SAFETY_FIELD( PFRPlasma_positionS );
      DLOG_SAFETY_FIELD( PFRPlasma_iT1Status );
      DLOG_SAFETY_FIELD( PFRPlasma_bValvePositionMonitorEnabled );
      DLOG_SAFETY_FIELD( PFRPlatelet_positionC );
      DLOG_SAFETY_FIELD( PFRPlatelet_positionS );
      DLOG_SAFETY_FIELD( PFRPlatelet_iT1Status );
      DLOG_SAFETY_FIELD( PFRPlatelet_bValvePositionMonitorEnabled );
      DLOG_SAFETY_FIELD( PFRRBC_positionC );
      DLOG_SAFETY_FIELD( PFRRBC_positionS );
      DLOG_SAFETY_FIELD( PFRRBC_iT1Status );
      DLOG_SAFETY_FIELD( PFRRBC_bValvePositionMonitorEnabled );
      DLOG_SAFETY_FIELD( PFRCassette_position );
      DLOG_SAFETY_FIELD( PFRCassette_cDeviceConstraint );
      DLOG_SAFETY_FIELD( PFRCassette_forceInitialize );
      DLOG_SAFETY_FIELD( PFRNeedle_monType );
      DLOG_SAFETY_FIELD( PFRNeedle_startTime );
      DLOG_SAFETY_FIELD( PFRNeedle_duration );
      DLOG_SAFETY_FIELD( PFRNeedle_timerHasFired );
      DLOG_SAFETY_FIELD( PFRNeedle_pumpTimerInletAccumulator );
      DLOG_SAFETY_FIELD( PFRNeedle_pumpTimerReturnAccumulator );
      DLOG_SAFETY_FIELD( PFR_fAccumInletVolumeAtHighRatio );
      DLOG_SAFETY_FIELD( PFR_fAccumACVolumeAtLowRatio );
      DLOG_SAFETY_FIELD( PFR_bUseableACRatio );
      DLOG_SAFETY_FIELD( PFR_fAvgACRatio );
      DLOG_SAFETY_FIELD( PFR_fLastKnownGoodRatio );
      DLOG_SAFETY_FIELD( PFR_acRatioA_bACRatioMonitoringActive );
      DLOG_SAFETY_FIELD( PFR_acRatioA_monType );
      DLOG_SAFETY_FIELD( PFR_fAccumACTooFastVolume );
      DLOG_SAFETY_FIELD( PFR_iHighRatioPersistsCounter );
      DLOG_SAFETY_FIELD( PFR_acRatioP_bACRatioMonitoringActive );
      DLOG_SAFETY_FIELD( PFR_acRatioP_monType );
      DLOG_SAFETY_FIELD( PFR_iHighRatioCounter );
      DLOG_SAFETY_FIELD( PFR_iLowRatioCounter );
      DLOG_SAFETY_FIELD( PFR_acRatioI_monType );
      DLOG_SAFETY_FIELD( PFR_acRatioI_bACRatioMonitoringActive );
      DLOG_SAFETY_FIELD( PFR_faccumDrawCycleInletRBCCollect );
      DLOG_SAFETY_FIELD( PFR_faccumReturnCycleInletRBCCollect );
      DLOG_SAFETY_FIELD( PFR_faccumCollectedRBCs );
      DLOG_SAFETY_FIELD( PFR_faccumACToPlateletBag );
      DLOG_SAFETY_FIELD( PFR_faccumACToPlasmaBag );
      DLOG_SAFETY_FIELD( PFR_faccumACToRBCBag );
      DLOG_SAFETY_FIELD( PFR_faccumSetVolume );
      DLOG_SAFETY_FIELD( PFR_fadjustedDonorHCT );
      DLOG_SAFETY_TIMER( PFR_drawCycleTimer );
      DLOG_SAFETY_TIMER( PFR_returnCycleTimer );
      DLOG_SAFETY_FIELD( PFR_fdrawCycleTime );
      DLOG_SAFETY_FIELD( PFR_freturnCycleTime );
      DLOG_SAFETY_FIELD( PFR_fACInfusionPerTBV );
      DLOG_SAFETY_FIELD( PFR_fACInfusionPerTBV_2 );
      DLOG_SAFETY_FIELD( PFR_fACInfusionPerTBV_3 );
      DLOG_SAFETY_FIELD( PFR_acInfusC_bACInfusionMonitoringActive );
      DLOG_SAFETY_FIELD( PFR_acInfusC_monType );
      DLOG_SAFETY_FIELD( PFR_faccumACDuringRBAndDisconnect );
      DLOG_SAFETY_TIMER( PFR_returnPumpTimer );
      DLOG_SAFETY_TIMER( PFR_rinsebackTimer );
      DLOG_SAFETY_TIMER( PFR_valveOpenTimer );
      DLOG_SAFETY_FIELD( PFR_valveOpenAccumulatedTime );
      DLOG_SAFETY_FIELD( PFR_iRBHighInfusionCounter );
      DLOG_SAFETY_FIELD( PFR_acInfusR_bACInfusionMonitoringActive );
      DLOG_SAFETY_FIELD( PFR_acInfusR_bPlasmaValveOpenTooLong );
      DLOG_SAFETY_FIELD( PFR_acInfusR_monType );
      DLOG_SAFETY_FIELD( PFR_faccumAKOVolToDonor );
      DLOG_SAFETY_FIELD( PFR_flastReturnToDonorDuringDraw );
      DLOG_SAFETY_TIMER( PFR_AKODrawCycleTimer );
      DLOG_SAFETY_FIELD( PFR_acInfusA_bACInfusionMonitoringActive );
      DLOG_SAFETY_FIELD( PFR_acInfusA_blimitReturnVolumeOnly );
      DLOG_SAFETY_FIELD( PFR_acInfusA_monType );
      DLOG_SAFETY_FIELD( PFR_faccumVolume );
      DLOG_SAFETY_FIELD( PFR_facPrimeVolume );
      DLOG_SAFETY_FIELD( PFR_firstCycleMinVolumeAllowed );
      DLOG_SAFETY_FIELD( PFR_firstCycleMaxVolumeAllowed );
      DLOG_SAFETY_FIELD( PFR_hypovolC_bHypovolemiaMonitoringActive );
      DLOG_SAFETY_FIELD( PFR_hypovolC_monType );
      DLOG_SAFETY_FIELD( PFR_hypovolC_PrevMonType );
      DLOG_SAFETY_FIELD( PFR_faccumECVolume );
      DLOG_SAFETY_FIELD( PFR_iRBValveBadPositionCounter );
      DLOG_SAFETY_FIELD( PFR_hypovolD_bHypovolemiaMonitoringActive );
      DLOG_SAFETY_FIELD( PFR_hypovolD_bHypervolemiaMonitoringActive );
      DLOG_SAFETY_FIELD( PFR_hypovolD_monType );
      DLOG_SAFETY_FIELD( PFR_fWBProcessedDuringRBCCollect );
      DLOG_SAFETY_FIELD( PFR_fWBProcessedNotDuringRBCCollect );
      DLOG_SAFETY_FIELD( PFR_compdepl_bDepletionMonitoringActive );
      DLOG_SAFETY_FIELD( PFR_compdepl_bPlateletCollectionStarted );
      DLOG_SAFETY_FIELD( PFR_compdepl_monType );
      DLOG_SAFETY_FIELD_HEX( PFRDataCRC );
   }
}

////////////////////////////////////////////////////////////////////////////////

extern "C" int corrupt_main( const char* args )
{
   Corrupter badGuy( PNAME_CORRUPT_DAT );
   return ( badGuy.execute() == true ? 0 : 1 );
}


////////////////////////////////////////////////////////////////////////////////
//
// Some enum have stream operators that are implemented in libraries that aren't
// convenient to get to so implement clones of them here.

#define STREAM_ENUM( os, enumTag )       \
   case enumTag: os << #enumTag ; break;

#define STREAM_UNKNOWN( os )             \
   default: os << "UNKNOWN"; break;

DataLog_Stream & operator << (DataLog_Stream &os, MON_TYPE monType)
{
   switch (monType)
   {
   STREAM_ENUM( os, MON_INACTIVE );
   STREAM_ENUM( os, MON_FIRST_DRAW );
   STREAM_ENUM( os, MON_RUN_CYCLES );
   STREAM_ENUM( os, MON_RINSEBACK );
   STREAM_ENUM( os, MON_FIRST_DRAW_COMPLETE );
   STREAM_ENUM( os, MON_DISCONNECT );
   STREAM_UNKNOWN( os );
   }
   return os;
}

DataLog_Stream & operator << (DataLog_Stream &os, FLOW_DIRECTION flowIO)
{
   os << FLOW_DIRECTION_STRING( flowIO ); // see s_set.hpp
   return os;
}

DataLog_Stream & operator << (DataLog_Stream &os, HW_VALVE_STATES valveIO)
{
   switch (valveIO)
   {
   STREAM_ENUM( os, HW_VALVE_UNKNOWN );
   STREAM_ENUM( os, HW_VALVE_COLLECT );
   STREAM_ENUM( os, HW_VALVE_OPEN );
   STREAM_ENUM( os, HW_VALVE_RETURN );
   STREAM_UNKNOWN( os );
   }
   return os;
}

DataLog_Stream & operator << (DataLog_Stream &os, HW_CASSETTE_STATES position)
{
   switch (position)
   {
   STREAM_ENUM( os, HW_CASSETTE_UNKNOWN );
   STREAM_ENUM( os, HW_CASSETTE_DOWN );
   STREAM_ENUM( os, HW_CASSETTE_UP );
   STREAM_UNKNOWN( os );
   }
   return os;
}

