/*
 * Copyright 2023, Andrew Lindesay <apl@lindesay.co.nz>
 * All Rights Reserved. Distributed under the terms of the MIT License.
 */

#ifndef SETTINGSSERVICE_H
#define SETTINGSSERVICE_H

#include <Arduino.h>
#include <WiFiNINA.h>

#include "settings.h"

class Settings;
class AvailableWifiNetwork;

/*
The settings service is a service which is able to store and retrieve the settings.
It may load and store those from memory but it would equally be feasible to read
and write those to some non-volatile storage mechanism such as EEPROM or an SD-Card.
*/

class SettingsService {
  public:
    SettingsService();
    virtual ~SettingsService();

    virtual bool isEmpty() = 0;
    virtual void reset() = 0;
    virtual Settings* load() = 0;
    virtual void save(Settings* value) = 0;

    AvailableWifiNetwork* wifiNetworks();
    void setWifiNetworks(AvailableWifiNetwork* value);

    static AvailableWifiNetwork* createWifiNetworks();

  private:
    AvailableWifiNetwork* _wifiNetworks;
};

class InMemorySettingsService : public SettingsService {
  public:
    InMemorySettingsService();
    virtual ~InMemorySettingsService();

    virtual bool isEmpty();
    virtual void reset();
    virtual Settings* load();
    virtual void save(Settings* value);    

  private:
    Settings* _settings;
};

#endif // SETTINGSSERVICE_H
