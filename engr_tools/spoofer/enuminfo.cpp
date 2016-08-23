// FILENAME: enuminfo.cpp
// $Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/engr_tools/spoofer/rcs/enuminfo.cpp 6.4 2002/10/10 21:24:51Z td07711 Exp $
// PURPOSE: provides for named access to supported enums and enum values
// CHANGELOG:
//   $Log: enuminfo.cpp $
//   Revision 6.4  2002/10/10 21:24:51Z  td07711
//   change error msg to debug msg
//   Revision 6.3  2002/10/04 21:21:52  td07711
//   replace most string use with const char*
//   Revision 6.2  2002/10/01 22:33:38  td07711
//   add a debug msg
//   Revision 6.1  2002/09/10 23:22:40  td07711
//   Revision 1.1  2002/09/09 23:51:55  td07711
//   Initial revision


#include "enuminfo.hpp"
#include "spooferlog.hpp"


// initialize statics
map< string, EnumInfo* > EnumInfo :: _database;


EnumInfo :: EnumInfo( const char* name )
: _name( name )
{
    // error out if already in _allEnumInfo
    if( _database.find( name ) != _database.end() )
    {
        LOG_ERROR("EnumInfo: %s already exists", name );
        cleanup();
        exit(1);
    }

    _database[ name ] = this;

    LOG_DEBUG("EnumInfo: %s constructed", name );
}


EnumInfo :: ~EnumInfo( )
{
    LOG_DEBUG("~EnumInfo: %s destroyed", enumName() );
}


int EnumInfo :: getValue( const char* name, const char* value )
{
    // PURPOSE: given enum and value names, return enum value as int
    // error out on invalid name

    return get( name )->getValue( value );
}


void EnumInfo :: dump( )
{
    // PURPOSE: dumps supported values for this enum

    LOG_INFO("dump: enum %s", enumName() );

    for( EnumValuesIterator_t i = _enumValues.begin();
        i != _enumValues.end(); i++ )
    {
        LOG_INFO("dump: enum %s value %s = %d", enumName(),
                 (*i).first.c_str(), (*i).second );
    }
}


void EnumInfo :: dumpAll( )
{
    // PURPOSE: dumps all Field Info objects 

    for( DatabaseIterator_t i = _database.begin();
        i != _database.end(); i++)
    {
        ((*i).second)->dump();
    }
}


void EnumInfo :: showEnums( )
{
    // PURPOSE: list all enum names supported

    LOG_INFO("Enums supported:");

    for( DatabaseIterator_t i = _database.begin();
        i != _database.end(); i++ )
    {
        LOG_INFO("    %s", ( *i ).second->enumName() );
    }
}


void EnumInfo :: showValues( const char* name )
{
    // PURPOSE: list all values supported for given enum

    if( _database.find( name ) == _database.end() )
    {
        LOG_ERROR("showValues: enum %s not found", name );
        cleanup();
        exit(1);
    }

    LOG_INFO("Values supported for enum %s:", name );
    EnumInfo* p = _database[ name ] ;

    for( EnumValuesIterator_t i = p->_enumValues.begin();
        i != p->_enumValues.end(); i++ )
    {
        LOG_INFO("    %s = %d", (*i).first.c_str(), (*i).second );
    }
}


void EnumInfo :: cleanup( )
{
    // PURPOSE:  cleanup _database stuff

    LOG_DEBUG("cleanup: n=%d", _database.size());

    for(DatabaseIterator_t i = _database.begin(); i != _database.end(); ++i)
    {
        LOG_DEBUG("cleanup: erasing entry for %s", i->second->enumName());

		delete i->second;
    }

	_database.clear();
}


void EnumInfo :: addValue( const char* valueName, int value )
{
    // PURPOSE: adds an enum value, string name and int, to the enum info

    // give warning if value is already there
    if( _enumValues.find( valueName ) != _enumValues.end() )
    {
        LOG_WARNING("addValue: for enum %s, %s already defined", 
                    enumName(), valueName );
    }
    else // add
    {
        _enumValues[ valueName ] = value;

        LOG_DEBUG("addValue: for enum %s, added %s = %d", 
                  enumName(), valueName, value );
    }
}


EnumInfo* EnumInfo :: get( const char* name )
{
    // PURPOSE: given name, returns ptr to enum info

    if( _database.find( name ) == _database.end() )
    {
        LOG_ERROR("get: %s not found", name );
        exit(1);
    }

    return _database[ name ] ;
}


int EnumInfo :: getValue( const char* value )
{
    // PURPOSE: given value name, return int value

    if( _enumValues.find( value ) == _enumValues.end() )
    {
        LOG_DEBUG("getValue: for enum %s, %s not found",
                  enumName(), value );
        return -1;
    }

    return _enumValues[ value ];
}


const char* EnumInfo :: getValueName( int value )
{
    // PURPOSE: given int, return value name

    for( EnumValuesIterator_t i = _enumValues.begin();
        i != _enumValues.end(); i++ )
    {
        if( ( *i ).second == value )
        {
            return ( *i ).first.c_str() ;
        }
    }

    LOG_DEBUG("getValueName: undefined value for enum %s value=%d",
              enumName(), value );

    return( "undefinedEnumValue" );
}
