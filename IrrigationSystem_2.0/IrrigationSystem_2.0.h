// Project has been designed for ESP8266

#define VERSION     "2.4.0"

#define AUTHOR      "David Cuerrier"
#define CONTACT     "david.cuerrier@gmail.com"
#define COPYRIGHT   "This code may be use as is, all modification need to be reviewed and pushed back into community repository"

#ifdef ESP8266
  
  #include "config.h"

  #include <Arduino.h>
  #include <math.h>
  #include "trace.h"
  #include <ESP8266WiFi.h>
  #include <ESP8266mDNS.h>
  // #include <WiFiUdp.h>
  #include <ArduinoOTA.h>
  #include <FlowMeter.h>

  #include <Arduino_JSON.h>
  
  #include <FS.h>
  #include <LittleFS.h>
  
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  
  #include "IrrigationController.h"

  bool switchRemoteUpdate = false;
  bool OTAUpdateOnProgress = false;
  unsigned int OTAUpdateProgress = 0;

  void setup();
  void loop();

  void initSerialCom(long speed, long timeout);
  void initOTAUpdate();
  void initWiFi();
  void initWebServer();
  void initController();

  String processor(const String& var);
  IRAM_ATTR void MeterISR();

  struct Config {
    short zone_count;
  };

#endif