/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2009 CaridianBCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  StatBarNoSolution

*     File name:   statBarNoSolution.hpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >***************************/

#ifndef __STAT_BAR_NO_SOLUTION_HPP__
#define __STAT_BAR_NO_SOLUTION_HPP__

#include "statBarBase.hpp"

class StatBarNoSolution
   : public StatBarBase
{
protected:
   virtual string    getStatusString ();
   virtual BITMAP_ID defaultBmp () { return BITMAP_STATUS_NO_SOL_DEFAULT; }

public:
   StatBarNoSolution();
};

#endif // #include __STAT_BAR_NO_SOLUTION_HPP__

/* FORMAT HASH 2b88d30b3490c7f2a438606a99852d79 */
