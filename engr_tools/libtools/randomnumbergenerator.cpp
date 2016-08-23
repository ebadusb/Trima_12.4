/* Copyright 1999 Cobe BCT, Inc.
 *
 * FILENAME: RandomNumberGenerator.cpp
 * PURPOSE: implements the Random Number Generator class
 * CHANGELOG:
 *   09/03/99 - dyes - initial version
 */

#include "randomnumbergenerator.hpp"


// initialize statics
unsigned int RandomNumberGenerator::_Seed = 0;


int RandomNumberGenerator::getint(int min, int max)
{
    ASSERT(max > min);
    return(min + rand()%(max-min+1));
}
