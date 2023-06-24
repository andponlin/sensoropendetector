/*
 * Copyright 2023, Andrew Lindesay <apl@lindesay.co.nz>
 * All Rights Reserved. Distributed under the terms of the MIT License.
 */
#include "sensorservice.h"

#include "constants.h"

/*
This object is capturing the state of the sensor; when it was opened and when
it was closed, when it was last notified as being opened and when it last
notified as being closed.
*/

class SensorState {
  public:
    SensorState();
    virtual ~SensorState();

    void reset();

    bool isOpen() const;

    unsigned long openAt() const;
    unsigned long closedAt() const;
    unsigned long lastNotifiedOpenAt() const;
    unsigned long lastNotifiedClosedAt() const;

    void setOpenAt(unsigned long value);
    void setClosedAt(unsigned long value);
    void setLastNotifiedOpenAt(unsigned long value);
    void setLastNotifiedClosedAt(unsigned long value);

    void printTo(Stream& stream);

  private:
    unsigned long _openAt;
    unsigned long _closedAt;
    unsigned long _lastNotifiedOpenAt;
    unsigned long _lastNotifiedClosedAt;
};

SensorState::SensorState() {
    reset();
}

SensorState::~SensorState() {
}

void SensorState::reset() {
    _openAt = 0L;
    _closedAt = 0L;
    _lastNotifiedOpenAt = 0L;
    _lastNotifiedClosedAt = 0L;
}

bool SensorState::isOpen() const {
    return 0 != openAt() && openAt() > closedAt();
}

unsigned long SensorState::openAt() const {
  return _openAt;
}

unsigned long SensorState::closedAt() const {
    return _closedAt;
}

unsigned long SensorState::lastNotifiedOpenAt() const {
    return _lastNotifiedOpenAt;
}

unsigned long SensorState::lastNotifiedClosedAt() const {
    return _lastNotifiedClosedAt;
}

void SensorState::setOpenAt(unsigned long value) {
    _openAt = value;
}

void SensorState::setClosedAt(unsigned long value) {
    _closedAt = value;
}

void SensorState::setLastNotifiedOpenAt(unsigned long value) {
    _lastNotifiedOpenAt = value;
}

void SensorState::setLastNotifiedClosedAt(unsigned long value) {
    _lastNotifiedClosedAt = value;
}

void SensorState::printTo(Stream& stream) {
#ifdef SERIAL_ENABLED
    Serial.print("{openAt:");
    Serial.print(openAt());
    Serial.print(",closeAt:");
    Serial.print(closedAt());
    Serial.print("}");
#endif
}

SensorService::SensorService(MonitoringSettings* monitoringSettings,
    NotificationService* notificationService,
    IndicatorService* indicatorService)
    :
    _sensorState(new SensorState()),
    _isPaused(false),
    _monitoringSettings(monitoringSettings),
    _notificationService(notificationService),
    _indicatorService(indicatorService) {
    reset();
}

SensorService::~SensorService() {
}

void SensorService::reset() {
    _isPaused = false;
    _sensorState->reset();
}

void SensorService::update(bool open) {
  unsigned long now = millis();

  if (_isPaused) {
    updateWithPause(open, now);
  } else {
    updateWithoutPause(open, now);
  }
}

/*
This is processing a change of state (open/closed) in the sensor
is paused.
*/

// private
void SensorService::updateWithPause(bool open, unsigned long now) {
  _indicatorService->setState(INDICATOR_PAUSED_UNTIL_CLOSE);

   if (_sensorState->isOpen()) {
        if (!open) {
#ifdef SERIAL_ENABLED
          Serial.println("detected closed in pause - unpausing");
#endif
          _isPaused = !_isPaused;
          _sensorState->reset();
        }
    }
    else {
        if (open) {
#ifdef SERIAL_ENABLED
            Serial.println("detected open in pause; will ignore the open");
#endif
            _sensorState->setOpenAt(now);
        }
    }
}

/*
This is processing a change of state (open / closed) in the sensor
is NOT paused.
*/

// private
void SensorService::updateWithoutPause(bool open, unsigned long now) {
    if (_sensorState->isOpen()) {
        if (!open) {

            // here we are capturing that the sensor was closed.

#ifdef SERIAL_ENABLED
            Serial.println("detected closed");
#endif

            _sensorState->setClosedAt(now);
            _indicatorService->setState(INDICATOR_CLOSED);

            if (_sensorState->lastNotifiedOpenAt() > _sensorState->openAt()) {
                _sensorState->setLastNotifiedClosedAt(now);
                _notificationService->notifyClose();
            }
        }
        else {

            // here check to see if there is a need to notify that the sensor
            // is open.

            unsigned long notifyOpenDelayMillis = _monitoringSettings->notifyOpenDelayMinutes() * 60 * 1000;

            if (now - _sensorState->openAt() >= notifyOpenDelayMillis
                && _sensorState->lastNotifiedOpenAt() < _sensorState->openAt()) {
                _sensorState->setLastNotifiedOpenAt(now);
                _notificationService->notifyOpen();
                _indicatorService->setState(INDICATOR_OPEN_WAIT_FOR_CLOSE);
            }
        }
    }
    else {
        if (open) {
#ifdef SERIAL_ENABLED
            Serial.println("detected open");
#endif
            _sensorState->setOpenAt(now);
            _indicatorService->setState(INDICATOR_OPEN_PRE_NOTIFY);
        }
        else {
          _indicatorService->setState(INDICATOR_CLOSED);
        }
    }
}

void SensorService::togglePause() {
  _isPaused = !_isPaused && _sensorState->isOpen();
  _sensorState->reset();
}

/*
The system should not sleep while there are changes in the state on the
sensor. This method will see when changes were last experienced and then
ensure that the system stays awake for a period afterward in case there
are further changes.
*/

bool SensorService::allowedToShortSleep() {
  if (_isPaused || _sensorState->isOpen()) {
    return false;
  }

  long now = millis();
  long last = max(
    max(_sensorState->openAt(), _sensorState->closedAt()),
    max(_sensorState->lastNotifiedOpenAt(), _sensorState->lastNotifiedClosedAt())
  );

  return (now - last) > MIN_PERIOD_TO_SHORT_SLEEP;
}

