/*
 * Copyright 2023, Andrew Lindesay <apl@lindesay.co.nz>
 * All Rights Reserved. Distributed under the terms of the MIT License.
 */
#ifndef CONSTANTS_H
#define CONSTANTS_H

#define SERIAL_ENABLED

/*
When a switch is toggled, there is a period over which the electronic
contacts are being closed and the sensor can signal on and off
sporadically. This delay is the period over which the signal from the
switch is expected to stabilise and settle on a value.
*/

#define DEBOUNCE_DELAY 100L

// These are the various input and output pins on the board that are
// used in this program. Remember that only certain pins support the
// "low power" wake up interrupts on the board.

#define PIN_LED 2
#define PIN_BUTTON 9
#define PIN_SENSOR 3

// The system should not sleep if the sensor has changed state in this
// number of milliseconds.

#define MIN_PERIOD_TO_SHORT_SLEEP 5000L

#define DELAY_WIFI_CONNECT_MILLIS (20 * 1000)

#define HOST_THREEMA_MSG_API "msgapi.threema.ch"

#endif // CONSTANTS_H
