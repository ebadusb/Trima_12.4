
#include "filehelp.h"
#include <iostream>
#include <cstdio>
#include <string>

#include "crcgen.h"

using namespace std;

#define BLOCKSIZE  10000

CFileHelper::CFileHelper()
{
    m_pData = 0;
    m_nAllocated = m_nLength = 0;
}

CFileHelper::~CFileHelper()
{
    Empty();
}

void CFileHelper::Empty()
{
    delete [] m_pData;
    m_pData = 0;
}


void CFileHelper::Append( const char *str )
{
    Append( str , strlen( str ) );
}

void CFileHelper::Append( const char *str , int length )
{
    ReSize( length );
    memcpy( &m_pData[m_nLength] , str , length );
    m_nLength += length;
}

// This routine checks if the needed size is available
// and does nothing if it is.  If not, it grows the buffer
// and copies the old data over.
void CFileHelper::ReSize( int sz )
{
    if ( ( m_nLength + sz ) >= m_nAllocated )
    {
        m_nAllocated = m_nLength+sz+BLOCKSIZE;
        char *ptr = new char[m_nAllocated];
        memcpy( ptr , m_pData, m_nLength );
        Empty();
        m_pData = ptr;
    }
}

unsigned long CFileHelper::CRC() const
{
    unsigned long lcrc = INITCRC_DEFAULT;
    crcgen32( &lcrc , (unsigned char *)m_pData , m_nLength );
    return lcrc;
}

unsigned long CFileHelper::CRC( char *str ) const
{
    unsigned long lcrc = CRC();
    sprintf (str , "0x%08x", (unsigned int)lcrc); // Force 8 character output
    return lcrc;
}

int CFileHelper::Length() const
{
    return m_nLength;
}

char * CFileHelper::pData()
{
    return m_pData;
}

const char * CFileHelper::cpData() const
{
    return m_pData;
}

bool CFileHelper::Read( const char *fname )
{
    long sz = 10000; 

    if ( sz == 0 )
       return 0;

    Empty();
    m_nAllocated = m_nLength = 0;
    ReSize( sz );
    
    // regular read is fine for reading
    FILE *fp = fopen( fname , "r" );

    if ( fp == 0 )
       return 0;

    int szret;
    bool bFlag = true;

    if ( ( szret = fread( m_pData , sizeof( char ) , sz , fp )) != sz )
    {
       perror( __FILE__ );
		 cerr << __FILE__ << " " << __LINE__ << " Read Error on file " << fname 
            << ". ret = " << szret << endl;
       bFlag = false;
    }

    m_nLength = sz;
    
    if ( fclose( fp ) != 0 )
    {
       perror( __FILE__ );
		 cerr << __FILE__ << " " << __LINE__ << " FClose Error on file " << fname 
            << "." << endl;
       bFlag = false;
    }
    
    return bFlag;
}




bool CFileHelper::CompareCRC( const char *crcFile )
{
    char FileCRC[20];

    const unsigned long thisCRC = CRC();

    FILE *fp = fopen (crcFile , "r");

    if (fp == 0)
       return 0;

    if (fgets (FileCRC, 15, fp) == 0)
    {
        FileCRC[0] = 0;
    }

    fclose (fp);

    const unsigned long intFileCRC = strtoul(FileCRC, 0, 0);

    bool returnValue = (thisCRC == intFileCRC);
    
    if ( returnValue == false )
    {
	    cerr << __FILE__ << " " << __LINE__ << " CRC Compare on " << crcFile << ": "
            << hex << "[" << thisCRC << "] crc file vs computed of [" << intFileCRC << "]." << dec << endl;
    }

    return returnValue;
}




