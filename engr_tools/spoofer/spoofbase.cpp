// FILENAME: spoofbase.cpp
// $Header: H:/BCT_Development/Trima5.1x/Trima/engr_tools/spoofer/rcs/spoofbase.cpp 6.2 2002/11/05 18:00:15Z td07711 Exp kgreusb $
// PURPOSE: base portion of spoof objects
// CHANGELOG:
//   $Log: spoofbase.cpp $
//   Revision 6.2  2002/11/05 18:00:15Z  td07711
//   replace parameterized constructor with default and init()
//   Revision 6.1  2002/10/23 20:45:07  td07711
//   add static SpooferAPI*
//   Revision 1.1  2002/10/22 23:17:16  td07711
//   Initial revision


#include "spooferapi.hpp"
#include "spoofbase.hpp"
#include "spooferlog.hpp"


//
// initialize statics
//
SpooferAPI* SpoofBase :: _pApi = 0;


//
// static functions
//

void SpoofBase :: init( SpooferAPI* pApi )
{
    ASSERT( pApi );
    _pApi = pApi;
}



//
// non static functions
//


SpoofBase :: SpoofBase( )
: _name( 0 ), _comment( 0 ), _timeout( 0 ), _Var1( 0 ), _Var2( 0 )
{
}


SpoofBase :: ~SpoofBase()
{
    LOG_DEBUG("~SpoofBase: %s destroyed", name() );
}


void SpoofBase :: show() const
{
    LOG_INFO( "%s %d %s", name(), getTimeout(), comment() );
}

void SpoofBase :: run_generic(const char* Var1, const char* Var2)
{
    LOG_INFO( "No derived run_generic so ignoring the var1 and var2 values input" );
    run();
}
