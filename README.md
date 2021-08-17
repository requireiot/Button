**Button/contact debouncing**

-----

## Overview

Debouncing logic for buttons, window contacts etc.

The code can be used with Arduino projects as well as non-Arduino, standard C/C++ AVR projects. It is compatible with Arduino I/O functions as well as with eth fast `stdpins.h` library for pin/port manipulations.

## Features

To use the library, you create an instance of the `Button` class or a derived class, and call the debouncing routine at regular intervals. The actual polling of the input pin can be done by the class, or by your application code, if the polling is a bit more complicated.

Member variables let you inquire
- whether the key is currectly pressed or the contact is closed
- how many times the key has been pressed
- how many times the key has been released
- how long the key has been held down since it was pressed

From these pieces of information, you can derive detection of higher-level gestures like double click, long press etc.

## How to use the library

 There are 3 ways of using this library:

1. use class ButtonPin, define the port and pin when calling the constructor or `init()`, then call `tick(void)` repeatedly

2. define a class derived from class Button, implementing the `pressed()` method, then call `tick(void)` repeatedly

3. use class `Button`, just call `tick(uint8_t)` repeatedly, providing the current status of the button contact

The variants 2 and 3 where the Button class instance itself has no knowledge of which port and pin the button is attached to are particularly useful in combination with my fast [`stdpins.h`](https://github.com/requireiot/stdpins) library for manipulating AVR I/O pins.

Typically, the polling function `tick()` will be called from a timer interrupt service routine. A pointer to the static method `Button::isr()` defined here can be used as an argument to the `add_task()` function from my [`AvrTimers`](https://github.com/requireiot/AvrTimers) library.
