// DonorDB.h

#include <fstream>
#include "bloodtype.h"
class DonorDB
{
private:
   
protected:

   // not usable and not implemented
   //
   DonorDB(const DonorDB &){};
   DonorDB &operator=(const DonorDB &){return *this;};

   std::fstream m_DonorDBStream;
   int m_donor_number;
   int m_total_donors;

public:
   DonorDB();
   virtual ~DonorDB();
   virtual const int Initialize( const char* pszPath );
   virtual const int GetNextDonor( float &fWeight
                                   , float &fTBV
                                   , float &fHct
                                   , float &fPrecount
                                   , BLOODTYPE &iBloodType );   
};

