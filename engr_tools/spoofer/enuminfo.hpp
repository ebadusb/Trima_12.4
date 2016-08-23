// FILENAME: enuminfo.hpp
// $Header: I:/BCT_Development/vxWorks/Trima/engr_tools/spoofer/rcs/enuminfo.hpp 6.1 2002/09/10 23:22:31 td07711 Exp td07711 $
// PURPOSE: defines which enums and enum values are supported for named access
// CHANGELOG:
//   $Log: enuminfo.hpp $
//   Revision 6.1  2002/09/10 23:22:31  td07711
//   Revision 1.1  2002/09/09 23:51:54  td07711
//   Initial revision


#ifndef EnumInfo_HPP // prevents multiple inclusion
#define EnumInfo_HPP


#include <map>    // STL map container
#include <string> // STL string


// CLASSNAME: EnumInfo
// RESPONSIBILITIES:
//   1. Constructor adds EnumInfo objects to a static container as they are created
//   2. Provides addValue() to define named access to supported values
class EnumInfo
{
    public:

        EnumInfo( const char* name ); // name of the enum supported
        virtual ~EnumInfo( );

        virtual void dump( ); // dumps data to stdout
        static void dumpAll( ); // dumps all enum info objects to stdout
        static void showEnums( ); // lists the enums supported
        static void showValues( const char* name ); // lists values supported for given enum
        static int getValue( const char* name, const char* value);
        static EnumInfo* get( const char* name);
        int getValue( const char* value ); // enum value name to int conversion
        const char* getValueName( int value ); // enum value to value name conversion

        static void cleanup( ); // must be run prior to task exit to
        // run destructors on enum info objects remaining

        const char* enumName( ) { return _name; };

        void addValue( const char* name, int value );

    
    protected:

 
    private:

        const char* _name; // name of this enum

        // this container holds all enum info objects defined
        // the corresponding iterator type is used to traverse it
        static map< string, EnumInfo* > _database;
        typedef map< string, EnumInfo* >::iterator DatabaseIterator_t;
        
        // this container holds all values supported for this particular enum
        map< string, int > _enumValues;
        typedef map< string, int >::iterator EnumValuesIterator_t;

        EnumInfo(); // catch unauthorized use
        EnumInfo(const EnumInfo&); // catch unauthorized use 
        EnumInfo& operator=(const EnumInfo& rhs); // catch unauthorized use
};

#endif // EnumInfo_HPP
