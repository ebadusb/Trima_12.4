// FILENAME: namedobject.hpp
// $Header: H:/BCT_Development/Trima5.R/Trima/engr_tools/spoofer/rcs/namedobject.hpp 6.1 2002/11/21 16:45:15Z td07711 Exp $
// PURPOSE: abstract base class for NamedMsg, NamedIntMsg, NamedCds
// CHANGELOG:
//   $Log: namedobject.hpp $
//   Revision 6.1  2002/11/21 16:45:15Z  td07711
//   #include restructuring


#ifndef NamedObject_HPP // prevents multiple inclusion
#define NamedObject_HPP


#include <map>    // STL map container
#include "spoofedelement.hpp"



// forward declarations
class SpoofableData;


class NamedObject
{
    // RESPONSIBILITIES:
    //   1. creates objects by name
    //   2. provides way to set and get object data

    //
    // static functions
    //

    public:

        static NamedObject* create( const char* name ); // create NamedObject


    //
    // non static stuff
    //

    public:

        NamedObject( const char* name );
        virtual ~NamedObject( );


        virtual void send( const void* pData, int size) = 0; // sets object data
        const char* name() { return _name; }; // returns object name

        // get object data
        virtual bool getData( const SpoofableData* pSpoofableData, void* buf, int timeoutMsec ) = 0;
        virtual bool getExactData( const SpoofableData* SpoofableData, void* buf, int timeoutMsec, int matchValue ) = 0;


    protected:

        const char* _name;
        void* _pDataBuffer;
        const SpoofableData* _pDataInfo;
        bool _getDataRval;


    private:

        NamedObject(); // catch unauthorized use
        NamedObject(const NamedObject&); // catch unauthorized use 
        NamedObject& operator=(const NamedObject& rhs); // catch unauthorized use
};

#endif // NamedObject_HPP
