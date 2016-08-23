/* FILENAME: KeywordValueParser.hpp
 * PURPOSE: header file for KeywordValueParser class
 * This class provides functions for parsing a file containing keyword value records
 * NOTES:
 * keyword value delimiter is any whitespace
 * comments lines (beginning with '#' character) are skipped
 * blank lines are skipped
 * leading whitespace is skipped
 * first string in line is keyword
 * remainder of line is value (up to '#' character for trailing comments)
 * if no value, then getValue() returns NULL
 * if getNext returns false, then getKeyword() and getValue() will return NULL
 * CHANGELOG:
 *   11/11/99 - dyes - initial version
 */

#ifndef __KeywordValueParser_HPP__ //  prevents multiple inclusion
#define __KeywordValueParser_HPP__



// CLASSNAME: KeywordValueParser
// RESPONSIBILITIES:
//   1. opens given file for parsing
//   2. provides getNext, getKeyword, and getValue functions
// COLLABORATIONS:
//   1. uses Logger
class KeywordValueParser
{
    public:

        KeywordValueParser(const char* file, int linebufsize=200);
        virtual ~KeywordValueParser();

        bool getNext(); // true if another record found
        const char* getLine() { return _linebuf; };
        const char* getKeyword() { return _keyword; };
        const char* getValue() { return _value; };
        int getLineNumber() { return _linenum; };
        bool check(); // true if file opened OK, else false

    protected:

        char* _filename;
        FILE* _file; 
        char* _linebuf; // input buffer
        char* _tempbuf; // parsing buffer
        char* _keyword;
        char* _value;
        int _linesize;
        int _linenum;

    private:

        KeywordValueParser();
        KeywordValueParser(const KeywordValueParser&); // catch unauthorized use
        KeywordValueParser& operator=(const KeywordValueParser& rhs); // catch unauthorized use
};

#endif // __KeywordValueParser_HPP__

