/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      simScreenTouch.cpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: See description in corresponding header file.
 *
 *
 *******************************************************************************
 *
 * REVISION HISTORY:
 * $Log: simScreenTouch.cpp $
 * Revision 1.2  2009/08/03 17:15:09Z  dslausb
 * Update logging and screenshot functionality
 * Revision 1.1  2009/06/11 15:30:16Z  dslausb
 * Initial revision
 * Revision 1.11  2009/06/11 15:07:42Z  dslausb
 * Update file headers
 *
 ******************************************************************************/


#include <vxworks.h>
#if CPU == SIMNT								// this whole file is meant only for the simulator
#include "simScreenTouch.hpp"
#include "trima_datalog.h"
#include "bwindow.hpp"
#include "button_bitmap.h"
#include "simDelay.hpp"

UGL_DEVICE_ID			SimScreenTouch::_uglDisplay		=	0;
UGL_INPUT_SERVICE_ID	SimScreenTouch::_uglInputServiceId=	0;
UGL_INPUT_DEV_ID		SimScreenTouch::_pointerDevId		=	0;
bool					SimScreenTouch::_initialized		=	false;

void SimScreenTouch::init()
{
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
		DataLog (log_level_sim_screen_touch_error) << "SimScreenTouch already initialized." << endmsg;
	}
}




void SimScreenTouch::touch(short x, short y)
{
	if ((x > 0)  &&  (x < 640)  &&  (y > 0)  &&  (y < 480))
	{
		UGL_MSG ptrMsg;
		DataLog (log_level_sim_screen_touch_info) << "touch(" << x << "; " << y << ")" << endmsg;

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

		SimDelay::seconds(2); // It seems we need more time to make sure stuff happens.
	}
	else
	{
		DataLog (log_level_sim_screen_touch_error) << "ERROR!!!!!!!: invalid touch coordinates: (" << x << "; " << y << ")" << endmsg;
		SimDelay::seconds(60);
	}
}

void SimScreenTouch::touch(GUI_BUTTON_ID button_id, GUI_SCREEN_ID screen_id)
{
	DataLog (log_level_sim_screen_touch_info) << "touch(BTN=" << button_id << "; SCRN=" << screen_id << ")" << endmsg;

	// Don't do anything until we're actually on the correct screen.
	SimDelay::waitForScreen(screen_id);

	GUI_SCREEN_ID current_screen_id = guiglobs::screen_status.get_current_displayed_screen();

	if (screen_id != current_screen_id)
	{
		DataLog (log_level_sim_screen_touch_error) << "ERROR!!! Screen touch(BTN=" << button_id << "; SCRN="
			<< screen_id << ") on screen " << current_screen_id << endmsg;

		SimDelay::seconds(60);			// give the user a chance to go to the correct screen if possible.
	}
	else
	{
		Base_Window *w = Base_Window::get_window_from_id(screen_id);
		if (w == NULL)
		{
			DataLog (log_level_sim_screen_touch_error) << "ERROR!!! NULL window returned for screen ID "
				<< screen_id << ". Using coordinates (0, 0)" << endmsg;

			touch(button_id, 0, 0);
		}
		else
			touch(button_id, w->left(), w->top());
	}
}

#define incorrectDimension(dim, min, max) (((dim > min)  &&  (dim < max))? false :	\
                                           (DataLog (log_level_sim_screen_touch_error) << "WARNING: Button \"" << button_id << "\" has incorrect " << #dim << " (" << dim << ")" << endmsg, true))

void SimScreenTouch::touch(GUI_BUTTON_ID button_id, short offset_x, short offset_y)
{
	//DataLog (log_level_sim_screen_touch_info) << "touch(BTN=" << button_id << "; " << offset_x << "; " << offset_y << ")" << endmsg;
	Button_Bitmap *b = Button_Bitmap::get_button_from_id(button_id);
	if (b == NULL)
		DataLog (log_level_sim_screen_touch_error) << "ERROR: Unknown button: touch(BTN=" << button_id << "; " << offset_x << "; " << offset_y << ")" << endmsg;
	else
	{
		short left   = b->left();
		short top    = b->top();
		short width  = b->width();
		short height = b->height();
		if (button_id == GUI_BUTTON_CONTINUE)		// why is the continue button not properly initialized?
		{
			if ((left != 206)  ||  (top != 434)  ||  (width != 229)  ||  (height != 32))
			{
				DataLog (log_level_sim_screen_touch_error) << "WARNING: Continue button not properly initialized. ("
					<< left << "; " << top << "; " << width << "; " << height << ")" << endmsg;
				left = 206; top = 434; width = 229; height = 32;
			}
		}
		if (incorrectDimension(offset_x, -1, 640)) offset_x = 0;
		if (incorrectDimension(offset_y, -1, 640)) offset_y = 0;
		if (incorrectDimension(left,     -1, 640))     left = 0;
		if (incorrectDimension(top,      -1, 480))      top = 0;
		if (incorrectDimension(width,     0, 320))    width = 40;
		if (incorrectDimension(height,    0, 240))   height = 30;
		touch(offset_x + left + width/2, offset_y + top + height/2);
	}
}

GUI_BUTTON_ID SimScreenTouch::charToKeypadBtn(char ch)
{
	switch (ch)
	{
	case '0': return GUI_BUTTON_KPAD_0;
	case '1': return GUI_BUTTON_KPAD_1;
	case '2': return GUI_BUTTON_KPAD_2;
	case '3': return GUI_BUTTON_KPAD_3;
	case '4': return GUI_BUTTON_KPAD_4;
	case '5': return GUI_BUTTON_KPAD_5;
	case '6': return GUI_BUTTON_KPAD_6;
	case '7': return GUI_BUTTON_KPAD_7;
	case '8': return GUI_BUTTON_KPAD_8;
	case '9': return GUI_BUTTON_KPAD_9;
	case 'c':
	case 'C': return GUI_BUTTON_KPAD_CLEAR;
	case 'e':
	case 'E': return GUI_BUTTON_KPAD_ENTER;
	default:
		DataLog (log_level_sim_screen_touch_error) << "ERROR: invalid character for the keypad: " << ch << endmsg;
		// let's not make a big deal out of it, and return "ENTER"
		return GUI_BUTTON_KPAD_ENTER;
	}
}



void SimScreenTouch::keypadSequence(const char *str)
{
   // Don't do anything until we're actually on the keypad screen.
   SimDelay::waitForScreen(GUI_SCREEN_KEYPAD);

   DataLog (log_level_sim_screen_touch_info) << "keypadSequence(\"" << str << "\")" << endmsg;
	for (; *str != 0; ++str)
		touch(charToKeypadBtn(*str), GUI_SCREEN_KEYPAD);
}


#endif /* CPU == SIMNT */

