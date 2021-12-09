/**
 * @file          Button.hpp
 * @author		  Bernd Waldmann
 * Created		: 03-Oct-2019
 * Tabsize		: 4
 *
 * This Revision: $Id: Button.h 1301 2021-12-06 16:51:19Z  $
 */ 

/*
   Copyright (C) 2019,2021 Bernd Waldmann

   This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. 
   If a copy of the MPL was not distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/

   SPDX-License-Identifier: MPL-2.0
*/

#ifndef BUTTON_H_
#define BUTTON_H_

/** 
 * @ingroup Button
 * @{
 */


/**
 * @brief Base class for debouncing a button, polling the hardware happens elsewhere
 * 
 */
class Button {
	private:
		volatile uint8_t	mState;
		volatile uint32_t	mLastPressed;
		volatile uint32_t	mLastReleased;
		volatile uint32_t	mMillis;
		volatile bool		mPending;
				 uint8_t	mMillisPerTick;
		
	public:
		/// recommended poll interval in ms.
		static const int 		MS_PER_TICK = 10;			
		/// minimum long press duration [ms]
		static const uint16_t	MIN_LONG_PRESS = 1000u;		
		/// max separation (#1 end to #2 start) for double click [ms]; typically 60-180ms
		static const uint16_t	MAX_DOUBLE_PRESS = 200u;	

		Button() { init(); }
		void init();

		void tick() { tick( pressed() ); }
        void tick( uint8_t isPressed );

        virtual bool pressed() { return false; }   // must be instantiated in a derived class by application

		void setMillisPerTick(uint8_t ms) { if (ms) mMillisPerTick=ms; }

		static void isr(void* arg);
		
		volatile bool		isDown;			///< true if button is currently pressed.
		volatile uint8_t	cPressed;		///< count # of times debounced button was pressed, can be reset by application.
		volatile uint8_t	cReleased;		///< count # of times debounced button was released, can be reset by application.
		volatile uint16_t	holdTime;		///< duration of current button press, in ms.
		volatile uint8_t	cShortPress;	///< count # of short presses detected, can be reset by application
		volatile uint8_t	cLongPress;		///< count # of long presses detected, can be reset by application
		volatile uint8_t	cDoublePress;   ///< count # of double clicks detected, can be reset by application
};


/// @brief Debounce a contact attached to a pin
class ButtonPin : public Button {
	private:
		uint8_t				mMask;
		volatile uint8_t	*mPort;
		
	public:
		ButtonPin(volatile uint8_t* port, uint8_t bit) : Button() { init(port,bit); }
		void init(volatile uint8_t* port, uint8_t bit);
        virtual bool pressed(); 
};


/** @} */

#endif /* BUTTON_H_ */