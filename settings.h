/*
 * Copyright 2023, Andrew Lindesay <apl@lindesay.co.nz>
 * All Rights Reserved. Distributed under the terms of the MIT License.
 */

// this file contains a number of data structures that define the way in
// which the application will be configured when it runs.

#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>

#include "common.h"

class AvailableWifiNetwork {
  public:
    AvailableWifiNetwork(String ssid, AvailableWifiNetwork* next);
    virtual ~AvailableWifiNetwork();
  
    const String ssid() const;
    AvailableWifiNetwork* next() const;
    void setNext(AvailableWifiNetwork* node);

    static AvailableWifiNetwork* lowerBound(AvailableWifiNetwork* node, String candidateSsid);
    static AvailableWifiNetwork* insert(AvailableWifiNetwork* node, String candidateSsid);

  private:
    String _ssid;
    AvailableWifiNetwork* _next;
};

class ThreemaRecipient {
  public:
    ThreemaRecipient(const String& to, ThreemaRecipient* next);
    ThreemaRecipient(ThreemaRecipient* other);
    virtual ~ThreemaRecipient();

    const String& to() const;
    ThreemaRecipient* next() const;
  
  private:
    String _to;
    ThreemaRecipient* _next;
};

class ThreemaSettings {
  public:
    ThreemaSettings(const String& from, const String& secret, ThreemaRecipient* recipients);
    ThreemaSettings(ThreemaSettings* other);
    virtual ~ThreemaSettings();

    const String& from() const;
    const String& secret() const;
    ThreemaRecipient* recipients() const;

    void printTo(Stream& stream);

    bool operator==(const ThreemaSettings& other);
    bool operator!=(const ThreemaSettings& other);

  private:
    String _from;
    String _secret;
    ThreemaRecipient* _recipients;
};

class WifiSettings {
  public:
    WifiSettings(const String& ssid, const String& passphrase);
    WifiSettings(const WifiSettings* other);
    virtual ~WifiSettings();
  
    const String& ssid() const;
    const String& passphrase() const;

    void printTo(Stream& stream);
      
    bool operator==(const WifiSettings& other);
    bool operator!=(const WifiSettings& other);

  private:
    const String _ssid;
    const String _passphrase;
};

class MonitoringSettings {
  public:
    MonitoringSettings(int notifyOpenDelayMinutes);
    MonitoringSettings(MonitoringSettings* other);
    virtual ~MonitoringSettings();

    int notifyOpenDelayMinutes() const;

    void printTo(Stream& stream);

    bool operator==(const MonitoringSettings& other);
    bool operator!=(const MonitoringSettings& other);

  private:
    int _notifyOpenDelayMinutes;    
};

class Settings {
  public:
    Settings(
      const String description,
      WifiSettings* wifiSettings,
      MonitoringSettings* monitoringSettings,
      NotificationMethod notificationMethod,
      ThreemaSettings* threemaSettings);
    Settings(Settings* other);
    virtual ~Settings();

    const String& description() const;
    WifiSettings* wifiSettings() const;
    MonitoringSettings* monitoringSettings() const;
    NotificationMethod notificationMethod() const;
    ThreemaSettings* threemaSettings() const;

    void printTo(Stream& stream);

    bool operator==(const Settings& other);
    bool operator!=(const Settings& other);

  private:
    String _description;
    WifiSettings* _wifiSettings;
    MonitoringSettings* _monitoringSettings;
    NotificationMethod _notificationMethod;
    ThreemaSettings* _threemaSettings;
};

#endif // SETTINGS_H
