/*
 * Copyright 2023, Andrew Lindesay <apl@lindesay.co.nz>
 * All Rights Reserved. Distributed under the terms of the MIT License.
 */
#include "settings.h"

#include <Arduino.h>

AvailableWifiNetwork::AvailableWifiNetwork(String ssid, AvailableWifiNetwork* next)
  :
  _ssid(ssid),
  _next(next) {
}

AvailableWifiNetwork::~AvailableWifiNetwork() {
  delete _next;
}
  
const String AvailableWifiNetwork::ssid() const {
  return _ssid;
}

AvailableWifiNetwork* AvailableWifiNetwork::next() const {
  return _next;
}

void AvailableWifiNetwork::setNext(AvailableWifiNetwork* node) {
  _next = node;
}

/*
This will return the node that is ordered direcly before the `ssid` provided.
*/

/*static*/ AvailableWifiNetwork* AvailableWifiNetwork::lowerBound(
  AvailableWifiNetwork* node, String candidateSsid) {

  if (NULL == node) {
    return NULL;
  }

  String nodeSsidLc = node->ssid();
  String candidateSsidLc = candidateSsid;
  nodeSsidLc.toLowerCase();
  candidateSsidLc.toLowerCase();

  if (nodeSsidLc >= candidateSsidLc) {
    return NULL;
  }

  AvailableWifiNetwork* nextLowerBound = lowerBound(node->next(), candidateSsid);

  if (NULL == nextLowerBound) {
    return node;
  }

  return nextLowerBound;
}

/*static*/ AvailableWifiNetwork* AvailableWifiNetwork::insert(
  AvailableWifiNetwork* node, String candidateSsid) {
    AvailableWifiNetwork* lowerBoundNode = lowerBound(node, candidateSsid);

    if (NULL == lowerBoundNode) {
      return new AvailableWifiNetwork(candidateSsid, node);
    }

    AvailableWifiNetwork* newNode = new AvailableWifiNetwork(candidateSsid, lowerBoundNode->next());
    lowerBoundNode->setNext(newNode);

    return node;
}

ThreemaRecipient::ThreemaRecipient(const String& to, ThreemaRecipient* next)
  :
  _to(to),
  _next(next) {
}

ThreemaRecipient::ThreemaRecipient(ThreemaRecipient* other)
  :
  _to(other->to()),
  _next(NULL) {
    if (NULL != other->next()) {
      _next = new ThreemaRecipient(other->next());
    }
}

ThreemaRecipient::~ThreemaRecipient() {
  delete _next;
}

const String& ThreemaRecipient::to() const {
  return _to;
}

ThreemaRecipient* ThreemaRecipient::next() const {
  return _next;
}

ThreemaSettings::ThreemaSettings(
  const String& from, const String& secret, ThreemaRecipient* recipients)
  :
  _from(from),
  _secret(secret),
  _recipients(recipients) {
}

ThreemaSettings::ThreemaSettings(ThreemaSettings* other)
  :
  _from(other->from()),
  _secret(other->secret()),
  _recipients(new ThreemaRecipient(other->recipients())) {
}

ThreemaSettings::~ThreemaSettings() {
  delete _recipients;
}

const String& ThreemaSettings::from() const {
  return _from;
}

const String& ThreemaSettings::secret() const {
  return _secret;
}

ThreemaRecipient* ThreemaSettings::recipients() const {
  return _recipients;
}

void ThreemaSettings::printTo(Stream& stream) {
  stream.print("{");
  stream.print("from:");
  stream.print(from());
  stream.print(",secret:");
  stream.print(secret());
  stream.print(",recipients:");

  ThreemaRecipient* node = recipients();

  while (NULL != node) {
    stream.print(node->to());
    node = node->next();
    if (NULL != node) {
      stream.print(",");
    }
  }
  
  stream.print("}");
}

bool ThreemaSettings::operator==(const ThreemaSettings& other) {
  if (from() != other.from() || secret() != other.secret()) {
    return false;
  }

  ThreemaRecipient* myRecipientNode = recipients();
  ThreemaRecipient* otherRecipientNode = other.recipients();

  while (true) {
    if (NULL == myRecipientNode || NULL == otherRecipientNode) {
      return (NULL == myRecipientNode) == (NULL == otherRecipientNode);
    }
    if (myRecipientNode->to() != otherRecipientNode->to()) {
      return false;
    }
    myRecipientNode = myRecipientNode->next();
    otherRecipientNode = otherRecipientNode->next();
  }
}

bool ThreemaSettings::operator!=(const ThreemaSettings& other) {
  return !(*this == other);
}

WifiSettings::WifiSettings(const String& ssid, const String& passphrase)
  :
  _ssid(ssid), 
  _passphrase(passphrase) {
}

WifiSettings::WifiSettings(const WifiSettings* other)
  :
  _ssid(other->ssid()),
  _passphrase(other->passphrase()) {
}

WifiSettings::~WifiSettings() {
}

const String& WifiSettings::ssid() const {
  return _ssid;
}

const String& WifiSettings::passphrase() const {
  return _passphrase;
}

void WifiSettings::printTo(Stream& stream) {
  stream.print("{");
  stream.print("ssid:");
  stream.print(ssid());
  stream.print(",passphrase:");
  stream.print(passphrase());
  stream.print("}");
}

bool WifiSettings::operator==(const WifiSettings& other) {
  return (ssid() == other.ssid()) && (passphrase() == other.passphrase());
}

bool WifiSettings::operator!=(const WifiSettings& other) {
  return !(*this == other);
}

MonitoringSettings::MonitoringSettings(int notifyOpenDelayMinutes)
  :
  _notifyOpenDelayMinutes(notifyOpenDelayMinutes) {
}

MonitoringSettings::MonitoringSettings(MonitoringSettings* other)
  :
  _notifyOpenDelayMinutes(other->notifyOpenDelayMinutes())
{
}

MonitoringSettings::~MonitoringSettings() {
}

int MonitoringSettings::notifyOpenDelayMinutes() const {
  return _notifyOpenDelayMinutes;
}

void MonitoringSettings::printTo(Stream& stream) {
  stream.print("{");
  stream.print("notifyOpenDelayMinutes:");
  stream.print(notifyOpenDelayMinutes());
  stream.print("}");
}

bool MonitoringSettings::operator==(const MonitoringSettings& other) {
  return notifyOpenDelayMinutes() == other.notifyOpenDelayMinutes();
}

bool MonitoringSettings::operator!=(const MonitoringSettings& other) {
  return !(*this == other);
}

Settings::Settings(
  const String description,
  WifiSettings* wifiSettings,
  MonitoringSettings* monitoringSettings,
  NotificationMethod notificationMethod,
  ThreemaSettings* threemaSettings)
  :
  _description(description),
  _wifiSettings(wifiSettings),
  _monitoringSettings(monitoringSettings),
  _notificationMethod(notificationMethod),
  _threemaSettings(threemaSettings) {
}
  
Settings::Settings(Settings* other)
  :
  _description(other->description()),
  _wifiSettings(new WifiSettings(other->wifiSettings())),
  _monitoringSettings(new MonitoringSettings(other->monitoringSettings())),
  _notificationMethod(other->notificationMethod()),
  _threemaSettings(new ThreemaSettings(other->threemaSettings())) {
}

Settings::~Settings() {
  delete _wifiSettings;
  delete _monitoringSettings;
  delete _threemaSettings;
}

const String& Settings::description() const {
  return _description;  
}

WifiSettings* Settings::wifiSettings() const {
  return _wifiSettings;
}

MonitoringSettings* Settings::monitoringSettings() const {
  return _monitoringSettings;
}

NotificationMethod Settings::notificationMethod() const {
  return _notificationMethod;  
}

ThreemaSettings* Settings::threemaSettings() const {
  return _threemaSettings;
}

void Settings::printTo(Stream& stream) {
  stream.println("{");
  stream.print("description:");
  stream.print(description());
  stream.print(",\nwifiSettings:");
  wifiSettings()->printTo(stream);
  stream.print(",\nmonitoringSettings:");
  monitoringSettings()->printTo(stream);
  stream.print(",\nnotificationMethod:");
  stream.print(Common::notificationMethodAsString(notificationMethod()));
  stream.print(",\nthreemaSettings:");
  threemaSettings()->printTo(stream);
  stream.println("\n}");
}

bool Settings::operator==(const Settings& other) {
  if (description() != other.description()
    || (NULL == wifiSettings()) != (NULL == other.wifiSettings())
    || (NULL == monitoringSettings()) != (NULL == other.monitoringSettings())
    || (NULL == threemaSettings()) != (NULL == other.threemaSettings())) {
      return false;
    }

    if (NULL != wifiSettings()) {
      if (*wifiSettings() != *(other.wifiSettings())) {
        return false;
      }
    }

    if (NULL != monitoringSettings()) {
      if (*monitoringSettings() != *(other.monitoringSettings())) {
        return false;
      }
    }

    if (NULL != threemaSettings()) {
      if (*threemaSettings() != (other.threemaSettings())) {
        return false;
      }
    }

    return true;
}

bool Settings::operator!=(const Settings& other) {
  return !(*this == other);
}
