// FILENAME: MemoryTest.cpp
// PURPOSE: class implementation for Memory Test
// NOTES: 
//   march_test - runs a march test over the specified memory space
//   This test simply mallocs the requested memory and runs the test over
//   that space.  If memory is not available, the test exits.  Remeber that
//   memory allocated is virtually mapped.  No guarantee is made as to what
//   physical memory is being used, or to whether consecutive virtual pages
//   are consecutive in physical memory.
//   This address increment used is one byte.
//   The march algorithm is described below.
//   486 cache is 8KB 4 way set associative with write through, so this test
//   performs extra reads to invalidate the cache line.
// CHANGLOG:
//   7/25/2000 - dyes - replace write() with writemem() and read() with readmem()
//       to avoid name clash on NT side.
//   5/27/99 - dyes - added refresh test
//   5/25/99 - dyes - initial version


#include "platform.h"
#include "logger.hpp"


#ifdef BUILD_FOR_UNIX
#include <unistd.h>  // for sleep()
#endif

#ifdef BUILD_FOR_NT
#define sleep(n) Sleep(1000*n)
#endif


#include "memorytest.hpp"



MemoryTest::MemoryTest(data_t* starting_address, int nbytes, int maxerrors)
: _starting_address(starting_address), _max_error_count(maxerrors),
  _cache_size(0), _cache_associativity(0), _nbytes(nbytes),
  _error_count(0)
{
    ASSERT(nbytes > 0);
    ASSERT(maxerrors > 0);

    _last_address = starting_address + nbytes/sizeof(data_t) - 1;
    LOG_DEBUG("MemoryTest: nbytes=%d maxerrors=%d",
              _nbytes, _max_error_count);
    LOG_DEBUG("MemoryTest: start=0x%08x end=0x%08x",
              _starting_address, _last_address);
}


MemoryTest::~MemoryTest()
{
    // nothing to cleanup
}


// FUNCTION: writemem
// PURPOSE: writes memory under test with specified data
void MemoryTest::writemem(data_t data)
{
    LOG_DEBUG("write: data=0x%02x", data);

    data_t* pTest;
    
    for(pTest = _starting_address; pTest <= _last_address; pTest++)
        *pTest = data;
}


// FUNCTION: readmem
// PURPOSE: reads memory under test for specified data
void MemoryTest::readmem(data_t data)
{
    LOG_DEBUG("read: data=0x%02x", data);

    data_t* pTest;
    data_t data_read;
    
    for(pTest = _starting_address; pTest <= _last_address; pTest++)
    {
        data_read = *pTest;
        if (data_read != data)
            mem_error(pTest, data_read, data);
    }
}


void MemoryTest::march(data_t data)
{
    LOG_DEBUG("march: data=0x%02x", data);

    data_t datap = ~data;
    data_t* pTest;
    data_t data_read;

    // following used for extra reads to invalidate cache line
    data_t* pExtra;
    data_t temp;
    int i; 
    int delta = (_cache_size*1024)/_cache_associativity;

    // read data, write data/, read data/
    for(pTest = _starting_address; pTest <= _last_address; pTest++)
    {
        // 
        // read data
        //
        data_read = *pTest;
        if (data_read != data)
            mem_error(pTest, data_read, data);
        
        //
        // write data/
        //
        *pTest = datap;
        // extra reads to invalidate the cache set written
        if(_cache_size)
        {
            for(i = 0, pExtra = pTest; i < _cache_associativity; i++)
            {
                pExtra += delta;
                if(pExtra > _last_address) // wrap around
                    pExtra = _starting_address + (pExtra-_last_address);
                temp = *pExtra; // read to flush cache line
            }
        }

        //
        // read data/
        //
        data_read = *pTest;
        if (data_read != datap)
            mem_error(pTest, data_read, datap);
    }
}


// FUNCTION: mem_error
// PURPOSE: logs memory error and exits if maxerrors exceeded
void MemoryTest::mem_error(data_t* address, data_t actual, data_t expected)
{
    LOG_ERROR("mem_error: addr=0x%08x data=0x%02x expected=0x%02x", 
              address, actual, expected);

    if(++_error_count > _max_error_count)
        LOG_FATAL("max error count=%d exceeded", _max_error_count);
}


// FUNCTION: set_cache
// PURPOSE: sets up cache characteristics
// size is in Kbytes (1024 byte chunks)
// associativity specifies set associativity of the cache
void MemoryTest::set_cache(int size, int associativity)
{
    ASSERT(size >= 0);
    ASSERT(associativity >= 0);

    _cache_size = size;
    _cache_associativity = associativity;

    if(_nbytes < 2*1024*_cache_size)
    {
        LOG_WARNING("to bypass cache, memory size under test must exceed %d bytes",
                    2*_cache_size*1024);
    }
}


// FUNCTION: march_test
// PURPOSE: self contained march test using specified data pattern
void MemoryTest::march_test(data_t data)
{
    LOG_DEBUG("march_test: data=0x%02x", data);
    
    writemem(data);
    march(data);
    readmem(~data);
    march(~data);
    readmem(data);
}


// FUNCTION: refresh_test
// PURPOSE: test for DRAM refresh integrity
void MemoryTest::refresh_test(data_t data, int seconds)
{
    LOG_DEBUG("refresh_test: data=0x%02x", data);
    
    writemem(data);
    sleep_unixEmu(seconds);
    readmem(data);

    writemem(~data);
    sleep_unixEmu(seconds);
    readmem(~data);
}

