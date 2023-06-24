/*
 * Copyright 2023, Andrew Lindesay <apl@lindesay.co.nz>
 * All Rights Reserved. Distributed under the terms of the MIT License.
 */
#ifndef INDICATOR_H
#define INDICATOR_H

#include <Arduino.h>

enum IndicatorState {
  INDICATOR_CLOSED,
  INDICATOR_PAUSED_UNTIL_CLOSE,
  INDICATOR_OPEN_PRE_NOTIFY,
  INDICATOR_OPEN_WAIT_FOR_CLOSE
};

/*
The indicator is a flashing LED light which shows various situations by flashing in
different ways. This service is sent a `pulse()` method invocation and it should
flash the LED as necessary during the pulse.
*/

class IndicatorService {
public:
  IndicatorService(int ledPin);

  void setState(IndicatorState state);
  void pulse();

private:
  bool _deriveLedValue();

private:
  IndicatorState _state;
  int _ledPin;
};

#endif // INDICATOR_H
