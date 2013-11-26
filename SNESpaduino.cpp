/*******************************
 *
 *	File: SNESpaduino.cpp
 *	Description: SNESpaduino library: Use an Arduino to read the button states of a SNES Gamepad
 *	Author: Damon Dransfeld
 *	Web: http://tacticalcode.de
 *	License: CC-BY-SA 3.0 ( http://creativecommons.org/licenses/by-sa/3.0/legalcode )
 *
 ******************************/

#include "Arduino.h"
#include "SNESpaduino.h"

SNESpad::SNESpad(byte data)
{
	PIN_DATA = data;

	pinMode(PIN_DATA, INPUT);

	// enable pull-ups, so we don't read random data
	digitalWrite(PIN_DATA, HIGH);
}

uint16_t SNESpad::getButtons(boolean return_inverted)
{
	// Return the bits
	if(return_inverted) 
		return ~state;
	else
		return state;
}

void SNESpad::reset(){
	state = 0;
}

void SNESpad::read(int i)
{
	// Read a button's state, shift it into the variable
 	state |= digitalRead(PIN_DATA) << i;
}

byte SNESpad::getDataPin()
{
	return PIN_DATA;
}

// Constructor: Init pins
SNESpaduino::SNESpaduino(byte latch, byte clock)
{
	// Store the latch, clock and data pin for later use
	PIN_LATCH = latch;
	PIN_CLOCK = clock;

	// Set correct modes for the communication pins
	pinMode(PIN_LATCH, OUTPUT);
	pinMode(PIN_CLOCK, OUTPUT);

	padCount = 0;
}

void SNESpaduino::addPad(SNESpad* pad) {
	pads[padCount++] = pad;
}

// Return the state of all buttons. 12 of the uint16_t's bits are used, the 4 MSBs must be ignored.
void SNESpaduino::read()
{
	// Init the button-state variable
	for (int k = 0; k < padCount; k++) pads[k]->reset();

	// Latch the current buttons' state into the pad's register
	digitalWrite(PIN_LATCH, HIGH);
	digitalWrite(PIN_LATCH, LOW);

	// Loop to receive 12 bits from the pad
	for(i = 0; i < 12; i++)
	{
		for (int k = 0; k < padCount; k++) {
			pads[k]->read(i);
		}

		// Send a clock pulse to shift out the next bit
		digitalWrite(PIN_CLOCK, HIGH);
		digitalWrite(PIN_CLOCK, LOW);
	}
}
