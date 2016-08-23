// FILENAME: spoofedelement.cpp
// $Header: E:/BCT_Development/Trima5.R/Trima/engr_tools/spoofer/rcs/spoofedelement.cpp 6.8 2003/10/23 18:25:30Z td07711 Exp dslausb $
// PURPOSE: implements SpoofedElement class
//   manages spoofing of specific data element within a SpoofedObject
// CHANGELOG:
//   $Log: spoofedelement.cpp $
//   Revision 6.8  2003/10/23 18:25:30Z  td07711
//   IT3338 - support fastdata spoofing
//   Revision 6.7  2003/07/15 17:15:33Z  td07711
//   IT6199 - use predefined datalog_levels for logging
//   Revision 6.6  2003/03/14 22:16:56Z  td07711
//   use datalogger directly instead of stderr for a few msgs
//   Revision 6.5  2002/10/24 21:19:15Z  td07711
//   support short and char data types
//   Revision 6.4  2002/10/23 21:41:37  td07711
//   moved error checking to SpoofedObject
//   Revision 6.3  2002/10/17 23:04:44  td07711
//   error out if invalid spoof
//   Revision 6.2  2002/10/10 21:25:16  td07711
//   add showSpoofs support
//   Revision 6.1  2002/10/09 21:01:48  td07711
//   debug msgs
//   Revision 1.2  2002/10/08 23:45:21  td07711
//   rearrange some stuff for debugging corruption problem
//   Revision 1.1  2002/10/08 18:31:31  td07711
//   Initial revision


#include <string>

#include "spoofedelement.hpp"
#include "spooferlog.hpp"
#include "datalog_levels.h"

#include "spoofabledata.hpp"
#include "spoofedobject.hpp"


SpoofedElement :: SpoofedElement( SpoofedObject* pObject, const char* elementName )
: _pSpoofConstant( 0 ), _spoofOffset( 0.0 ), _spoofMult( 1.0 ),
  _pSpoofedElement( 0 ), _pOriginalElement( 0 ), _pParent( pObject )
{
    ASSERT( pObject );

    // lookup field info
    _pDataInfo = SpoofableData::get( pObject->name(), elementName );
    ASSERT( _pDataInfo );

    // calculate ptr to field to spoof
    _pSpoofedElement = pObject->spoofedBuf() + dataInfo().offset();
    _pOriginalElement = pObject->originalBuf() + dataInfo().offset();

    LOG_DEBUG("SpoofedElement: %s.%s constructed", 
              dataInfo().objectName(), dataInfo().name() );
}


SpoofedElement :: ~SpoofedElement( )
{
    LOG_DEBUG( "~SpoofedElement: destroying %s.%s",
              dataInfo().objectName(), dataInfo().name() );

    _pDataInfo = 0;
    _pSpoofedElement = 0;
    _pOriginalElement = 0;

    // JPH -- new rule... you can only delete the same type you "new"ed i.e char
    // i dont know if this cast delete works , but it compiles ;)
    delete [] (char*)_pSpoofConstant;
    _pSpoofConstant = 0;
}


void SpoofedElement :: dump( ) const
{
    // PURPOSE: dumps spoof info for this element

    dataInfo().dump();

    if( _pSpoofConstant )
    {
        LOG_INFO( "dump: spoof constant = %s", 
                 dataInfo().valueToString( _pSpoofConstant ).c_str() );
    }
    else
    {
        LOG_INFO( "dump: spoof offset = %1.2f mult = %1.2f",
                 _spoofOffset, _spoofMult );
    }
}


void SpoofedElement :: spoof( )
{
    // PURPOSE: spoofs the msg field in the parent Object's spoof buffer

    string originalValue = dataInfo().valueToString( _pOriginalElement );

    if( _pSpoofConstant )
    {
        memcpy( _pSpoofedElement, _pSpoofConstant, dataInfo().size() );
    }
    else
    {
        switch( dataInfo().type() )
        {
            case SpoofableData::INT :
            case SpoofableData::ENUM :
            {
                *((int*)_pSpoofedElement) = (int) 
                                        (*((int*)_pOriginalElement)  * _spoofMult + _spoofOffset);
                break;
            }

            case SpoofableData::FLOAT :
            {
                *((float*)_pSpoofedElement) = *((float*)_pOriginalElement)  * _spoofMult + _spoofOffset;
                break;
            }

            case SpoofableData::SHORT :
            {
                *((short*)_pSpoofedElement) = (short) 
                                        (*((short*)_pOriginalElement)  * _spoofMult + _spoofOffset);
                break;
            }

            case SpoofableData::CHAR :
            {
                *((char*)_pSpoofedElement) = (char) 
                                        (*((char*)_pOriginalElement)  * _spoofMult + _spoofOffset);
                break;
            }

	    case SpoofableData::FASTDATA_ARRAY :
	    {
		short* pOrig = (short*)_pOriginalElement;
		short* pSpoofed = (short*)_pSpoofedElement;
		short* pEnd = (short*)_pOriginalElement + dataInfo().size()/sizeof(short);    

		for( ; pOrig < pEnd; pOrig++, pSpoofed++ )
		{
		    *pSpoofed = (short)( *pOrig * _spoofMult + _spoofOffset );
		}
		break;
	    }

            default:
            {
                LOG_ERROR("spoof: %s.%s unspoofable type %d",
                          dataInfo().objectName(), dataInfo().name(), dataInfo().type() );
                break; 
            }
        }
    }

    if( SpoofedObject::isShowSpoofs() )
    {
        string spoofedValue = dataInfo().valueToString( _pSpoofedElement );

	DataLog( log_level_spoofer_info ) << dataInfo().objectName() << "." << dataInfo().name()
	    << " spoofed from " << originalValue 
	    << " to " << spoofedValue << endmsg;
    }
}


void SpoofedElement :: setSpoofValue(const char* spoofValue)
{
    // PURPOSE: converts string to appropriate data

    if( _pSpoofConstant == 0 )
    {
        _pSpoofConstant = new char[ dataInfo().size() ];
    }

    dataInfo().stringToValue( _pSpoofConstant, spoofValue );
}


void SpoofedElement :: setSpoofValue( float multiplier, float offset )
{
    // PURPOSE: sets spoof up with multiplier and offset

    _spoofMult = multiplier;
    _spoofOffset = offset;

    // JPH -- new rule... you can only delete the same type you "new"ed i.e char
    // i dont know if this cast delete works , but it compiles ;)
    delete [] (char*)_pSpoofConstant;
    _pSpoofConstant = 0; // denotes that multiplier/offset should be used
}
