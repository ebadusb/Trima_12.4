/* Copyright 1999 Cobe BCT, Inc.
 *
 * FILENAME: RandomNumberGenerator.hpp
 * PURPOSE: defines a Random Number Generator class
 * CHANGELOG:
 *   09/03/99 - dyes - initial version
 */

#ifndef RandomNumberGenerator_HPP // prevents multiple inclusion
#define RandomNumberGenerator_HPP

#include <stdlib.h>
#include "logger.hpp"


// CLASSNAME: RandomNumberGeneratorTest
// RESPONSIBILITIES:
//   1. can be reseeded at any time
//   2. returns random int between 0 and max
//   3. return random bool
// COLLABORATIONS:
//   1. Uses Logging macros defined in Logger.hpp
class RandomNumberGenerator
{
    public:

        RandomNumberGenerator(unsigned int seed) { _Seed = seed; };
        RandomNumberGenerator() { };
        static void setseed(unsigned int seed) { _Seed = seed; srand(seed); };
        bool getbool() { return rand()&0x01; };
        int getint(int min, int max);
        static unsigned int getseed() { return _Seed; };


    protected:

        static unsigned int _Seed;

    private:

        RandomNumberGenerator(const RandomNumberGenerator&); // catch unauthorized use 
        RandomNumberGenerator& operator=(const RandomNumberGenerator& rhs); // catch unauthorized use
};

#endif // RandomNumberGenerator_HPP
