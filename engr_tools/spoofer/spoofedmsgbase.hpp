// FILENAME: spoofedmsgbase.hpp
// $Header: I:/BCT_Development/vxWorks/Trima/engr_tools/spoofer/rcs/spoofedmsgbase.hpp 6.9 2002/10/11 23:28:49Z td07711 Exp td07711 $
// PURPOSE: msg type independent portion of SpoofedMsg class
// CHANGELOG:
//   $Log: spoofedmsgbase.hpp $
//   Revision 6.9  2002/10/11 23:28:49Z  td07711
//   remove getbuf()
//   Revision 6.8  2002/10/09 21:03:46  td07711
//   comment change
//   Revision 6.7  2002/10/08 00:14:40  td07711
//   moved most things to SpoofedObject
//   Revision 6.6  2002/10/04 21:19:02  td07711
//   replace most string use with const char*
//   Revision 6.5  2002/09/24 23:52:26  td07711
//   support limit on spoofs executed
//   Revision 6.4  2002/09/24 00:08:03  td07711
//   add mult/offset support
//   Revision 6.3  2002/09/13 23:47:29  td07711
//   Revision 6.2  2002/09/06 22:50:01  td07711
//   Revision 6.1  2002/09/03 23:02:53  td07711
//   Revision 1.1  2002/08/31 00:36:53  td07711
//   Initial revision


#ifndef SpoofedMsgBase_HPP // prevents multiple inclusion
#define SpoofedMsgBase_HPP


#include <map>    // STL map container
#include <string> // STL string
#include "spoofedobject.hpp"


class SpoofedMsgBase : public SpoofedObject
{
    // RESPONSIBILITIES:
    //   1. defines API for dealing with templated SpoofedMsg objects

    //
    // static data and functions
    //

    public:

        static void setMaxSpoofedMsgs( int n ); // limits max spoofed msgs
	static void setMsgMultiplier( int n ); // block or multiply message traffic


    protected:

        static int _maxSpoofedMsgs;
        static int _numSpoofedMsgs;
	static int _msgMultiplier;


    //
    // instance data and functions
    //

    public:

        SpoofedMsgBase( const char* name );
        virtual ~SpoofedMsgBase( );


        virtual void spoof( ) = 0; // blocking callback for spoofing an unspoofed msg
        virtual void notify( ) = 0; // blocking callback for receiving spoofed msg

        virtual void send( ) = 0; // sends a spoofed msg, with fields set as specified

        virtual char* spoofedBuf() = 0; // returns ptr to spoofed data to send
        virtual char* originalBuf() = 0; // returns ptr to original data


    protected:


    private:

        SpoofedMsgBase(const SpoofedMsgBase&); // catch unauthorized use 
        SpoofedMsgBase& operator=(const SpoofedMsgBase& rhs); // catch unauthorized use
};

#endif // SpoofedMsgBase_HPP
