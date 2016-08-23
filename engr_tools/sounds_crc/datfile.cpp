/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      datfile.h
 *             Modified dat_file_reader to read data into memory and
 *             access more efficiently trima config and calibration
 *             files.
 *
 * AUTHOR:     Bruce Dietrich
 *
 *
 ******************************************************************************/

#include <cctype>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "datfile.h"

/////////////////////////////////////////////////////////////////
// CFileLine
// determine line type here and make copies of name and value if necessary


CFileLine::CFileLine()
{
    Init();
}

CFileLine::CFileLine( const char * str )
{
    Init();
    SetData( str );
}

CFileLine::CFileLine( const char * name, const char * value )
{
    Init();
    char *str = new char[strlen(name)+1+strlen(value)+1];
    strcpy( str, name );
    strcat( str, "=" );
    strcat( str, value );
    SetData( str );
    delete[] str;
}

CFileLine::CFileLine( const CFileLine &fl )
{
    Init();
    Transfer( fl );
}

const CFileLine & CFileLine::operator=( const CFileLine &fl )
{
    if ( this != &fl )
    {
        Transfer( fl );
    }
    return *this;
}

CFileLine::~CFileLine()
{
    Erase();
}

void CFileLine::Init()
{
    m_pLine = 0;
    m_nType = LT_COMMENT;
    m_nLineNum = 0;
    m_pName = 0;
    m_pValue = 0;
}

void CFileLine::Transfer( const CFileLine &fl )
{
    m_nLineNum = fl.Line();
    SetData( fl.cpData() );
}

void CFileLine::Erase()
{
    delete [] m_pLine;
    m_pLine = 0;
    delete [] m_pName;
    m_pName = 0;
    delete [] m_pValue;
    m_pValue = 0;
}

void CFileLine::SetData( const char *str )
{
    Erase();
    m_pLine = new char[strlen( str )+1];
    strcpy( m_pLine , str );
    FlushTrail( m_pLine );
    FlushLead( m_pLine );
    m_nType = Parse( m_pLine );
}


// eliminate trailing whitespace
void CFileLine::FlushTrail( char *str )
{
    int ln = strlen( str ) - 1;
    while ( ( ln >= 0 ) && isspace( str[ln] ) )
    {
        str[ln] = 0;
        ln--;
    }
}

// eliminate leading whitespace
void CFileLine::FlushLead( char *str )
{
    int iStart;     // index of first non whitespace character

    // locate the first non-whitespace character
    iStart = 0;
    while ( str[iStart] && isspace( str[iStart] ) )
    {
        iStart++;
    }

    // if no whitespace, just return
    if ( iStart == 0 ) return;

    // now move the string to the left to eliminate whitespace
    int index = 0;
    while( str[iStart] )
    {
        str[index] = str[iStart];
        index++;
        iStart++;
    }
    str[index] = 0;
}

CFileLine::LINETYPE CFileLine::Parse( const char *ln )
{
   int index = 0;
   bool bFound = false;
   bool allSpace = true;
   LINETYPE ltype = LT_ERROR;

   // skip any white space first
   while ( ( ln[index] != 0 ) && !bFound )
   {
      if ( !isspace( ln[index] ) )
      {
         allSpace = false;
         switch ( ln[index] )
         {
            case '#':       // comment found first
               bFound = true;
               ltype = LT_COMMENT;
               break;
            case '[':       // section heading found
               bFound = true;
               ltype = LT_SECTION;
               // verify good section or log error
               if ( !ParseSection( &ln[index] ) )
               {
                  ltype = LT_ERROR;
               }
               break;
            case '=':       // value is here
               bFound = true;
               ltype = LT_VALUE;
               // verify good syntax or log error
               if ( !ParseValue( ln , index ) )
               {
                  ltype = LT_ERROR;
               }
               break;
            default:
               break;
         }
      }
      index++;
   }
   if ( allSpace ) ltype = LT_COMMENT;

   return ltype;
}

// str points to string starting with '['
// verify closing bracket, and that name exists in between.
// [name].  Return TRUE if successful.
bool CFileLine::ParseSection( const char *str )
{
    int index = strlen( str ) - 1;
    if ( index < 0 ) index = 0;

    while( ( index > 0 ) && ( str[index] != ']' ) )
    {
        index--;
    }

    if ( index > 0 )
    {
        m_pName = new char[index];
        strncpy( m_pName , &str[1] , index - 1 );
        m_pName[index - 1] = 0;
        FlushTrail( m_pName );
        FlushLead( m_pName );
    }
    return (index > 0);
}

// str points to string in the form name=value
// token is the index of the equal sign
// This routine verifies this and stores the name and value.
// return TRUE if successful

bool CFileLine::ParseValue( const char *str , int token )
{
    int ln = strlen( str );
    bool flg = false;

    if ( // check for a non-zero token length.  value can be zero (i.e. "keyword=" is a valid line)
        ( ( ln - token ) >= 1 ) &&
        ( token > 0 )
        )
    {
        m_pName = new char[token+1];
        strncpy( m_pName , str , token );
        m_pName[token] = 0;
        m_pValue = new char[ln - token];
        strncpy( m_pValue , &str[token+1] , ln - token - 1 );
        m_pValue[ln-token-1] = 0;
        FlushTrail( m_pName );
        FlushLead( m_pName );
        FlushTrail( m_pValue );
        FlushLead( m_pValue );
        flg = true;
    }
    return flg;
}



///////////////////////////////////////////////////////////////////////
// CDatFileReader
// Reads in a data file, keeps a copy in memory, parses out sections
// and values.

CDatFileReader::CDatFileReader( const char * fname, int logErrors )
{
    Init( logErrors );
    m_nError = Initialize( fname, logErrors );
}

CDatFileReader::CDatFileReader( const char * fname, int logErrors, bool throwAwayErrors )
{
   Init( logErrors, throwAwayErrors );
   m_nError = Initialize( fname, logErrors );
}

CDatFileReader::CDatFileReader( int logErrors )
{
   Init( logErrors );
}

CDatFileReader::~CDatFileReader()
{
    Erase();
}

void CDatFileReader::Init( int logErrors, bool throwAwayErrors )
{
   m_ThrowAwayErrors = throwAwayErrors;
   m_nError = DFR_OK;
   m_pHead = m_pTail = 0;
   m_nNumLines = 0;
   m_filename = 0;
   _LogErrors = logErrors;
}

void CDatFileReader::Erase()
{
    LnNode *tmp;

    while ( m_pHead )
    {
        delete m_pHead->lnData;
        tmp = m_pHead;
        m_pHead = (LnNode *)m_pHead->lnNext;
        delete tmp;
    }
    delete[] m_filename;
    Init( _LogErrors, m_ThrowAwayErrors );
}

DFR_ERROR CDatFileReader::Initialize( const char * fname, int logErrors )
{
   _LogErrors = logErrors;

   delete[] m_filename;
   m_filename = new char[strlen(fname)+1];
   strcpy(m_filename,fname);
   printf("Initialized filename : %s \n", m_filename);
   FILE *fp = fopen( fname , "r" );

   if ( fp == NULL)
   {
       // do some kind of logging???
       SetError( DFR_FOPEN );
       LogErr( "File Open Error" , fname );
       return m_nError;
   }

   // eliminate previous file if used before
   Erase();

   // make temporary storage for each line
   char * pStr = new char[1000];
   // read each line, parse and store
   while( fgets( pStr , 1000 , fp ) != NULL )
   {
      pStr[999] = 0;
      // create and init another node.  Parse and store the line.
      LnNode *pNd = new LnNode;
      pNd->lnNext = 0;
      pNd->lnData = new CFileLine( pStr );
      // store the current line number
      pNd->lnData->Line( m_nNumLines++ );

      // record parse error if a bad line is encountered
      if ( pNd->lnData->Type() == CFileLine::LT_ERROR )
      {
         if ( !m_ThrowAwayErrors )
         {
            LogErr( "Parse Error" , pNd->lnData->cpData() );
            SetError( DFR_PARSE );
         }
         else
         {
            delete pNd->lnData;
            delete pNd;
            continue;
         }
      }

      // store into our linked list
      // Special case if this is the first line
      if ( m_pHead == 0 )
      {
          m_pHead = pNd;
      }
      else
      {
          m_pTail->lnNext = (void *)pNd;
      }
      m_pTail = pNd;
   }

   // finished with the file, close it.
   fclose( fp );
   delete [] pStr;

   return m_nError;
}

DFR_ERROR CDatFileReader::InitAndThrowAwayErrors( const char *fname, int logErrors )
{
   m_ThrowAwayErrors = true;
   return Initialize( fname, logErrors );
}

const char * CDatFileReader::Find( const char *sct , const char *name )
{
    CFileLine *pLine = FindLine( sct , name );
    if ( pLine == 0 ) return 0;
    return pLine->cpValue();
}

const char * CDatFileReader::Find( const char *name )
{
    CFileLine *pLine = FindLine( name );
    if ( pLine == 0 ) return 0;
    return pLine->cpValue();
}

const char * CDatFileReader::FindFirst( const char *sct , const char *name, FindIter& iter )
{
   const char  * retVal = NULL;
   LnNode *pNd = m_pHead;
   CFileLine *pLine;
   bool bFoundSection = false;

   iter.name = name;
   iter.pos = NULL;

   // first, locate the appropriate section
   while ( pNd )
   {
       pLine = pNd->lnData;

       // If this line is a section header
       if ( pLine->IsSection() )
       {
           // if we already were through our section, then something's wrong
           if ( bFoundSection )
           {
              SetError( DFR_NOTFOUND );
              break;
           }

           // check if it is the correct section
           if ( pLine->cpName() &&
                strcmp( pLine->cpName() , sct ) == 0 )
           {
                bFoundSection = true;
           }
       }

       // If we are within the section, look for the name
       if ( bFoundSection )
       {
           // Is this line a value line?
           if ( pLine->IsValue() )
           {
			    if ( strncmp(name,pLine->cpName(),strlen(name)) == 0)
                {
                    // found it, return pointer to the node
                    iter.pos = (void *)pNd;
                    break;
                }

           }
       }

       pNd = (LnNode *)pNd->lnNext;
   }

   if ( !iter.pos )
   {
      SetError( DFR_NOTFOUND );
   }
   else
   {
      CFileLine * pLine = ((LnNode *)(iter.pos))->lnData;
      retVal = pLine->cpValue(); 
   }

   return retVal;
}

const char * CDatFileReader::FindNext( FindIter& iter )
{
   const char * retVal = NULL;

   while ( iter.pos )
   {
      iter.pos = ((LnNode *)iter.pos)->lnNext;
      if ( iter.pos )
      {
         CFileLine * pLine = ((LnNode *)iter.pos)->lnData;
         if ( pLine->IsSection() )
         {
            //
            // finished scanning section, set to NULL so we stop searching
            //
            iter.pos = NULL;
         }
         else if ( pLine->IsValue() )
         {
            if ( strncmp( iter.name, pLine->cpName() , strlen(iter.name) ) == 0 )
            {
                // found it, return pointer to the node
                retVal = pLine->cpValue();
                break;
            }
         }
      }
   }

   return retVal;
}

const char * CDatFileReader::IterName( FindIter& iter )
{
   CFileLine * pLine = ((LnNode *)iter.pos)->lnData;
   return pLine->cpName();
}

// old interface
bool CDatFileReader::find( const char * section,
                         const char * name,
                         char * return_value,
                         unsigned int return_length
                         )
{
    if ( return_value == 0 )
       return false;

    const char *p = Find( section , name );
    
    if ( p == 0 )
       return false;

    if ( strlen( p ) >= return_length )
    {
       LogErr( "Length of value string too long for return." , name );
       return false;
    }

    strcpy( return_value , p );
    
    return true; 
}

float CDatFileReader::GetFloat( const char *sct , const char *nm )
{
    const char *p = Find( sct , nm );
    if ( p == 0 )
    {
        return 0;
    }

    return (float)atof( p );
}

float CDatFileReader::GetFloat( const char *nm )
{
    const char *p = Find( nm );
    if ( p == 0 )
    {
        return 0;
    }

    return (float)atof( p );
}

int CDatFileReader::GetInt( const char *sct , const char *nm )
{
    const char *p = Find( sct , nm );
    if ( p == 0 )
    {
        return 0;
    }

    return atoi( p );
}

int CDatFileReader::GetInt( const char *nm )
{
    const char *p = Find( nm );
    if ( p == 0 )
    {
        return 0;
    }

    return atoi( p );
}

string CDatFileReader::GetString( const char *sct , const char *nm )
{
    const char * c = Find( sct , nm );
    string s;
    if ( c )
    {
       s = c;
    }

    return s;
}

string CDatFileReader::GetString( const char *nm )
{
    const char * c = Find( nm );
    string s;
    if ( c )
    {
       s = c;
    }

    return s;
}

bool CDatFileReader::GetFloat( const char *sct,
                               const char *nm,
                               float low,
                               float high,
                               float &fret
                               )
{
    const char *p = Find( sct , nm );
    
    if ( p != 0 )
    {
        float tmp = (float)atof( p );
        if ( ( tmp < low ) || ( tmp > high ) )
        {
          LogErr( nm , p , low , high );
          SetError( DFR_VALUE );
          tmp =low;
        }
        fret = tmp;
    }

    return ( m_nError == DFR_OK );
}

bool CDatFileReader::GetInt( const char *sct,
                             const char *nm,
                             int low,
                             int high,
                             int &fret
                               )
{
    const char *p = Find( sct , nm );
    
    if ( p != 0 )
    {
        int tmp = atoi( p );
        if ( ( tmp < low ) || ( tmp > high ) )
        {
          LogErr( nm , p , low , high );
          SetError( DFR_VALUE );
          tmp =low;
        }
        fret = tmp;
    }

    return ( m_nError == DFR_OK );
}


// original interface preserved
// The float value is returned.
// the error code is set if value not found or out of range.
// the error code is not touched if already in error
float CDatFileReader::get_float( const char * section,
                                const char * name,
                                float low_value,
                                float high_value
                                )
{
    float tmp;

    GetFloat( section , name , low_value , high_value , tmp );
    return tmp;
}

int CDatFileReader::get_int( const char * section,
                                const char * name,
                                int low_value,
                                int high_value
                                )
{
    int tmp;

    GetInt( section , name , low_value , high_value , tmp );
    return tmp;
}

unsigned long CDatFileReader::get_hex( const char * section, const char * name)
{

    const char *p = Find( section , name );

    if ( p == 0 ) return 0;

    unsigned long val = strtoul( p, NULL, 16 );
    if ( val == ULONG_MAX )
    {
        LogErr( "Get Hex Error" , name );
        SetError( DFR_VALUE );
    }

    return val;
}

void CDatFileReader::SetError( DFR_ERROR err )
{
    // Only keep the first encountered error
   if ( m_nError == DFR_OK ) m_nError = err;
}

void CDatFileReader::LogErr( const char *inf , const char *ln ) const
{
    if ( _LogErrors )
    {
        printf("%s, %s, %s\n", m_filename, inf, ln);
	   cerr << m_filename << " datafile error: " << inf << ">" << ln << "<" << endl;

    }
}

void CDatFileReader::LogErr( const char *ln , const char *valstr,
                            float mn , float mx ) const
{
    if ( _LogErrors )
    {
	   cerr << m_filename << " datafile error: Range[" << mn << "," << mx << "]: " << ln << "=" << valstr << endl;
    }
}

bool CDatFileReader::SetInt( const char *sct,
                            const char *name,
                            int value )
{
    char tmpstr[15];
    sprintf( tmpstr , "%d", value );
    return SetValue( sct , name , tmpstr );
}

bool CDatFileReader::SetFloat( const char *sct,
                            const char *name,
                            float value )
{
    char tmpstr[40];
    sprintf( tmpstr , "%12.4f", value );

    // clean up the float string before sending on.
    // remove trailing zeros after the decimal point.
    int ln = strlen( tmpstr );
    
    if ( ln <= 0 )
       return false;

    int i = ln -1;
    char ch;
    bool bZeroFound = true;    // true if zeros at end
    while ( bZeroFound && ( i >= 0 ) )
    {
        ch = tmpstr[i];

        if ( ch != '0' )
        {   // non-zero found so exit loop
            bZeroFound = false;
        }

        // if a '0' at the end, just remove it.
        if ( ch == '0' )
        {
            tmpstr[i] = 0;
        }

        // if all zeros behind decimal, remove decimal and exit
        if ( ch == '.' )
        {
            tmpstr[i] = 0;
        }
        i--;
    }

    // Remove preceding spaces from the float string
    i = 0;
    while ( ( tmpstr[i] != 0 ) && ( tmpstr[i] == ' ' ) )
    {
        i++;
    }

    // Now send on to store
    return SetValue( sct , name , &tmpstr[i] );
}

bool CDatFileReader::SetValue( const char *sct,
                               const char *name,
                               const char *value )
{
    CFileLine * ptr = FindLine( sct , name );
    
    if ( ptr == 0 )
       return false;

    char *pstr = new char[strlen( name ) + strlen( value ) + 10];

    sprintf( pstr , "%s=%s", name,value );
    ptr->SetData( pstr );
    delete [] pstr;
    
    return true;
}

void CDatFileReader::AddComment( const char *comment )
{
   //
   // create and init another node. 
   LnNode *pNd = new LnNode;
   pNd->lnNext = 0;
   pNd->lnData = new CFileLine( comment );
   // store the current line number
   pNd->lnData->Line( m_nNumLines++ );

   // record parse error if a bad line is encountered
   if ( pNd->lnData->Type() != CFileLine::LT_COMMENT )
   {
      delete pNd->lnData;
      delete pNd;
      return;
   }

   // store into our linked list
   // Special case if this is the first line
   if ( m_pHead == 0 )
   {
      m_pHead = pNd;
   }
   else
   {
      m_pTail->lnNext = (void *)pNd;
   }
   m_pTail = pNd;
}

void CDatFileReader::AddSection( const char *sct )
{
   //
   // create and init another node. 
   LnNode *pNd = new LnNode;
   pNd->lnNext = 0;
   pNd->lnData = new CFileLine( sct );
   // store the current line number
   pNd->lnData->Line( m_nNumLines++ );

   // record parse error if a bad line is encountered
   if ( pNd->lnData->Type() != CFileLine::LT_SECTION )
   {
      delete pNd->lnData;
      delete pNd;
      return;
   }

   // store into our linked list
   // Special case if this is the first line
   if ( m_pHead == 0 )
   {
      m_pHead = pNd;
   }
   else
   {
      m_pTail->lnNext = (void *)pNd;
   }
   m_pTail = pNd;
}

void CDatFileReader::AddLine( const char *sct,
                              const char *name,
                              const char *value )
{
   if ( FindLine(sct,name) )
   {
      //
      // Return if the line is already in the file ...
      //
      return;
   }

   LnNode *sctNd = FindNode( sct ); 
   if ( sctNd )
   {
      //
      // create and init another node. 
      LnNode *pNd = new LnNode;
      pNd->lnNext = 0;
      pNd->lnData = new CFileLine( name, value );
      // store the current line number
      pNd->lnData->Line( m_nNumLines++ );
   
      // store into our linked list
      if ( sctNd == m_pTail )
      {
         m_pTail->lnNext = (void *)pNd;
         m_pTail = pNd;
      }
      else
      {
         pNd->lnNext = sctNd->lnNext;
         sctNd->lnNext = pNd;
      }
   }
}

void CDatFileReader::AddLine( const char * name, const char * value )
{
   if ( FindLine(name) )
   {
      //
      // Return if the line is already in the file ...
      //
      return;
   }

   //
   // create and init another node. 
   LnNode *pNd = new LnNode;
   pNd->lnNext = 0;
   pNd->lnData = new CFileLine( name, value );
   // store the current line number
   pNd->lnData->Line( m_nNumLines++ );
   
   // store into our linked list
   m_pTail->lnNext = (void *)pNd;
   m_pTail = pNd;
}

void CDatFileReader::RemoveLine( const char *sct,
                                 const char *name )
{
   if ( !FindLine(sct,name) )
   {
      //
      // Return if the line is not already in the file ...
      //
      return;
   }

   LnNode *prevNode=0;
   LnNode *remNode=0;
   LnNode *pNd = m_pHead;
   CFileLine *pLine;
   bool bFoundSection = false;
   
   // first, locate the appropriate section
   while ( pNd )
   {
      pLine = pNd->lnData;

      // If this line is a section header
      if ( pLine->IsSection() )
      {
         // if we already were through our section, then something's wrong
         if ( bFoundSection )
         {
            LogErr( "Value not in specified section" , name );
            SetError( DFR_NOTFOUND );
            return;
         }

         // check if it is the correct section
         if ( pLine->cpName() &&
              strcmp( pLine->cpName() , sct ) == 0 )
         {
            bFoundSection = true;
         }
      }

      // If we are within the section, look for the name
      if ( bFoundSection )
      {
         // Is this line a value line?
         if ( pLine->IsValue() )
         {
            if ( pLine->cpName() &&
                 strcmp( pLine->cpName() , name ) == 0 )
            {
               // found it
               if (prevNode==0)
               {
                  // set the head to be the next ptr
                  m_pHead = (LnNode*)pNd->lnNext;
               }
               else
               {
                  prevNode->lnNext = pNd->lnNext;
               }
               delete pNd->lnData;
               delete pNd;
               m_nNumLines--;
               return;
            }

         }
      }
      prevNode = pNd;
      pNd = (LnNode *)pNd->lnNext;
   }

}

void CDatFileReader::RemoveLine( const char * name )
{
   if ( !FindLine(name) )
   {
      //
      // Return if the line is not already in the file ...
      //
      return;
   }

   LnNode *prevNode=0;
   LnNode *remNode=0;
   LnNode *pNd = m_pHead;
   CFileLine *pLine;
   
   // first, locate the appropriate section
   while ( pNd )
   {
      pLine = pNd->lnData;

      if ( pLine->cpName() &&
           strcmp( pLine->cpName() , name ) == 0 )
      {
         // found it
         if (prevNode==0)
         {
            // set the head to be the next ptr
            m_pHead = (LnNode*)pNd->lnNext;
         }
         else
         {
            prevNode->lnNext = pNd->lnNext;
         }
         delete pNd->lnData;
         delete pNd;
         m_nNumLines--;
         return;
      }

      prevNode = pNd;
      pNd = (LnNode *)pNd->lnNext;
   }

}

CFileLine * CDatFileReader::FindLine( const char *sct,
                                     const char *name )
{
   CFileLine * retVal = NULL;

   LnNode * node = FindNode(sct, name);
   if ( node )
   {
      retVal = node->lnData;
   }

   return retVal;
}

CDatFileReader::LnNode * CDatFileReader::FindNode( const char *sct,
                                                   const char *name )
{
   if ( sct == 0 || name == 0 )
   {
      LogErr( "Value not found" , "Bad Parameters" );
      SetError( DFR_NOTFOUND );
      return 0;
   }


   LnNode *pNd = m_pHead;
   CFileLine *pLine;
   bool bFoundSection = false;
   // first, locate the appropriate section
   while ( pNd )
   {
       pLine = pNd->lnData;

       // If this line is a section header
       if ( pLine->IsSection() )
       {
           // if we already were through our section, then something's wrong
           if ( bFoundSection )
           {
              LogErr( "Value not in specified section" , name );
              SetError( DFR_NOTFOUND );
              return 0;
           }

           // check if it is the correct section
           if ( pLine->cpName() &&
                strcmp( pLine->cpName() , sct ) == 0 )
           {
                bFoundSection = true;
           }
       }

       // If we are within the section, look for the name
       if ( bFoundSection )
       {
           // Is this line a value line?
           if ( pLine->IsValue() )
           {
                if ( pLine->cpName() &&
                     strcmp( pLine->cpName() , name ) == 0 )
                {
                    // found it, return pointer to the node
                    return pNd;
                }

           }
       }
       pNd = (LnNode *)pNd->lnNext;
   }
   printf("FindNode1 %s %s\n", name, sct);

   LogErr( "Not Found" , bFoundSection ? name : sct );
   SetError( DFR_NOTFOUND );
   return 0;
}

CFileLine * CDatFileReader::FindLine( const char *name )
{
   CFileLine * retVal = NULL;

   LnNode * node = FindNode(name);
   if ( node )
   {
      retVal = node->lnData;
   }

   return retVal;
}

CDatFileReader::LnNode * CDatFileReader::FindNode( const char *name )
{
   if ( name == 0 )
   {
      LogErr( "Value not found" , "Bad Parameter" );
      SetError( DFR_NOTFOUND );
      return 0;
   }


   LnNode *pNd = m_pHead;
   CFileLine *pLine;

   // first, locate the appropriate section
   while ( pNd )
   {
       pLine = pNd->lnData;

       // If this line is a section header
       if (    pLine->IsSection() 
            || pLine->IsValue() )
       {
          if ( pLine->cpName() &&
               strcmp( pLine->cpName() , name ) == 0 )
          {
             // found it, return pointer to the node
             return pNd;
          }
       }

       pNd = (LnNode *)pNd->lnNext;
   }

   LogErr( "Not Found" , name );
   SetError( DFR_NOTFOUND );
   return 0;
}





