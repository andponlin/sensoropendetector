/*
 * Copyright 2023, Andrew Lindesay <apl@lindesay.co.nz>
 * All Rights Reserved. Distributed under the terms of the MIT License.
 */
#include "settingsservice.h"

#include <WiFiNINA.h>

#define SETTINGS_MAGIC 23619


SettingsService::SettingsService() {
  _wifiNetworks = NULL;
}

SettingsService::~SettingsService() {
  delete _wifiNetworks;
}

/*static*/
AvailableWifiNetwork* SettingsService::createWifiNetworks() {
   int numSsid = WiFi.scanNetworks();
   AvailableWifiNetwork* result = NULL;

  if (-1 == numSsid) {
    Serial.println("Couldn't get a WiFi connection");
  }
  else {
    for (int i = 0; i < numSsid; i++) {
      result = AvailableWifiNetwork::insert(result, WiFi.SSID(i));
    }
  }

  return result;
}


AvailableWifiNetwork* SettingsService::wifiNetworks() {
  return _wifiNetworks;
}

void SettingsService::setWifiNetworks(AvailableWifiNetwork* value) {
  if (value != _wifiNetworks) {
    delete _wifiNetworks;
    _wifiNetworks = value;
  }
}

InMemorySettingsService::InMemorySettingsService() {
  _settings = NULL;
}

InMemorySettingsService::~InMemorySettingsService() {
  delete _settings;
}

bool InMemorySettingsService::isEmpty() {
  return NULL == _settings;
}

void InMemorySettingsService::reset() {
   delete _settings;
   _settings = NULL;
}

Settings* InMemorySettingsService::load() {
  return _settings;
}

void InMemorySettingsService::save(Settings* value) {
  if (_settings != value) {
    delete _settings;
    _settings = new Settings(value);
  }
}
