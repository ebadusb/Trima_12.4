// FILENAME: spoofedmsgbase.cpp
// $Header: I:/BCT_Development/vxWorks/Trima/engr_tools/spoofer/rcs/spoofedmsgbase.cpp 6.12 2002/10/11 23:28:26Z td07711 Exp td07711 $
// PURPOSE: msg type independent portion of SpoofedMsg
// CHANGELOG:
//   $Log: spoofedmsgbase.cpp $
//   Revision 6.12  2002/10/11 23:28:26Z  td07711
//   remove old comments
//   Revision 6.11  2002/10/08 00:14:19  td07711
//   moved some things to SpoofedObject
//   Revision 6.10  2002/10/04 21:19:25  td07711
//   replace most string use with const char*
//   Revision 6.9  2002/09/27 19:38:40  td07711
//   added support for more messages
//   Revision 6.8  2002/09/26 21:48:19  td07711
//   support ProcStateMsg
//   Revision 6.7  2002/09/24 23:52:15  td07711
//   support limit on spoofs executed
//   Revision 6.6  2002/09/24 00:07:49  td07711
//   add mult/offset support
//   Revision 6.5  2002/09/13 23:47:32  td07711
//   Revision 6.4  2002/09/06 22:49:58  td07711
//   Revision 6.3  2002/09/05 23:44:22  td07711
//   Revision 6.2  2002/09/04 22:36:40  td07711
//   Revision 6.1  2002/09/03 23:02:45  td07711
//   Revision 1.1  2002/08/31 00:36:55  td07711
//   Initial revision


// standard includes
#include <string.h> // for memset()

// spoofer includes
#include "spoofedmsgbase.hpp"
#include "spooferlog.hpp"


//
// initialize statics here
//
int SpoofedMsgBase :: _maxSpoofedMsgs = 0;
int SpoofedMsgBase :: _numSpoofedMsgs = 0;
int SpoofedMsgBase :: _msgMultiplier = 1;


SpoofedMsgBase :: SpoofedMsgBase( const char* name )
: SpoofedObject( name )
{
}


SpoofedMsgBase :: ~SpoofedMsgBase( )
{
}


void SpoofedMsgBase :: setMaxSpoofedMsgs( int n )
{
    // PURPOSE:  limits number of spoofed msgs, 0 means no limit

    _numSpoofedMsgs = 0;
    _maxSpoofedMsgs = n;
}


void SpoofedMsgBase :: setMsgMultiplier( int n )
{
    // PURPOSE:  block or multiply message traffic

    _msgMultiplier = n;
}

