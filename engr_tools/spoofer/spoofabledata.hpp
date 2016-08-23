// FILENAME: spoofabledata.hpp
// $Header: I:/BCT_Development/vxWorks/Trima/engr_tools/spoofer/rcs/spoofabledata.hpp 6.3 2003/01/29 22:18:26Z td07711 Exp td07711 $
// PURPOSE: defines a spoofable data 
//	provides info for spoofing the data at runtime using string naming convention
// CHANGELOG:
//   $Log: spoofabledata.hpp $
//   Revision 6.3  2003/01/29 22:18:26Z  td07711
//   renamed init() to initdb()
//   Revision 6.2  2002/11/12 00:48:08Z  td07711
//   add OBJECT_TYPE info
//   Revision 6.1  2002/10/24 21:17:44  td07711
//   support char and short data types
//   Revision 1.1  2002/10/04 21:51:35  td07711
//   Initial revision


#ifndef SpoofableData_HPP // prevents multiple inclusion
#define SpoofableData_HPP


#include <map>   
#include <string>


// forward declarations
class EnumInfo;


class SpoofableData
{
    // RESPONSIBILITIES:
    //   1. provides static initdb() to create a database of spoofable data elements
    //   2. provides other static functions for accessing the database
    //   3. registers with database upon construction
    
    public:

        enum DATA_TYPE
        {
            INT, FLOAT, ENUM, SHORT, CHAR, FASTDATA_ARRAY
        };
	
        enum OBJECT_TYPE
        {
            CDS, MSG
        };

        //
        // constructors
        //
        SpoofableData( const char* objectName, const char* name,
                      void* objectAddress, void* dataAddress,
                      DATA_TYPE type, const char* enumName = 0 );

        SpoofableData( OBJECT_TYPE objType, const char* objectName, const char* name,
                      void* objectAddress, void* dataAddress,
                      DATA_TYPE type, const char* enumName = 0 );
        
        SpoofableData( const char* intMessageName );


        virtual ~SpoofableData(); 

        //
        // static functions
        //
        static void initdb( ); // initialize a database of spoofable data
        static void dumpAll( ); // dumps info of all spoofable data
        static void showObjectNames( ); // lists the spoofable object names
        static void showDataNames( const char* objectName ); // lists spoofable data names for given object
        static void validate( const char* objectName, const char* dataName ); // error out and exit if invalid
        static void validate( const char* key ); // error out and exit if invalid
        static const SpoofableData* get( const char* objectName, const char* dataName ); 
        static void cleanup(); // cleanup database of spoofable data

        virtual void dump( ) const; // dumps this instance
        const char* name() const { return _name; };
        const char* objectName() const { return _objectName; };

        int offset() const { return _offset; }; // data's byte offset location in its object 
        DATA_TYPE type() const { return _type; }; // INT, FLOAT, ENUM, SHORT, CHAR
        OBJECT_TYPE objectType() const { return _objectType; }; // CDS, MSG
        int size() const { return _size; }; // size in bytes of the data

        const string valueToString( void* pValue ) const; 
        void stringToValue( void* pValue, const char* stringValue) const;


    protected:

        void setOffset( int offset ) { _offset = offset; }; 

        const char* _name;
        const char* _objectName; // name of parent message

        OBJECT_TYPE _objectType;
        DATA_TYPE _type;
        int _size;
        int _offset;

        EnumInfo* _pEnum; // defines enum info if ENUM

    
    private:

        // this container holds all SpoofedMember objects defined
        // the corresponding iterator type is used to traverse it
        static map< string, SpoofableData* > _database;
        typedef map< string, SpoofableData* >::iterator databaseIterator_t;
        
        void commonInit( void* objectAddress, void* dataAddress, const char* enumName );

        SpoofableData(); // catch unauthorized use
        SpoofableData(const SpoofableData&); // catch unauthorized use 
        SpoofableData& operator=(const SpoofableData& rhs); // catch unauthorized use
};

#endif // SpoofableData_HPP
