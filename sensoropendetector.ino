/*
 * Copyright 2023, Andrew Lindesay <apl@lindesay.co.nz>
 * All Rights Reserved. Distributed under the terms of the MIT License.
 */

#include <WiFiNINA.h>

#include "ArduinoLowPower.h"

#include "constants.h"
#include "debouncedigitalinput.h"
#include "sensorservice.h"
#include "notificationservice.h"
#include "settingsservice.h"
#include "indicatorservice.h"

#include "staticsettings.h"

enum StateMachine {
  START,
  WATCH
};

int wifiStatus = WL_IDLE_STATUS;
SettingsService* settingsService = NULL;
NotificationService* notificationService = NULL;
SensorService* sensorService = NULL;
IndicatorService* indicatorService = NULL;
DebouncedDigitalInput* buttonInput = NULL;
DebouncedDigitalInput* sensorInput = NULL;
StateMachine stateMachine = START;

void printWiFiStatus() {
#ifdef SERIAL_ENABLED
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
#endif
}

void setupSerial() {
#ifdef SERIAL_ENABLED
  unsigned long now = millis();
  Serial.begin(9600);

  while (!Serial && (millis() - now) < 10000L) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
#endif
}

void setupWifi() {
  if (WiFi.status() == WL_NO_MODULE) {
  #ifdef SERIAL_ENABLED
    Serial.println("Communication with WiFi module failed!");
  #endif
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();

  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
#ifdef SERIAL_ENABLED
    Serial.print("Please upgrade the firmware; current [");
    Serial.print(fv);
    Serial.print("] expected [");
    Serial.print(WIFI_FIRMWARE_LATEST_VERSION);
    Serial.println("]");
#endif
    // don't continue
    while (true);
  }
}

void setupServices() {
  if (NULL == indicatorService) {
    indicatorService = new IndicatorService(PIN_LED);
  }
  if (NULL == notificationService || NULL == sensorService) {
    Settings* settings = settingsService->load();
    if (NULL == notificationService) {
      notificationService = createNotificationService(settings);
    }
    if (NULL == sensorService) {
      sensorService = new SensorService(
        new MonitoringSettings(settings->monitoringSettings()),
        notificationService,
        indicatorService
      );
    }
    delete settings;
  }
}

void setup() {

  setupSerial();

#ifdef SERIAL_ENABLED
  Serial.println("will setup");
#endif

  pinMode(PIN_LED, OUTPUT);

  pinMode(PIN_SENSOR, INPUT_PULLUP);
  LowPower.attachInterruptWakeup(PIN_SENSOR, awakeFromSleep, CHANGE);
  sensorInput = new DebouncedDigitalInput(PIN_SENSOR);

  pinMode(PIN_BUTTON, INPUT_PULLUP);
  LowPower.attachInterruptWakeup(PIN_BUTTON, awakeFromSleep, CHANGE);
  buttonInput = new DebouncedDigitalInput(PIN_BUTTON);

  setupWifi();

  settingsService = new InMemorySettingsService();
  settingsService->save(STATICSETTINGS);
  sensorService = NULL;
  notificationService = NULL;

#ifdef SERIAL_ENABLED
  Serial.println("did setup");
#endif
}

NotificationService* createNotificationService(Settings* settings) {
  switch (settings->notificationMethod()) {
    case THREEMA:
      return new ThreemaNotificationService(
        settings->description(),
        settings->wifiSettings(),
        settings->threemaSettings()
      );
    case LOG:
      return new LogNotificationService();
    default:
#ifdef SERIAL_ENABLED
      Serial.println("unknown notification method");
#endif
      return new LogNotificationService();
  }
}

void handleSensor() {
  sensorInput->pulse();
  bool newState = sensorInput->getState();
  sensorService->update(newState);
}

void handleButton() {
  bool priorState = buttonInput->getState();
  buttonInput->pulse();
  if (priorState && !buttonInput->getState()) {
#ifdef SERIAL_ENABLED
    Serial.println("button was depressed");
#endif
    sensorService->togglePause();
  }
}

void handleIndicator() {
  indicatorService->pulse();
}

void handleLoopDelay() {
  if (
      sensorService->allowedToShortSleep()
      && buttonInput->allowedToShortSleep()
      && sensorInput->allowedToShortSleep()
  ) {
#ifdef SERIAL_ENABLED
    Serial.println("deep sleep..");
    Serial.end();
#endif
    LowPower.deepSleep();
    setupSerial();
  }
}

void awakeFromSleep() {
  // Serial.println("..awake on pin");
}

void loop() {
  switch (stateMachine) {
    case START:
      delete sensorService;
      delete notificationService;
      sensorService = NULL;
      notificationService = NULL;
      stateMachine = WATCH;
      break;
    case WATCH:
      setupServices();
      handleButton();
      handleSensor();
      handleIndicator();
      handleLoopDelay();
      break;
  }
}
