/*
 * Copyright 2023, Andrew Lindesay <apl@lindesay.co.nz>
 * All Rights Reserved. Distributed under the terms of the MIT License.
 */

#ifndef COMMON_H
#define COMMON_H

#include <Arduino.h>

/*
This enum defines the different ways in which somebody could be notified about
the sensor being open.
*/

enum NotificationMethod {
  LOG,
  THREEMA
};

/*
This is a set of common functions that are used through the software.
*/

class Common {
public:
  static String notificationMethodAsString(NotificationMethod value);
  static NotificationMethod notificationMethodFromString(String value);
};

#endif // COMMON_H
