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

#if !defined( INCLUDE_DATFILE )
#define INCLUDE_DATFILE

#include <string>
using namespace std;
enum DFR_ERROR { DFR_OK ,       // file read OK
         DFR_FOPEN,             // error on file open
         DFR_PARSE,             // parse error on the file
         DFR_VALUE,             // Value out of specified range
         DFR_NOTFOUND           // Section/Name not found
 };

// The config, cal and cobe_config files are made up of a list of
// three types of lines (linefeed terminated) in an ASCII file.  These
// are 1) Section Heading, 2) Values  3) Comment/WhiteSpace.  All values,
// that follow a section heading, belong to that section.  All values,
// belong to a section.  Section names are bounded by '[' , ']' brackets.
// The values follow the format of name=value.

// Each line of text from the file will be read and preserved in its,
// original form so that this class could be used in the future, to
// write out files and preserve comments.

// this is the base class for a single line of text.
class CFileLine {

public:

    enum LINETYPE { LT_ERROR , LT_COMMENT , LT_SECTION , LT_VALUE };

    CFileLine();
    CFileLine( const char * str );
    CFileLine( const char * name, const char * value );
    CFileLine( const CFileLine &fl );
    virtual ~CFileLine();

    const CFileLine & operator=( const CFileLine &fl );

    int Line( void ) const { return m_nLineNum; };
    int Line( int ln ) { return m_nLineNum = ln; };
    LINETYPE Type( void ) const { return m_nType; };
    char * pData( void ) { return m_pLine; };
    const char *cpData( void ) const { return m_pLine; };

    // store the line, parse and determine the type
    void SetData( const char *str );

    // quick check
    bool IsSection( void ) const { return ( m_nType == LT_SECTION ); };
    bool IsValue( void ) const { return ( m_nType == LT_VALUE ); };

    // name and value access if appropriate
    const char *cpName( void ) const { return m_pName; };
    const char *cpValue( void ) const { return m_pValue; };


protected:
    void Init( void );
    void Transfer( const CFileLine &fl );
    void Erase( void );
    bool ParseSection( const char *p );
    bool ParseValue( const char *p , int token );
    LINETYPE Parse( const char * ln );
    void FlushTrail( char * str );
    void FlushLead( char * str );

private:
    LINETYPE m_nType;       // line type
    char * m_pLine;         // original data
    int m_nLineNum;         // line number in the file
    char * m_pName;         // copy of the name for sections, values
    char * m_pValue;        // copy of the value for value lines

};

////////////////////////////////////////////////////////////////////////



// CDatFileReader
// Reads in section file type files: config.dat, cobe_config.dat, cal.dat

class CDatFileReader
{
   public:

      CDatFileReader( const char * fname, int logErrors=0 );
      CDatFileReader( const char * fname, int logErrors, bool throwAwayErrors );
      CDatFileReader( int logErrors=0 );
      virtual ~CDatFileReader();

      // call with filename to read in the file
      DFR_ERROR Initialize( const char *fname, int logErrors=0 );
      DFR_ERROR InitAndThrowAwayErrors( const char *fname, int logErrors );

      // access to most recent error
      DFR_ERROR Error( void ) const { return m_nError; };

      // this search fn will locate the value string for the
      // given section/name combination.  Return NULL is not exist.
      const char * Find( const char *sct , const char *name );
      const char * Find( const char *name );

      // The FindFirst/FindNext interface allows handling of section/name
      // combinations which may appear multiple times in the file.  An example
      // is the hardware ID words
      struct FindIter
      {
         const char *   name;
         void *         pos;
      };

      const char * FindFirst( const char *sct, const char *name, FindIter& iter );
      const char * FindNext( FindIter& iter );
      const char * IterName( FindIter& iter );

      // compatability with the old interface:
      // locate and return the string to the pointer provided.
      bool find( const char * section, const char * name,
                char * return_data, unsigned int return_length);

      // Obtain hexidecimal value given section and name
      unsigned long get_hex( const char *section, const char * name );

      // Obtain a float value given section and name
      float GetFloat( const char *section, const char * name );
      // Obtain a float value without the section
      float GetFloat( const char * name );

      // Obtain a int value given section and name
      int GetInt( const char *section, const char * name );
      // Obtain a int value without the section
      int GetInt( const char * name );

      // Obtain a string value given section and name
      string GetString( const char * section, const char * name );
      // Obtain a string value without the section
      string GetString( const char * name );

      // Obtain a float value and perform a range check.
      // return TRUE if the value is within range.
      bool GetFloat( const char * section, const char * name,
                       float low_value, float high_value,
                       float &return_value );

      // Obtain an int value and perform a range check.
      // return TRUE if the value is within range.
      bool GetInt( const char * section, const char * name,
                       int low_value, int high_value,
                       int &return_value );

      // this provides compatability to the previous dat file interface.
      // Obtain floating values from the file by providing its,
      // section name, value name and low and high limits.
      // If the value is out of range, it sets m_nError appropriately.
      // 
      float get_float( const char * section, const char * name,
                       float low_value, float high_value);

      int get_int( const char * section, const char * name,
                       int low_value, int high_value);

      // modify the line for an existing name=value
      bool SetValue( const char *section,
                      const char *name,
                      const char * value );
      bool SetFloat( const char *section,
                      const char *name,
                      float value );
      bool SetInt( const char *section,
                      const char *name,
                      int value );

      void AddComment( const char * comment );
      void AddSection( const char * sct );
      void AddLine( const char * sct, const char * name, const char * value );
      void AddLine( const char * name, const char * value );
      void RemoveLine( const char * sct, const char * name );
      void RemoveLine( const char * name );


   protected:

      struct LnNode
      {
         CFileLine *lnData;
         void *lnNext;
      };

           
      void Erase( void );
      void Init( int logErrors, bool throwAwayErrors=false );
      void SetError( DFR_ERROR err );
      void LogErr( const char *info, const char *line ) const;
      void LogErr( const char *line , const char *valstr , 
          float mn , float mx ) const;

      CFileLine * FindLine ( const char *sct , const char *name );
      LnNode * FindNode( const char *sct , const char *name );
      CFileLine * FindLine ( const char *name );
      LnNode * FindNode( const char *name );

        LnNode *m_pHead,*m_pTail;
        int m_nNumLines;
        DFR_ERROR m_nError;
        char *m_filename;

        int _LogErrors;
        bool m_ThrowAwayErrors;

};

#endif
