// FILENAME: spoofedcdsbase.hpp
// $Header: I:/BCT_Development/vxWorks/Trima/engr_tools/spoofer/rcs/spoofedcdsbase.hpp 1.1 2002/10/11 23:30:32 td07711 Exp td07711 $
// PURPOSE: cds independent portion of SpoofedCds class
// CHANGELOG:
//   $Log: spoofedcdsbase.hpp $
//   Revision 1.1  2002/10/11 23:30:32  td07711
//   Initial revision


#ifndef SpoofedCdsBase_HPP // prevents multiple inclusion
#define SpoofedCdsBase_HPP


// common includes
#include "datastore.h"

// spoofer includes
#include "spoofedobject.hpp"


class SpoofedCdsBase : public SpoofedObject
{
    // RESPONSIBILITIES:
    //   1. defines API for dealing with templated SpoofedCds objects

    public:

        SpoofedCdsBase( const char* name );
        virtual ~SpoofedCdsBase( );

        virtual char* spoofedBuf() = 0; // returns ptr to spoofed data
        virtual char* originalBuf() = 0; // returns ptr to original data


    protected:


    private:

        SpoofedCdsBase(const SpoofedCdsBase&); // catch unauthorized use 
        SpoofedCdsBase& operator=(const SpoofedCdsBase& rhs); // catch unauthorized use
};

#endif // SpoofedCdsBase_HPP
