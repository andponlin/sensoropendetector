/*
 * Copyright 2023, Andrew Lindesay <apl@lindesay.co.nz>
 * All Rights Reserved. Distributed under the terms of the MIT License.
 */
#ifndef DEBOUNCEDIGITALINPUT_H
#define DEBOUNCEDIGITALINPUT_H

/*
Switches represent digital inputs but while the switch closes, there can be
a moment where it is not actually fully closed. In this case it can oscillate
between on and off for a moment. This is "bouncing" and this class will keep
track of the bouncing giving the switch a period in which it is able to
settle down.
*/

class DebouncedDigitalInput {
public:
  DebouncedDigitalInput(int pin);
  virtual ~DebouncedDigitalInput();

  void pulse();

  bool getState();

  bool allowedToShortSleep();

private:
  int _pin;
  bool _state;
  bool _stateDebounce;
  long _debounceStartTime = 0L;
};

#endif // DEBOUNCEDIGITALINPUT_H
