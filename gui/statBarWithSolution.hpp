/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2009 CaridianBCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  StatBarWithSolution

*     File name:   statBarWithSolution.hpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >***************************/

#ifndef __STAT_BAR_WITH_SOLUTION_HPP__
#define __STAT_BAR_WITH_SOLUTION_HPP__

#include "statBarBase.hpp"

class StatBarWithSolution
   : public StatBarBase
{
protected:
   virtual string    getStatusString ();
   virtual BITMAP_ID defaultBmp () { return BITMAP_STATUS_WITH_SOL_DEFAULT; }

public:
   StatBarWithSolution();
};

#endif // #include __STAT_BAR_WITH_SOLUTION_HPP__

/* FORMAT HASH 3e65d3e24e0e7bbb6ff223dbee96721c */
