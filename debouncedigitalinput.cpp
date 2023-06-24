/*
 * Copyright 2023, Andrew Lindesay <apl@lindesay.co.nz>
 * All Rights Reserved. Distributed under the terms of the MIT License.
 */
#include "debouncedigitalinput.h"

#include <Arduino.h>

#include "constants.h"

DebouncedDigitalInput::DebouncedDigitalInput(int pin)
  :
  _pin(pin),
  _state(false),
  _stateDebounce(false),
  _debounceStartTime(0L)
{
}

DebouncedDigitalInput::~DebouncedDigitalInput() {
}

void DebouncedDigitalInput::pulse() {
  boolean currentState = (LOW == digitalRead(_pin));
  long now = millis();

  if (currentState != _stateDebounce) {
    _debounceStartTime = now;
  }

  if ((now - _debounceStartTime) > DEBOUNCE_DELAY) {
    _state = _stateDebounce;
  }

  _stateDebounce = currentState;
}

bool DebouncedDigitalInput::getState() {
  return _state;
}

/*
The device should only go into sleep mode once the button has debounced and has
settled on being on or off. This method will return true if the input has
settled down.
*/

bool DebouncedDigitalInput::allowedToShortSleep() {
  long now = millis();
  return now > (_debounceStartTime + (DEBOUNCE_DELAY * 2));
}
