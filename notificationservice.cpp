/*
 * Copyright 2023, Andrew Lindesay <apl@lindesay.co.nz>
 * All Rights Reserved. Distributed under the terms of the MIT License.
 */
#include "notificationservice.h"

#include <ArduinoHttpClient.h>

#include "constants.h"
#include "httputils.h"
#include "settings.h"

NotificationService::NotificationService() {
}

NotificationService::~NotificationService() {
}


LogNotificationService::LogNotificationService() {
}

LogNotificationService::~LogNotificationService() {
}

void LogNotificationService::notifyOpen() {
#ifdef SERIAL_ENABLED
    Serial.println("Notify -> opened");
#endif
}

void LogNotificationService::notifyClose() {
#ifdef SERIAL_ENABLED
    Serial.println("Notify -> closed");
#endif
}

ThreemaNotificationService::ThreemaNotificationService(
    const String& description,
    WifiSettings* wifiSettings,
    ThreemaSettings* threemaSettings)
    :
    _description(description),
    _wifiSettings(new WifiSettings(wifiSettings)),
    _threemaSettings(new ThreemaSettings(threemaSettings)) {
}

ThreemaNotificationService::~ThreemaNotificationService() {
    delete _threemaSettings;
    delete _wifiSettings;
}

void ThreemaNotificationService::notify(String& message) {

#ifdef SERIAL_ENABLED
    Serial.print("will open wifi to ssid [");
    Serial.print(_wifiSettings->ssid());
    Serial.println("]");
#endif

    int status = WiFi.begin(
        _wifiSettings->ssid().c_str(),
        _wifiSettings->passphrase().c_str());

    unsigned long start_millis = millis();

    do {
        delay(1000L);
    } while (
        ((millis() - start_millis) < DELAY_WIFI_CONNECT_MILLIS)
        && (WL_CONNECTED != (status = WiFi.status())));

    if (WL_CONNECTED != status) {
#ifdef SERIAL_ENABLED
        Serial.println("unable to begin the wifi session");
#endif
    }
    else {
        ThreemaRecipient* recipientNode = _threemaSettings->recipients();
        while (NULL != recipientNode) {
          notifyRecipient(recipientNode, message);
          recipientNode = recipientNode->next();
        }
    }

    WiFi.disconnect();

#ifdef SERIAL_ENABLED
    Serial.println("ended the wifi");
#endif
}

void ThreemaNotificationService::notifyRecipient(ThreemaRecipient* recipient, String& message) {

#ifdef SERIAL_ENABLED
    Serial.print("will send notification to threema [");
    Serial.print(recipient->to());
    Serial.println("]");
#endif

    WiFiClient wifi;

    // The HTTP library is able to handle the connection itself, but it is not
    // able to HTTPS connect so instead the connection is made from outside the
    // library.  By calling `connectionKeepAlive`, the library will use the
    // existing connection that has already been stood up instead of trying to
    // connect.

    if (wifi.connectSSL(HOST_THREEMA_MSG_API, 443)) {
      notifyRecipient(wifi, recipient, message);
      wifi.stop(); // disconnect
    } else {
#ifdef SERIAL_ENABLED
      Serial.println("unable to connect to the threema api server");
#endif
    }
}

void ThreemaNotificationService::notifyRecipient(WiFiClient& wifi, ThreemaRecipient* recipient, String& message) {
  String payload = "to=" + HttpUtils::encodeFormValue(recipient->to())
    + "&from=" + HttpUtils::encodeFormValue(_threemaSettings->from())
    + "&secret=" + HttpUtils::encodeFormValue(_threemaSettings->secret())
    + "&text=" + HttpUtils::encodeFormValue(message)
    + "\r\n";

  HttpClient httpClient = HttpClient(wifi, HOST_THREEMA_MSG_API, 443);

  httpClient.connectionKeepAlive();

#ifdef SERIAL_ENABLED
  Serial.println("will connect to the threema api server");
#endif

  httpClient.beginRequest();

  if (HTTP_SUCCESS != httpClient.post("/send_simple")) {
#ifdef SERIAL_ENABLED
    Serial.print("failed to POST notification to threema server");
#endif
    return;
  }

  httpClient.sendHeader("Content-Type", "application/x-www-form-urlencoded");
  httpClient.sendHeader("Connection", "close");
  httpClient.sendHeader("Content-Length", payload.length());

  httpClient.beginBody();
  httpClient.print(payload);
  httpClient.endRequest();

  int statusCode = httpClient.responseStatusCode();

  if (2 != statusCode / 100) {
#ifdef SERIAL_ENABLED
      Serial.print("failed to send notification to threema; status code [");
      Serial.print(statusCode);
      Serial.println("]");
#endif
      return;
  }

#ifdef SERIAL_ENABLED
  Serial.print("did send notification to threema [");
  Serial.print(recipient->to());
  Serial.print("] with message [");
  Serial.print(message);
  Serial.println("]");
#endif
}

void ThreemaNotificationService::notifyOpen() {
    notify("Open \"" + _description + "\"");
}

void ThreemaNotificationService::notifyClose() {
    notify("Close \"" + _description + "\"");
}
