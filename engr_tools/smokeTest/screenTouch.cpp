/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * Copyright (c) 2011 CaridianBCT, Inc. All rights reserved.
 *
 * TITLE:      screenTouch.cpp
 *
 * AUTHOR:     Dan Slauson
 *
 * MODIFIED BY: Robert Watkins
 *
 * COPY AND PASTED AND MODIFIED by: Robert Watkins
 *
 * DESCRIPTION: See description in corresponding header file.
 *
 ******************************************************************************/
#include <vxworks.h>
#include <datalog.h>

#include "screenTouch.hpp"
#include "trima_datalog.h"
#include "delay.hpp"
#include "simDataLogLevels.hpp"

UGL_DEVICE_ID			ScreenTouch::_uglDisplay		=	0;
UGL_INPUT_SERVICE_ID	ScreenTouch::_uglInputServiceId =	0;
UGL_INPUT_DEV_ID		ScreenTouch::_pointerDevId		=	0;
bool					ScreenTouch::_initialized		=	false;


// Returns -1 upon error.
short ScreenTouch::charToKeypadBtnXCoordinate(char ch, GUI_BUTTON_ID currentButton)
{
	if (currentButton == GUI_BUTTON_DONOR_HEIGHT)
	{
		switch (ch)
		{
		case '0': return BUTN_DONOR_HEIGHT_X + BUTN_KPAD_0_X + 2;
		case '1': return BUTN_DONOR_HEIGHT_X + BUTN_KPAD_1_X + 2;
		case '2': return BUTN_DONOR_HEIGHT_X + BUTN_KPAD_2_X + 2;
		case '3': return BUTN_DONOR_HEIGHT_X + BUTN_KPAD_3_X + 2;
		case '4': return BUTN_DONOR_HEIGHT_X + BUTN_KPAD_4_X + 2;
		case '5': return BUTN_DONOR_HEIGHT_X + BUTN_KPAD_5_X + 2;
		case '6': return BUTN_DONOR_HEIGHT_X + BUTN_KPAD_6_X + 2;
		case '7': return BUTN_DONOR_HEIGHT_X + BUTN_KPAD_7_X + 2;
		case '8': return BUTN_DONOR_HEIGHT_X + BUTN_KPAD_8_X + 2;
		case '9': return BUTN_DONOR_HEIGHT_X + BUTN_KPAD_9_X + 2;
		case 'c':
		case 'C': return BUTN_DONOR_HEIGHT_X + BUTN_KPAD_CLEAR_X + 2;
		case 'e':
		case 'E': return BUTN_DONOR_HEIGHT_X + BUTN_KPAD_ENTER_X + 2;
		default:
			DataLog (log_level_spoofer_debug) << "ERROR: invalid character for the keypad: " << ch << endmsg;
			// let's not make a big deal out of it, and return "ENTER"
			return BUTN_DONOR_HEIGHT_X + BUTN_KPAD_ENTER_X + 2;
		}
	}
	else if (currentButton == GUI_BUTTON_DONOR_WEIGHT)
	{
		switch (ch)
		{
		case '0': return BUTN_DONOR_WEIGHT_X + BUTN_KPAD_0_X + 2;
		case '1': return BUTN_DONOR_WEIGHT_X + BUTN_KPAD_1_X + 2;
		case '2': return BUTN_DONOR_WEIGHT_X + BUTN_KPAD_2_X + 2;
		case '3': return BUTN_DONOR_WEIGHT_X + BUTN_KPAD_3_X + 2;
		case '4': return BUTN_DONOR_WEIGHT_X + BUTN_KPAD_4_X + 2;
		case '5': return BUTN_DONOR_WEIGHT_X + BUTN_KPAD_5_X + 2;
		case '6': return BUTN_DONOR_WEIGHT_X + BUTN_KPAD_6_X + 2;
		case '7': return BUTN_DONOR_WEIGHT_X + BUTN_KPAD_7_X + 2;
		case '8': return BUTN_DONOR_WEIGHT_X + BUTN_KPAD_8_X + 2;
		case '9': return BUTN_DONOR_WEIGHT_X + BUTN_KPAD_9_X + 2;
		case 'c':
		case 'C': return BUTN_DONOR_WEIGHT_X + BUTN_KPAD_CLEAR_X + 2;
		case 'e':
		case 'E': return BUTN_DONOR_WEIGHT_X + BUTN_KPAD_ENTER_X + 2;
		default:
			DataLog (log_level_spoofer_debug) << "ERROR: invalid character for the keypad: " << ch << endmsg;
			// let's not make a big deal out of it, and return "ENTER"
			return BUTN_DONOR_WEIGHT_X + BUTN_KPAD_ENTER_X + 2;
		}
	}
	else if (currentButton == GUI_BUTTON_DONOR_BLOOD_TYPE)
	{
		switch (ch)
		{
		case '0': return BUTN_DONOR_BLOOD_TYPE_X + BUTN_KPAD_0_X + 2;
		case '1': return BUTN_DONOR_BLOOD_TYPE_X + BUTN_KPAD_1_X + 2;
		case '2': return BUTN_DONOR_BLOOD_TYPE_X + BUTN_KPAD_2_X + 2;
		case '3': return BUTN_DONOR_BLOOD_TYPE_X + BUTN_KPAD_3_X + 2;
		case '4': return BUTN_DONOR_BLOOD_TYPE_X + BUTN_KPAD_4_X + 2;
		case '5': return BUTN_DONOR_BLOOD_TYPE_X + BUTN_KPAD_5_X + 2;
		case '6': return BUTN_DONOR_BLOOD_TYPE_X + BUTN_KPAD_6_X + 2;
		case '7': return BUTN_DONOR_BLOOD_TYPE_X + BUTN_KPAD_7_X + 2;
		case '8': return BUTN_DONOR_BLOOD_TYPE_X + BUTN_KPAD_8_X + 2;
		case '9': return BUTN_DONOR_BLOOD_TYPE_X + BUTN_KPAD_9_X + 2;
		case 'c':
		case 'C': return BUTN_DONOR_BLOOD_TYPE_X + BUTN_KPAD_CLEAR_X + 2;
		case 'e':
		case 'E': return BUTN_DONOR_BLOOD_TYPE_X + BUTN_KPAD_ENTER_X + 2;
		default:
			DataLog (log_level_spoofer_debug) << "ERROR: invalid character for the keypad: " << ch << endmsg;
			// let's not make a big deal out of it, and return "ENTER"
			return BUTN_DONOR_BLOOD_TYPE_X + BUTN_KPAD_ENTER_X + 2;
		}
	}
	else if (currentButton == GUI_BUTTON_DONOR_HEMATOCRIT)
	{
		switch (ch)
		{
		case '0': return BUTN_DONOR_HEMATOCRIT_X + BUTN_KPAD_0_X + 2;
		case '1': return BUTN_DONOR_HEMATOCRIT_X + BUTN_KPAD_1_X + 2;
		case '2': return BUTN_DONOR_HEMATOCRIT_X + BUTN_KPAD_2_X + 2;
		case '3': return BUTN_DONOR_HEMATOCRIT_X + BUTN_KPAD_3_X + 2;
		case '4': return BUTN_DONOR_HEMATOCRIT_X + BUTN_KPAD_4_X + 2;
		case '5': return BUTN_DONOR_HEMATOCRIT_X + BUTN_KPAD_5_X + 2;
		case '6': return BUTN_DONOR_HEMATOCRIT_X + BUTN_KPAD_6_X + 2;
		case '7': return BUTN_DONOR_HEMATOCRIT_X + BUTN_KPAD_7_X + 2;
		case '8': return BUTN_DONOR_HEMATOCRIT_X + BUTN_KPAD_8_X + 2;
		case '9': return BUTN_DONOR_HEMATOCRIT_X + BUTN_KPAD_9_X + 2;
		case 'c':
		case 'C': return BUTN_DONOR_HEMATOCRIT_X + BUTN_KPAD_CLEAR_X + 2;
		case 'e':
		case 'E': return BUTN_DONOR_HEMATOCRIT_X + BUTN_KPAD_ENTER_X + 2;
		default:
			DataLog (log_level_spoofer_debug) << "ERROR: invalid character for the keypad: " << ch << endmsg;
			// let's not make a big deal out of it, and return "ENTER"
			return BUTN_DONOR_HEMATOCRIT_X + BUTN_KPAD_ENTER_X + 2;
		}
	}
	else if (currentButton == GUI_BUTTON_DONOR_PLATELET_PRECOUNT)
	{
		switch (ch)
		{
		case '0': return BUTN_DONOR_PLATELET_PRECOUNT_X + BUTN_KPAD_0_X + 2;
		case '1': return BUTN_DONOR_PLATELET_PRECOUNT_X + BUTN_KPAD_1_X + 2;
		case '2': return BUTN_DONOR_PLATELET_PRECOUNT_X + BUTN_KPAD_2_X + 2;
		case '3': return BUTN_DONOR_PLATELET_PRECOUNT_X + BUTN_KPAD_3_X + 2;
		case '4': return BUTN_DONOR_PLATELET_PRECOUNT_X + BUTN_KPAD_4_X + 2;
		case '5': return BUTN_DONOR_PLATELET_PRECOUNT_X + BUTN_KPAD_5_X + 2;
		case '6': return BUTN_DONOR_PLATELET_PRECOUNT_X + BUTN_KPAD_6_X + 2;
		case '7': return BUTN_DONOR_PLATELET_PRECOUNT_X + BUTN_KPAD_7_X + 2;
		case '8': return BUTN_DONOR_PLATELET_PRECOUNT_X + BUTN_KPAD_8_X + 2;
		case '9': return BUTN_DONOR_PLATELET_PRECOUNT_X + BUTN_KPAD_9_X + 2;
		case 'c':
		case 'C': return BUTN_DONOR_PLATELET_PRECOUNT_X + BUTN_KPAD_CLEAR_X + 2;
		case 'e':
		case 'E': return BUTN_DONOR_PLATELET_PRECOUNT_X + BUTN_KPAD_ENTER_X + 2;
		default:
			DataLog (log_level_spoofer_debug) << "ERROR: invalid character for the keypad: " << ch << endmsg;
			// let's not make a big deal out of it, and return "ENTER"
			return BUTN_DONOR_PLATELET_PRECOUNT_X + BUTN_KPAD_ENTER_X + 2;
		}
	}
	return -1;
}

// Returns -1 upon error.
short ScreenTouch::charToKeypadBtnYCoordinate(char ch, GUI_BUTTON_ID currentButton)
{
	if (currentButton == GUI_BUTTON_DONOR_HEIGHT)
	{
		switch (ch)
		{
		case '0': return BUTN_DONOR_HEIGHT_Y + BUTN_KPAD_0_Y + 2;
		case '1': return BUTN_DONOR_HEIGHT_Y + BUTN_KPAD_1_Y + 2;
		case '2': return BUTN_DONOR_HEIGHT_Y + BUTN_KPAD_2_Y + 2;
		case '3': return BUTN_DONOR_HEIGHT_Y + BUTN_KPAD_3_Y + 2;
		case '4': return BUTN_DONOR_HEIGHT_Y + BUTN_KPAD_4_Y + 2;
		case '5': return BUTN_DONOR_HEIGHT_Y + BUTN_KPAD_5_Y + 2;
		case '6': return BUTN_DONOR_HEIGHT_Y + BUTN_KPAD_6_Y + 2;
		case '7': return BUTN_DONOR_HEIGHT_Y + BUTN_KPAD_7_Y + 2;
		case '8': return BUTN_DONOR_HEIGHT_Y + BUTN_KPAD_8_Y + 2;
		case '9': return BUTN_DONOR_HEIGHT_Y + BUTN_KPAD_9_Y + 2;
		case 'c':
		case 'C': return BUTN_DONOR_HEIGHT_Y + BUTN_KPAD_CLEAR_Y + 2;
		case 'e':
		case 'E': return BUTN_DONOR_HEIGHT_Y + BUTN_KPAD_ENTER_Y + 2;
		default:
			DataLog (log_level_spoofer_debug) << "ERROR: invalid character for the keypad: " << ch << endmsg;
			// let's not make a big deal out of it, and return "ENTER"
			return BUTN_DONOR_HEIGHT_Y + BUTN_KPAD_ENTER_Y + 2;
		}
	}
	else if (currentButton == GUI_BUTTON_DONOR_WEIGHT)
	{
		switch (ch)
		{
		case '0': return BUTN_DONOR_WEIGHT_Y + BUTN_KPAD_0_Y + 2;
		case '1': return BUTN_DONOR_WEIGHT_Y + BUTN_KPAD_1_Y + 2;
		case '2': return BUTN_DONOR_WEIGHT_Y + BUTN_KPAD_2_Y + 2;
		case '3': return BUTN_DONOR_WEIGHT_Y + BUTN_KPAD_3_Y + 2;
		case '4': return BUTN_DONOR_WEIGHT_Y + BUTN_KPAD_4_Y + 2;
		case '5': return BUTN_DONOR_WEIGHT_Y + BUTN_KPAD_5_Y + 2;
		case '6': return BUTN_DONOR_WEIGHT_Y + BUTN_KPAD_6_Y + 2;
		case '7': return BUTN_DONOR_WEIGHT_Y + BUTN_KPAD_7_Y + 2;
		case '8': return BUTN_DONOR_WEIGHT_Y + BUTN_KPAD_8_Y + 2;
		case '9': return BUTN_DONOR_WEIGHT_Y + BUTN_KPAD_9_Y + 2;
		case 'c':
		case 'C': return BUTN_DONOR_WEIGHT_Y + BUTN_KPAD_CLEAR_Y + 2;
		case 'e':
		case 'E': return BUTN_DONOR_WEIGHT_Y + BUTN_KPAD_ENTER_Y + 2;
		default:
			DataLog (log_level_spoofer_debug) << "ERROR: invalid character for the keypad: " << ch << endmsg;
			// let's not make a big deal out of it, and return "ENTER"
			return BUTN_DONOR_WEIGHT_Y + BUTN_KPAD_ENTER_Y + 2;
		}
	}
	else if (currentButton == GUI_BUTTON_DONOR_BLOOD_TYPE)
	{
		switch (ch)
		{
		case '0': return BUTN_DONOR_BLOOD_TYPE_Y + BUTN_KPAD_0_Y + 2;
		case '1': return BUTN_DONOR_BLOOD_TYPE_Y + BUTN_KPAD_1_Y + 2;
		case '2': return BUTN_DONOR_BLOOD_TYPE_Y + BUTN_KPAD_2_Y + 2;
		case '3': return BUTN_DONOR_BLOOD_TYPE_Y + BUTN_KPAD_3_Y + 2;
		case '4': return BUTN_DONOR_BLOOD_TYPE_Y + BUTN_KPAD_4_Y + 2;
		case '5': return BUTN_DONOR_BLOOD_TYPE_Y + BUTN_KPAD_5_Y + 2;
		case '6': return BUTN_DONOR_BLOOD_TYPE_Y + BUTN_KPAD_6_Y + 2;
		case '7': return BUTN_DONOR_BLOOD_TYPE_Y + BUTN_KPAD_7_Y + 2;
		case '8': return BUTN_DONOR_BLOOD_TYPE_Y + BUTN_KPAD_8_Y + 2;
		case '9': return BUTN_DONOR_BLOOD_TYPE_Y + BUTN_KPAD_9_Y + 2;
		case 'c':
		case 'C': return BUTN_DONOR_BLOOD_TYPE_Y + BUTN_KPAD_CLEAR_Y + 2;
		case 'e':
		case 'E': return BUTN_DONOR_BLOOD_TYPE_Y + BUTN_KPAD_ENTER_Y + 2;
		default:
			DataLog (log_level_spoofer_debug) << "ERROR: invalid character for the keypad: " << ch << endmsg;
			// let's not make a big deal out of it, and return "ENTER"
			return BUTN_DONOR_BLOOD_TYPE_Y + BUTN_KPAD_ENTER_Y + 2;
		}
	}
	else if (currentButton == GUI_BUTTON_DONOR_HEMATOCRIT)
	{
		switch (ch)
		{
		case '0': return BUTN_DONOR_HEMATOCRIT_Y + BUTN_KPAD_0_Y + 2;
		case '1': return BUTN_DONOR_HEMATOCRIT_Y + BUTN_KPAD_1_Y + 2;
		case '2': return BUTN_DONOR_HEMATOCRIT_Y + BUTN_KPAD_2_Y + 2;
		case '3': return BUTN_DONOR_HEMATOCRIT_Y + BUTN_KPAD_3_Y + 2;
		case '4': return BUTN_DONOR_HEMATOCRIT_Y + BUTN_KPAD_4_Y + 2;
		case '5': return BUTN_DONOR_HEMATOCRIT_Y + BUTN_KPAD_5_Y + 2;
		case '6': return BUTN_DONOR_HEMATOCRIT_Y + BUTN_KPAD_6_Y + 2;
		case '7': return BUTN_DONOR_HEMATOCRIT_Y + BUTN_KPAD_7_Y + 2;
		case '8': return BUTN_DONOR_HEMATOCRIT_Y + BUTN_KPAD_8_Y + 2;
		case '9': return BUTN_DONOR_HEMATOCRIT_Y + BUTN_KPAD_9_Y + 2;
		case 'c':
		case 'C': return BUTN_DONOR_HEMATOCRIT_Y + BUTN_KPAD_CLEAR_Y + 2;
		case 'e':
		case 'E': return BUTN_DONOR_HEMATOCRIT_Y + BUTN_KPAD_ENTER_Y + 2;
		default:
			DataLog (log_level_spoofer_debug) << "ERROR: invalid character for the keypad: " << ch << endmsg;
			// let's not make a big deal out of it, and return "ENTER"
			return BUTN_DONOR_HEMATOCRIT_Y + BUTN_KPAD_ENTER_Y + 2;
		}
	}
	else if (currentButton == GUI_BUTTON_DONOR_PLATELET_PRECOUNT)
	{
		switch (ch)
		{
		case '0': return BUTN_DONOR_PLATELET_PRECOUNT_Y + BUTN_KPAD_0_Y + 2;
		case '1': return BUTN_DONOR_PLATELET_PRECOUNT_Y + BUTN_KPAD_1_Y + 2;
		case '2': return BUTN_DONOR_PLATELET_PRECOUNT_Y + BUTN_KPAD_2_Y + 2;
		case '3': return BUTN_DONOR_PLATELET_PRECOUNT_Y + BUTN_KPAD_3_Y + 2;
		case '4': return BUTN_DONOR_PLATELET_PRECOUNT_Y + BUTN_KPAD_4_Y + 2;
		case '5': return BUTN_DONOR_PLATELET_PRECOUNT_Y + BUTN_KPAD_5_Y + 2;
		case '6': return BUTN_DONOR_PLATELET_PRECOUNT_Y + BUTN_KPAD_6_Y + 2;
		case '7': return BUTN_DONOR_PLATELET_PRECOUNT_Y + BUTN_KPAD_7_Y + 2;
		case '8': return BUTN_DONOR_PLATELET_PRECOUNT_Y + BUTN_KPAD_8_Y + 2;
		case '9': return BUTN_DONOR_PLATELET_PRECOUNT_Y + BUTN_KPAD_9_Y + 2;
		case 'c':
		case 'C': return BUTN_DONOR_PLATELET_PRECOUNT_Y + BUTN_KPAD_CLEAR_Y + 2;
		case 'e':
		case 'E': return BUTN_DONOR_PLATELET_PRECOUNT_Y + BUTN_KPAD_ENTER_Y + 2;
		default:
			DataLog (log_level_spoofer_debug) << "ERROR: invalid character for the keypad: " << ch << endmsg;
			// let's not make a big deal out of it, and return "ENTER"
			return BUTN_DONOR_PLATELET_PRECOUNT_Y + BUTN_KPAD_ENTER_Y + 2;
		}
	}
	return -1;
}

// Returns true if there is an error and false otherwise.
bool ScreenTouch::init()
{
	bool initError = false;

	if (!_initialized)
	{
		_initialized = true;
		UGL_REG_DATA	*pRegistryData;

		uglDriverFind(UGL_DISPLAY_TYPE, 0, (UGL_UINT32 *)&_uglDisplay);
		pRegistryData = uglRegistryFind (UGL_INPUT_SERVICE_TYPE, 0, 0, 0);
		_uglInputServiceId = (UGL_INPUT_SERVICE_ID)pRegistryData->id;

		pRegistryData = uglRegistryFind (UGL_PTR_TYPE, 0, 0, 0);
		_pointerDevId = (UGL_INPUT_DEV_ID)pRegistryData->id;
	}
	else
	{
		initError = true;
		DataLog (log_level_spoofer_debug) << "SimScreenTouch already initialized." << endmsg;
	}

	return initError;
}

// This function doesnt wait for you to be on the keypad screen (i.e. waitForScreen(GUI_SCREEN_KEYPAD)
// isn't called in this function).
// Returns true if there is an error and false otherwise.
bool ScreenTouch::keypadSequence(string str, GUI_BUTTON_ID currentButton)
{
	bool keypadSequenceError = false;

	DataLog (log_level_spoofer_debug) << "keypadSequence(\"" << str << "\")" << endmsg;

	if ( str.empty() )
	{
		keypadSequenceError = true;
	}

	for (int i = 0; i <= str.length() - 1; i++)
	{
		touch(charToKeypadBtnXCoordinate(str[i], currentButton), charToKeypadBtnYCoordinate(str[i], currentButton));
	}

	return keypadSequenceError;
}

// Returns true if there is an error and false otherwise.
bool ScreenTouch::touch(short x, short y, unsigned short delay = 2)
{
	bool touchError = false;

	if ((x > 0)  &&  (x < 640)  &&  (y > 0)  &&  (y < 480))
	{
		UGL_MSG ptrMsg;
		DataLog (log_level_spoofer_debug) << "touch(" << x << "; " << y << ")" << endmsg;

		uglCursorOn(_uglDisplay);
		uglCursorMove(_uglDisplay, x, y);

		memset (&ptrMsg, 0, sizeof (ptrMsg));
		ptrMsg.type = MSG_RAW_PTR;
		ptrMsg.data.rawPtr.deviceId = _pointerDevId;
		ptrMsg.data.rawPtr.isAbsolute = UGL_TRUE;
		ptrMsg.data.rawPtr.pos.absolute.x = x;
		ptrMsg.data.rawPtr.pos.absolute.y = y;
		ptrMsg.data.rawPtr.buttonState = UGL_PTR_BUTTON1;

		uglInputMsgPost(_uglInputServiceId, &ptrMsg);

		memset (&ptrMsg, 0, sizeof (ptrMsg));
		ptrMsg.type = MSG_RAW_PTR;
		ptrMsg.data.rawPtr.deviceId = _pointerDevId;
		ptrMsg.data.rawPtr.isAbsolute = UGL_TRUE;
		ptrMsg.data.rawPtr.pos.absolute.x = x;
		ptrMsg.data.rawPtr.pos.absolute.y = y;

		uglInputMsgPost(_uglInputServiceId, &ptrMsg);

		Delay::seconds(delay); // It seems we need more time to make sure stuff happens.
	}
	else
	{
		bool touchError = true;

		DataLog (log_level_spoofer_debug) << "ERROR!!!!!!!: invalid touch coordinates: (" << x << "; " << y << ")" << endmsg;
		Delay::seconds(60);
	}

	return touchError;
}


// START OF WRAPPER FUNCTIONS

//
// TRUE  = ERROR
// FALSE = NO ERROR
//

short ScreenTouch::wrapCharToKeypadBtnXCoordinate(char ch, GUI_BUTTON_ID currentButton)
{
	short charToKeypadBtnXCoordinateRetVal =  ScreenTouch::charToKeypadBtnXCoordinate(ch, currentButton);
	
	// If statement here is different than in the other wrapers
	if (charToKeypadBtnXCoordinateRetVal == -1) 
	{
		DataLog (log_level_spoofer_debug) << "Error with screenTouch::charToKeypadBtnXCoordinate()" << endmsg;
		exit(0);
	}
	return charToKeypadBtnXCoordinateRetVal;
}

short ScreenTouch::wrapCharToKeypadBtnYCoordinate(char ch, GUI_BUTTON_ID currentButton)
{
	short charToKeypadBtnYCoordinateRetVal = ScreenTouch::charToKeypadBtnYCoordinate(ch, currentButton);
	
	// If statement here is different than in the other wrapers
	if (charToKeypadBtnYCoordinateRetVal == -1) 
	{
		DataLog (log_level_spoofer_debug) << "Error with screenTouch::charToKeypadBtnYCoordinate()" << endmsg;
		exit(0);
	}
	return charToKeypadBtnYCoordinateRetVal;
}

void ScreenTouch::wrapInit()
{
	bool initError =  ScreenTouch::init();

	if (initError == true) 
	{
		DataLog (log_level_spoofer_debug) << "Error with SimScreenTouch::init()" << endmsg;
		exit(0);
	}
}

void ScreenTouch::wrapKeypadSequence(string str, GUI_BUTTON_ID currentButton)
{	
	bool keypadSequenceError = ScreenTouch::keypadSequence(str, currentButton);
	
	// If statement here is different than in the other wrapers
	if (keypadSequenceError == true) 
	{
		DataLog (log_level_spoofer_debug) << "Error with screenTouch::keypadSequence()" << endmsg;
		exit(0);
	}
}

void ScreenTouch::wrapTouch(short x, short y, unsigned short delay = 2)
{
	bool touchError =  ScreenTouch::touch(x, y, delay);
	
	if (touchError == true) 
	{
		DataLog (log_level_spoofer_debug) << "Error with SimScreenTouch::touch(short x, short y)" << endmsg;
		exit(0);
	}
}
