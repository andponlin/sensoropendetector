/*
 * Copyright 2023, Andrew Lindesay <apl@lindesay.co.nz>
 * All Rights Reserved. Distributed under the terms of the MIT License.
 */

#include "common.h"

#include <Arduino.h>

/*static*/
String Common::notificationMethodAsString(NotificationMethod value) {
    switch (value) {
        case LOG:
            return "LOG";
        case THREEMA:
            return "THREEMA";
        default:
            return "???";
    }
}

/*static*/
NotificationMethod Common::notificationMethodFromString(String value) {
    if ("THREEMA" == value) {
        return THREEMA;
    }
    return LOG;
}
