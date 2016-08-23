/*******************************************************************************
 *
 * Copyright (c) 2003 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      cursor_msgs.h
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file provides message definitions for SERVICE Mode cursor commands
 *
 * HISTORY:
 * $Log$
 *
 ******************************************************************************/


#ifndef __CURSOR_MSGS__
#define __CURSOR_MSGS__



// Must match values in TED ICD.
enum CURSOR_CMDS
{
   CURSOR_LEFT  = 1,
   CURSOR_RIGHT = 2,
   CURSOR_UP    = 3,
   CURSOR_DOWN  = 4
};



typedef struct CURRENT_CURSOR_POSITION_MSG
{
   int x;
   int y;
};


#endif

/* FORMAT HASH 1eeb604635576c3eac8510062370648c */
