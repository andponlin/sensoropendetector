/*
 * Copyright 2023, Andrew Lindesay <apl@lindesay.co.nz>
 * All Rights Reserved. Distributed under the terms of the MIT License.
 */
#ifndef NOTIFICATIONSERVICE_H
#define NOTIFICATIONSERVICE_H

#include <Arduino.h>
#include <WiFiNINA.h>

class ThreemaSettings;
class ThreemaRecipient;
class WifiSettings;

/*
This abstract superclass of the notification services provides the
interfaces for concrete subclasses to provide. The notification
service has only one job; to notify out that the sensor was opened
or closed.
*/

class NotificationService {
    public:
        NotificationService();
        virtual ~NotificationService();

        virtual void notifyOpen() = 0;
        virtual void notifyClose() = 0;
};

class LogNotificationService : public NotificationService {
    public:
        LogNotificationService();
        virtual ~LogNotificationService();

        virtual void notifyOpen();
        virtual void notifyClose();
};

class ThreemaNotificationService : public NotificationService {
    public:
        ThreemaNotificationService(
            const String& description,
            WifiSettings* wifiSettings,
            ThreemaSettings* threemaSettings);
        virtual ~ThreemaNotificationService();

        virtual void notifyOpen();
        virtual void notifyClose();

    private:
        void notify(String& message);
        void notifyRecipient(ThreemaRecipient* recipient, String& message);
        void notifyRecipient(WiFiClient& wifi, ThreemaRecipient* recipient, String& message);

    private:
        String _description;
        WifiSettings* _wifiSettings;
        ThreemaSettings* _threemaSettings;         
};

#endif // NOTIFICATIONSERVICE_H
