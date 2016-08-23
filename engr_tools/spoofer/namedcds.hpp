// FILENAME: namedcds.hpp
// $Header: H:/BCT_Development/Trima5.R/Trima/engr_tools/spoofer/rcs/namedcds.hpp 6.2 2008/01/23 17:22:41Z dslausb Exp $
// PURPOSE: provides templated cds by name
// CHANGELOG:
//   $Log: namedcds.hpp $
//   Revision 6.2  2008/01/23 17:22:41Z  dslausb
//   IT 8435 - VxWorks Checkin
//   Revision 6.1  2002/11/21 22:13:13Z  td07711
//   bugfix - remove erroneous delete


#ifndef NamedCds_HPP // prevents multiple inclusion
#define NamedCds_HPP

#include "spooferlog.hpp"

#include "spooferapi.hpp"



// CLASSNAME: NamedCds
// RESPONSIBILITIES:
//   1. implements NamedObject::send() and getData()
template < class T_cds, class T_elem >
class NamedCds : public NamedObject
{
    public:

        NamedCds( const char* name );
        virtual ~NamedCds( );


        //
        // base class function overides
        //
        void send( const void* pData, int size ); // sends data in message
        bool getData( const SpoofableData* pSpoofableData, void* buf, int timeoutMsec );
        bool getExactData( const SpoofableData* pSpoofableData, void* buf, int timeoutMsec, int matchValue );

        void init( BaseElement< T_elem >* pElement ) { _pElement = pElement; };
        T_cds& cds() { return _cds; };

    protected:


    private:

        T_cds _cds; // provides templated message interface
        T_elem _buf; // storage for Get results
        BaseElement< T_elem >* _pElement; // cds element

        NamedCds(const NamedCds&); // catch unauthorized use 
        NamedCds& operator=(const NamedCds& rhs); // catch unauthorized use
};


template < class T_cds, class T_elem >
NamedCds< T_cds, T_elem > :: NamedCds( const char* name )
: NamedObject( name ), _cds( ROLE_SPOOFER ), _pElement( 0 )
{
    // NOTE: _cds initialed ROLE_SPOOFER so that Get() will return
    // actual data, not spoofed data

    ASSERT( name );

    LOG_DEBUG( "NamedCds: %s constructed", name );
}


template < class T_cds, class T_elem >
NamedCds< T_cds, T_elem > :: ~NamedCds()
{
    _pElement = 0;

    LOG_DEBUG("~NamedCds: destroyed");
}


template < class T_cds, class T_elem >
void NamedCds< T_cds, T_elem > :: send(const void* pData, int size)
{
    // PURPOSE: sends specified data, verifies correct message size

    LOG_ERROR("send: send to a NamedCds is not supported");
}


template < class T_cds, class T_elem >
bool NamedCds< T_cds,  T_elem > :: getData( 
     const SpoofableData* pDataInfo, void* buf, int timeoutMsec )
{
    // PURPOSE: copy desired data into callers space
    // NOTE: timeoutMsec not applicable for Cds access

    ASSERT( _pElement );

    // get the cds element
    _pElement->Get( &_buf ); 

    // copy the desired data to the caller's space
    memcpy( buf, &_buf + pDataInfo->offset(), pDataInfo->size() );

    return true;
}

// added for Next Generation spoofer 
template < class T_cds, class T_elem >
bool NamedCds< T_cds,  T_elem > :: getExactData( 
     const SpoofableData* pDataInfo, void* buf, int timeoutMsec, int matchValue )
{
    // PURPOSE: copy desired data into callers space
    // NOTE: timeoutMsec not applicable for Cds access

    ASSERT( _pElement );

    // get the cds element
    _pElement->Get( &_buf ); 

    // copy the desired data to the caller's space
    memcpy( buf, &_buf + pDataInfo->offset(), pDataInfo->size() );

    return true;
}
#endif // NamedCds_HPP
