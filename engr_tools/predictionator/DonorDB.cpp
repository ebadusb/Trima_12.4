// DonorDB.cpp
#include "DonorDB.h"


DonorDB::DonorDB() :
   m_donor_number(0),
   m_total_donors(0)
{
}

DonorDB::~DonorDB()
{
   m_DonorDBStream.close();
}

const int DonorDB :: Initialize( const char* pszPath )
{
   if (m_DonorDBStream.is_open()) {
      m_DonorDBStream.close();
   }
   m_DonorDBStream.open( pszPath, std::ios::in );
   if( m_DonorDBStream.fail() )
   {
      // msg box for user
      return( -1 );
   }

   m_DonorDBStream >> m_total_donors;
   m_donor_number = 0;
   return( m_total_donors );
}

const int DonorDB :: GetNextDonor( float &fWeight
                                   , float &fTBV
                                   , float &fHct
                                   , float &fPrecount
                                   , BLOODTYPE   &iBloodType )
{
   if (m_DonorDBStream.eof()) return 0;
   if (m_donor_number >= m_total_donors) return 0;

   m_DonorDBStream >> fHct >> fPrecount >> fTBV;

   //
   //
   //   Our donor db does not contain weight or blood type.  These are
   //   dummied in for now but will eventually need to be returned (and
   //   weight units will need to be determined).
   //
   fWeight = 200.0f;
   iBloodType = BT_UNKNOWN;
   return ++m_donor_number;
}
