/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * Copyright (c) 2011 CaridianBCT, Inc. All rights reserved.
 *
 * TITLE:      screenTouch.hpp
 *
 * AUTHOR:     Dan Slauson
 *
 * MODIFIED BY: Robert Watkins
 *
 * COPY AND PASTED AND MODIFIED by: Robert Watkins
 *
 * DESCRIPTION: This class encapsulates the functionality of simulating screen
 *	touches.  That's pretty much it.
 *
 ******************************************************************************/

#ifndef SCREEN_TOUCH_HPP__
#define SCREEN_TOUCH_HPP__

#include "ugl\uglinput.h"
#include "ugl\ugl.h"
#include "ugl\uglevent.h"
#include "ugl\uglwin.h"
#include <guiglobs.hpp>

class ScreenTouch
{
private:
	static UGL_DEVICE_ID	_uglDisplay;
    static UGL_INPUT_SERVICE_ID _uglInputServiceId;
	static UGL_INPUT_DEV_ID _pointerDevId;
	static bool _initialized;

protected:
	/*!
	 * Takes a keypad character and current button as input and returns the 
	 * x coordinate of that keypad character above the current button.
	 *
	 * ch:
     * One of the keypad characters: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, c (clear), 
	 * C (clear), e (enter), E (enter).
	 *
	 * currentButton:
	 * The button for which you want to push character, ch, on the keypad.
	 * currentButton must be from the following list (GUI_BUTTON_DONOR_HEIGHT, 
	 * GUI_BUTTON_DONOR_WEIGHT, GUI_BUTTON_DONOR_BLOOD_TYPE, GUI_BUTTON_DONOR_HEMATOCRIT,
	 * GUI_BUTTON_DONOR_PLATELET_PRECOUNT). Each of these buttons has its own keypad that
	 * appears on the gui above the button.
	 * If currentButton is not one of the above buttons than -1 is returned.
	 *
	 * Return Value:
	 * If ch is a valid keypad character in the list above and currentButton is a valid
	 * button in the list above than the x coordinate of that keypad character is returned.
	 * If currentButton is not a valid button than -1 is returned.
	 * If currentButton is a valid button but ch is not a valid keypad character than the 
	 * x coordinate for the enter button is returned.
     */
	static short charToKeypadBtnXCoordinate(char ch, GUI_BUTTON_ID currentButton);

	/*!
	 * Takes a keypad character and current button as input and returns the 
	 * y coordinate of that keypad character above the current button.
	 *
	 * ch:
     * One of the keypad characters: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, c (clear), 
	 * C (clear), e (enter), E (enter).
	 *
	 * currentButton:
	 * The button for which you want to push character, ch, on the keypad.
	 * currentButton must be from the following list (GUI_BUTTON_DONOR_HEIGHT, 
	 * GUI_BUTTON_DONOR_WEIGHT, GUI_BUTTON_DONOR_BLOOD_TYPE, GUI_BUTTON_DONOR_HEMATOCRIT,
	 * GUI_BUTTON_DONOR_PLATELET_PRECOUNT). Each of these buttons has its own keypad that
	 * appears on the gui above the button.
	 *
	 * Return Value:
	 * If ch is a valid keypad character in the list above and currentButton is a valid
	 * button in the list above than the y coordinate of that keypad character is returned.
	 * If currentButton is not a valid button than -1 is returned.
	 * If currentButton is a valid button but ch is not a valid keypad character than the 
	 * y coordinate for the enter button is returned.
     */
	static short charToKeypadBtnYCoordinate(char ch, GUI_BUTTON_ID currentButton);

	/*!
     * Initializes the ScreenTouch class. This function must be called before calling
	 * any of the other functions in the ScreenTouch class.
	 *
	 * Return Value:
	 * Returns true if there is an error with initialization and false otherwise.
     */
	static bool init();

	/*!
	 * Takes a sequence of keypad characters and pushes those characters for the keypad above
	 * currentButton.
	 *
	 * str:
     * A string consisting of valid keypad characters from the list: 0, 1, 2, 3, 4, 5, 
	 * 6, 7, 8, 9, c (clear), C (clear), e (enter), E (enter). In practice str should end with 
	 * e or E but it doesn't have to.
	 *
	 * currentButton:
	 * The button for which you want to push the sequence of characters, str, 
	 * on the keypad.
	 * currentButton must be from the following list (GUI_BUTTON_DONOR_HEIGHT, 
	 * GUI_BUTTON_DONOR_WEIGHT, GUI_BUTTON_DONOR_BLOOD_TYPE, GUI_BUTTON_DONOR_HEMATOCRIT,
	 * GUI_BUTTON_DONOR_PLATELET_PRECOUNT). Each of these buttons has its own keypad that
	 * appears on the gui above the button.
	 *
	 * Return Value:
	 * If str is empty than true is returned (error), otherwise false is returned (no error)
     */
	static bool keypadSequence(string str, GUI_BUTTON_ID currentButton);

	/*!
	 * Takes x and y coordinates for a button and pushes that button.
	 *
	 * x:
     * X coordinate for which pixal you want to push. x must be in the rage (0, 640) 
	 * not including endpoints.
	 *
	 * y:
	 * Y coordinate for which pixal you want to push. y must be in the rage (0, 480) 
	 * not including endpoints.
	 *
	 * delay:
	 * How many seconds to wait after the button is pushed. The default value of 2 is fine for
	 * most cases.
	 *
	 * Return Value:
	 * If x and y are not in the valid ranges than true is returned (error). 
	 * If x and y are in teh valid ranges than false is returned (no error).
     */
	static bool touch(short x, short y, unsigned short delay = 2);

public:
// These are wrapper functions to handle error values returned by the real functions.
// These are the functions that should actually be called.

	/*!
	 * If there is an error with charToKeypadBtnXCoordinate(), then a DataLog error message is printed and
	 * the program is exited.
     */
	static short wrapCharToKeypadBtnXCoordinate(char ch, GUI_BUTTON_ID currentButton);

	/*!
	 * If there is an error with charToKeypadBtnYCoordinate(), then a DataLog error message is printed and
	 * the program is exited.
     */
	static short wrapCharToKeypadBtnYCoordinate(char ch, GUI_BUTTON_ID currentButton);

	/*!
	 * If there is an error with init(), then a DataLog error message is printed and
	 * the program is exited.
     */
	static void wrapInit();

	/*!
	 * If there is an error with keypadSequence(), then a DataLog error message is printed and
	 * the program is exited.
     */
	static void wrapKeypadSequence(string str, GUI_BUTTON_ID currentButton);

	/*!
	 * If there is an error with touch(), then a DataLog error message is printed and
	 * the program is exited.
     */
	static void wrapTouch(short x, short y, unsigned short delay = 2);
};

#endif /* ifndef SCREEN_TOUCH_HPP__ */

