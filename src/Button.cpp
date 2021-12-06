/** 
 * @file 		  Button.cpp
 * @author		  Bernd Waldmann
 * Created		: 03.10.2019 16:21:28
 * Tabsize		: 4
 *
 * This Revision: $Id: Button.cpp 1300 2021-12-05 13:05:54Z  $
 */
 
/*
   Copyright (C) 2020,2021 Bernd Waldmann

   This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. 
   If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/

   SPDX-License-Identifier: MPL-2.0
*/


/**
 * @brief Poll and debounce a button, report how often it was pressed and released.
 *
 * if the polling routine tick() is called every 10ms, then it takes 40ms to 
 * detect a button press, and another 40ms to detect a button release, so the
 * class can handle >10 button presses per second.
 * While this is not relevant for manually pressed buttons, it may be relevant 
 * for e.g. reed switches used to sense magnetic pulses from a gas meter
 *
 * There are 3 ways of using this library:
 * 1. using class ButtonPin, define the port and pin when calling the constructor or `init()`, 
 *    then call `tick(void)` repeatedly
 * 2. define a class derived from class Button, implementing the `pressed()` method, 
 *    then call `tick(void)` repeatedly
 * 3. using class Button, just call `tick(uint8_t)` repeatedly, 
 *    providing the current status of the button contact
 * 
 * The variants 2 and 3 where the Button class instance itself has no knowledge of 
 * which port and pin the button is attached to are particularly useful 
 * in combination with my fast <stdpins.h> library for manipulating AVR I/O pins.
 * 
 * Typically, the polling function `tick()` will be called from a timer interrupt service routine.
 * A pointer to the static method `Button::isr()` defined here can be used as an argument
 * to the `add_task()` function from my `AvrTimers` library.
 * 
 * The class detects multiple gestures:
 * 1. any button press, reported at start/end, via cPressed/cReleased
 * 2. short button press (duration <1s), reported 200ms after release, via cShortPress
 * 3. long button press (duration >1s), reported at release, via cLongPress
 * 4. double press (press <200ms after previous release), reported at release, via cDoublePress
 */ 

#include <inttypes.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include <avr/io.h>
#define __STDC_LIMIT_MACROS
#include <stdint.h>


extern "C" unsigned long millis(void);

#include "Button.h"

/*
    The debounce logic samples the state of the button input multiple times
    and accumulates samples in a variable (max 8 samples).
    A valid keypress is a pattern of 1x not pressed, then N times pressed, 
    e.g. 0,1,1,1 for N=3
    A valid key release is a pattern of 1x pressed, then N times not pressed, 
    e.g. 1,0,0,0 for N=3
*/

// level must be steady for 3 ticks
#define NTICKS 3        
// how may samples to look at?
#define MASK ((1 << (NTICKS+1))-1)
// expected pattern at start of keypress
#define RISE ((1 << NTICKS)-1)


/** 
 * @defgroup Button  <Button.hpp>: a class for reading and debouncing a button or contact.
 * @{
 */


/**
 * @brief Initialize basic button instance.
 * 
 */
void Button::init()
{
	mState = 0;
	isDown = false;
}


/** 
 * @brief Static member function that can be called from an ISR. 
 * Converts argument to pointer to Button instance and calls `tick()` member function.
 * 
 * @param arg	pointer to object (argument passed on by timer, mentioned in `AvrTimerBase::add_task`)
 */
void Button::isr(void* arg)
{
	Button* pb = (Button*)arg;
	pb->tick();
}


/** 
 * @brief Do debouncing, his function has no knowledge about which port & pin the button is attached to.
 * @param	isPressed	!=0 if physical button is currently pressed
 */
void Button::tick( uint8_t isPressed )
{
	mMillis += MS_PER_TICK;

	mState <<= 1;
	mState |= isPressed ? 1 : 0;

    // just pressed? look for e.g. [na na na na 0 1 1 1] pattern 
	if ((mState & MASK) == RISE) {
		cPressed++;
		isDown = true;
		holdTime = 0;	// start measuring duration
		mPending = false;

		mLastPressed = mMillis;
	}
    // just released? look for e.g. [na na na na 1 0 0 0] pattern
	if ((mState | ~MASK) == ~RISE) {
		cReleased++;
		isDown = false;

		if (holdTime > MIN_LONG_PRESS) {
			// long press (pressed for more than 1000ms) ?
			if (cLongPress < UINT16_MAX) cLongPress++;			
		} else if ((uint32_t)(mLastPressed - mLastReleased) < MAX_DOUBLE_PRESS) {
			// double press (this start less than 200ms after previous end)?
			if (cDoublePress < UINT16_MAX) cDoublePress++;
		} else {
			// might be a double click, wait and see
			mPending = true;
		}
		mLastReleased = mMillis;
	}
	if (isDown && (holdTime < UINT16_MAX-MS_PER_TICK))
		holdTime += MS_PER_TICK;
	if (mPending && ((uint32_t)(mMillis - mLastReleased) > MAX_DOUBLE_PRESS)) {
		//it's not a double click
		mPending = false;
		if (cShortPress < UINT16_MAX) cShortPress++;			
	}
}


/**
 * @brief Define which pin to poll.
 * 
 * @param port  pointer to port, e.g. `&PINB`  or `&PINC`
 * @param bit   port bit [0..7]
 */
void ButtonPin::init( volatile uint8_t* port, uint8_t bit )
{
	mPort = port;
	mMask = _BV(bit);
	Button::init();
}


bool ButtonPin::pressed(void)
{
	uint8_t b = *(mPort) & mMask;
	return (b != 0);
}


/**@}*/
