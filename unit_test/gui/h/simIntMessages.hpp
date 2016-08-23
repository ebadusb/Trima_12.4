/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      simIntMessages.hpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: This file gets includeded from the main trimamessages.h if
 *		software is compiled for SIMNT.  It provides several extra int messages
 *		that can be used in simulator and screenshot software.
 *		Note: do not include this file directly.  It does not make sense out of
 *		a very specific context, so your code will not compile.
 *
 *******************************************************************************
 *
 * REVISION HISTORY:
 * $Log: simIntMessages.hpp $
 *
 ******************************************************************************/

#ifndef __SIM_INT_MESSAGES_HPP_1__
#define __SIM_INT_MESSAGES_HPP_1__

// These are messages that are only used to manipulate
// things in the simulator.
INT_MESSAGE( SimntTraverseEnumeratedScreens		); // Use this message to switch screen types in screens that overload the callback.
INT_MESSAGE( SimntGuiScreenChange				);
INT_MESSAGE( SimntGuiScreenUpdate				); // Use this message whenever the screen updates and redraws.
INT_MESSAGE( SimntGuiProcessedRunsumStruct		); // Use this message whenever the screen updates and redraws.
INT_MESSAGE( SimntGuiProcessedProcStatus		); // Use this message whenever the screen updates and redraws.

#endif // #ifndef __SIM_INT_MESSAGES_HPP_1__

#ifdef _TRIMA_MESSAGES_CPP_
#ifndef __SIM_INT_MESSAGES_HPP_2__
#define __SIM_INT_MESSAGES_HPP_2__

DEFINE_OBJ( SimntTraverseEnumeratedScreens		);
DEFINE_OBJ( SimntGuiScreenChange				);
DEFINE_OBJ( SimntGuiScreenUpdate				);
DEFINE_OBJ( SimntGuiProcessedRunsumStruct		);
DEFINE_OBJ( SimntGuiProcessedProcStatus			);
#endif // #ifndef __SIM_INT_MESSAGES_HPP_2__
#endif // #ifdef _TRIMA_MESSAGES_CPP_



