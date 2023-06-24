/*
 * Copyright 2023, Andrew Lindesay <apl@lindesay.co.nz>
 * All Rights Reserved. Distributed under the terms of the MIT License.
 */
#include "indicatorservice.h"

IndicatorService::IndicatorService(int ledPin)
  :
  _state(INDICATOR_CLOSED),
  _ledPin(ledPin) {
}

void IndicatorService::setState(IndicatorState state) {
  _state = state;
}

void IndicatorService::pulse() {
  digitalWrite(_ledPin, _deriveLedValue());
}

bool IndicatorService::_deriveLedValue() {
  unsigned long now = millis();
  
  switch (_state) {
    case INDICATOR_CLOSED:
      return LOW;
    case INDICATOR_PAUSED_UNTIL_CLOSE:
      switch ((now % 5000) / 250) {
        case 1:
        case 3:
        case 5:
          return HIGH;
        default:
        return LOW;
      }
    case INDICATOR_OPEN_PRE_NOTIFY:
      return (now % 2000) < 1000;
    case INDICATOR_OPEN_WAIT_FOR_CLOSE:
      return (now % 5000) < 1000;
    default:
      return HIGH;
  }
}
