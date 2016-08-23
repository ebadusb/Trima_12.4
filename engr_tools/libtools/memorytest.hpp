/* Copyright 1999 Cobe BCT, Inc.
 *
 * FILENAME: MemoryTest.hpp
 * PURPOSE: defines a Memory Test class
 * CHANGELOG:
 *   07/25/2000 - dyes - replace write() with writemem() and read() with readmem()
 *       to avoid name clash on NT side.
 *   05/26/99 - dyes - initial version
 */

#ifndef MemoryTest_HPP // prevents multiple inclusion
#define MemoryTest_HPP

#include "logger.hpp"


typedef char data_t; // data type being tested


// CLASSNAME: MemoryTest
// RESPONSIBILITIES:
//   1. manages running a test on specified area of memory
//   2. extensible to support different kinds of tests
//   3. error count can be reset and queried
//   4. does not allow copying or assignment
//   5. support march_test
//   6. supports refresh_test
// COLLABORATIONS:
//   1. Uses Logging macros defined in Logger.hpp
class MemoryTest
{
    public:

        MemoryTest(data_t* starting_address, int nlocations, int maxerrors); 
        ~MemoryTest();

        // standalone tests
        void march_test(data_t data);
        void refresh_test(data_t data, int seconds);

        // error count control
        void reset_error_count() { _error_count = 0; };  
        int get_error_count() { return _error_count; } ;

        // cache settings
        void set_cache(int size, int n_way_sets); // size in Kbytes


    protected:

        int _nbytes; // size of memory to test
        data_t* _starting_address;
        data_t* _last_address;
        int _cache_size;
        int _cache_associativity;
        int _max_error_count;
        int _error_count;

        void writemem(data_t data); // fills mem
        void march(data_t data); // one pass march, rwr
        void readmem(data_t data); // reads and checks mem
        void mem_error(data_t* address, data_t actual, data_t expected);

    private:

        MemoryTest(); // catch unauthorized use
        MemoryTest(const MemoryTest&); // catch unauthorized use 
        MemoryTest& operator=(const MemoryTest& rhs); // catch unauthorized use
};

#endif // MemoryTest_HPP
