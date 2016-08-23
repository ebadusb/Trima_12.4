// FILENAME: spoofedcds.hpp
// $Header: I:/BCT_Development/vxWorks/Trima/engr_tools/spoofer/rcs/spoofedcds.hpp 6.5 2003/01/22 01:26:17Z td07711 Exp td07711 $
// PURPOSE: manages spoofing for a given CDS type
// CHANGELOG:
//   $Log: spoofedcds.hpp $
//   Revision 6.5  2003/01/22 01:26:17Z  td07711
//   debug msgs
//   Revision 6.4  2002/11/09 01:19:33Z  td07711
//   modify spoof caching
//   Revision 6.3  2002/10/18 23:17:00  td07711
//   add spoofing support
//   Revision 6.2  2002/10/17 23:02:01  td07711
//   mods to make callback mechanism work
//   Revision 6.1  2002/10/16 15:24:46  td07711
//   spoofedCds support


#ifndef SpoofedCds_HPP // prevents multiple inclusion
#define SpoofedCds_HPP


// common includes
#include "datastore.h"

// spoofer includes
#include "spoofedcdsbase.hpp"
#include "spoofedelement.hpp"
#include "spoofabledata.hpp"
#include "spooferlog.hpp"


// CLASSNAME: SpoofedCds
// RESPONSIBILITIES:
//   1. manages access to elements of a spoofed CDS
template < class T_cds, class T_elem >
class SpoofedCds : public SpoofedCdsBase
{
    public:

        SpoofedCds(const char* name);
        virtual ~SpoofedCds();


        //
        // base class function overides
        //
        char* spoofedBuf() { return (char*)&_sbuf; };
        char* originalBuf() { return (char*)&_obuf; };
        void init( BaseElement< T_elem >* pElement ); // sets up spoofing of the CDS element
        T_cds& cds() { return _cds; };

        //
        // installed as BaseElement _callback (datastore.h)
        //
        void cdsSpoof( void* pToFrom ); // spoofing callback

    protected:


    private:

        void spoof( );

        T_cds _cds; // provides templated CDS interface
        T_elem _obuf; // original data
        T_elem _sbuf; // spoofed data
        BaseElement< T_elem >* _pElement; // cds element
        CallbackBase* _pCallback; // spoofing callback
     
        SpoofedCds(const SpoofedCds&); // catch unauthorized use 
        SpoofedCds& operator=(const SpoofedCds& rhs); // catch unauthorized use
};



template < class T_cds, class T_elem >
SpoofedCds< T_cds, T_elem > :: SpoofedCds( const char* name) 
: SpoofedCdsBase( name ), _cds( ROLE_SPOOFER ), 
  _pElement( 0 ), _pCallback( 0 )
{
    // PURPOSE:  construct spoofable CDS by name

    ASSERT( name );

    LOG_DEBUG("SpoofedCds: %s constructed", name );
}


template < class T_cds, class T_elem >
SpoofedCds< T_cds, T_elem > :: ~SpoofedCds()
{
    // PURPOSE: destroy spoofable CDS instance

    ASSERT( _pElement );
    _pElement->ClearSpoof();
    _pElement = 0;
    
    delete _pCallback;
    _pCallback = 0;

    LOG_DEBUG( "~SpoofedCds: %s destroyed", name() );
}


template < class T_cds, class T_elem >
void SpoofedCds< T_cds, T_elem > :: init( BaseElement< T_elem >* pElement )
{
    // PURPOSE: register the spoofing callback on the CDS element

    _pElement = pElement;

    if( _pCallback == 0 )
    {
        _pCallback = new Callback< SpoofedCds< T_cds, T_elem> >( this,
                            &SpoofedCds< T_cds, T_elem >::cdsSpoof );
    }

    LOG_DEBUG( "init: installing CDS element callback for %s", name() );
    _pElement->SetSpoof( _pCallback );
}


template < class T_cds, class T_elem >
void SpoofedCds< T_cds, T_elem > :: cdsSpoof( void* pToFrom )
{
    // PURPOSE: the spoofing callback for the CDS element
    // from callback.h - using the function that takes a void* arg
    // since our spoofing function has to know where to get the original data
    // and where to put the spoofed data, it's getting passed a pair* for which
    // pair.first is a To* and pair.second is a From*.

    pair< T_elem*, const T_elem* >* pPair = (pair< T_elem*, const T_elem* >*)pToFrom;
    

    if( _spoofingEnabled == false )
    {
	*pPair->first = *pPair->second;  // copy actual cds data to caller
    }
    else // spoof the data
    {
	if ( _pElement->isSpooferCacheValid() == false || _spoofBufferNotValid )
	{
	    _pElement->setSpooferCacheValid();  // must set prior to copy
	    _obuf = *pPair->second;  // copy cdsElement to buffer

	    spoof(); // apply the spoof
	}

	// copy spoofed buffer to caller's buffer
	*pPair->first = _sbuf;
    }
}


template < class T_cds, class T_elem >
void SpoofedCds< T_cds, T_elem > :: spoof( )
{
    // copy original buffer to spoof buffer
    _sbuf = _obuf;

    spoofTheData();
}


#endif // SpoofedCds_HPP
