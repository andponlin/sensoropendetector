/*
 * Copyright 2023, Andrew Lindesay <apl@lindesay.co.nz>
 * All Rights Reserved. Distributed under the terms of the MIT License.
 */
#ifndef SENSORSTATE_H
#define SENSORSTATE_H

#include "settings.h"
#include "notificationservice.h"
#include "indicatorservice.h"

class SensorState;

/*
This object keeps track of the sensor state. A sensor can be open or closed, but there
is also the concept of the sensor being paused. If a sensor is paused then there is no
notification sent for the sensor being open.

This service also keeps track of signalling to the notification and indicator
services to let them know when they should send a notification or show an
indicator (flash an LED for example) respectively. This takes into account the
state of the sensor and the pause.
*/

class SensorService {
    public:
        SensorService(MonitoringSettings* monitoringSettings,
            NotificationService* notificationService,
            IndicatorService* indicatorService);
        ~SensorService();

        void update(bool open);
        void reset();
        void togglePause();
        bool allowedToShortSleep();

    private:
        void updateWithoutPause(bool open, unsigned long now);
        void updateWithPause(bool open, unsigned long now);

    private:
        SensorState* _sensorState;
        bool _isPaused;
        IndicatorService* _indicatorService;
        MonitoringSettings* _monitoringSettings;
        NotificationService* _notificationService;
};

#endif // SENSORSTATE_H
