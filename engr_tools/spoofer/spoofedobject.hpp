// FILENAME: spoofedobject.hpp
// $Header: I:/BCT_Development/vxWorks/Trima/engr_tools/spoofer/rcs/spoofedobject.hpp 6.2 2002/10/18 23:20:12 td07711 Exp td07711 $
// PURPOSE: class definition for a SpoofedObject
// CHANGELOG:
//   $Log: spoofedobject.hpp $
//   Revision 6.2  2002/10/18 23:20:12  td07711
//   add spoofTheData() to spoof the spoofedElements
//   Revision 6.1  2002/10/10 21:28:06  td07711
//   add remove() and showSpoofs()
//   Revision 1.1  2002/10/08 18:31:30  td07711
//   Initial revision


#ifndef SpoofedObject_HPP // prevents multiple inclusion
#define SpoofedObject_HPP


#include <map>    // STL map container
#include <string> // STL string
#include "spoofedelement.hpp"


// CLASSNAME: SpoofedObject
// RESPONSIBILITIES:
//   1. a SpoofedObject is a thing that can be spoofed.  Base class from which
//      a SpoofedMsg and SpoofedCds is derived.
//   2. define functions to be overidden by subclasses
//   3. manages SpoofedElement objects
class SpoofedObject
{

    //
    // static data and functions
    //

    public:

        static SpoofedObject* create( const char* name ); // create via name 
        static SpoofedObject* find( const char* name ); // retrieve via name
        static void remove( const char* name ); // remove from spoofing
        static void cleanup( ); // heap cleanup
        static void enableSpoofing( ) { _spoofingEnabled = true; };
        static void disableSpoofing( ) { _spoofingEnabled = false; };
        static void showSpoofs( ) { _showSpoofs = true; }; // call once to show spoofs as they occur
        static bool isShowSpoofs( ) { return _showSpoofs; };
        static void dumpAll( ); // dumps info for all SpoofedObject
    
    
    protected:

        static bool _spoofingEnabled;
        static bool _showSpoofs;
    
        // this static container holds all SpoofedObjects in use
        static map< string, SpoofedObject* > _allSpoofedObjects;
        typedef map< string, SpoofedObject* >::iterator SpoofedObjectsIterator_t; 
        

    //
    // instance data and functions
    //

    public:

        SpoofedObject( const char* name );
        virtual ~SpoofedObject( );

        void addSpoof( const char* elementName, const char* spoofValue );
        void addSpoof( const char* elementName, float mult, float offset );
        void removeSpoof( const char* elementName );
        void dump( ); // dumps all spoof info for this SpoofedObject

        virtual void spoof( ) = 0; // spoofs this object

        const char* name() const { return _name; };
        int elements() { return _spoofedElements.size(); };

        virtual char* spoofedBuf() = 0; // defined in derived class
        virtual char* originalBuf() = 0; // defined in derived class


    protected:

        const char* _name;
        void spoofTheData(); // nonvirtual helper function used by spoof()

        bool _spoofBufferNotValid; // optimization used to avoid avoid applying the spoof


        // this container holds all SpoofedElement objects defined for this SpoofedObject
        // the corresponding iterator type is used to traverse it
        map< string, SpoofedElement* > _spoofedElements;
        typedef map< string, SpoofedElement* >::iterator spoofedElementsIterator_t;

    
    private:

        SpoofedObject(const SpoofedObject&); // catch unauthorized use 
        SpoofedObject& operator=(const SpoofedObject& rhs); // catch unauthorized use
};

#endif // SpoofedObject_HPP
