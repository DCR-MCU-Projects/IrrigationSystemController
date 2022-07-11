// Project has been designed for ESP8266

#define VERSION     "2.3.1002"

#define AUTHOR      "David Cuerrier"
#define CONTACT     "david.cuerrier@gmail.com"
#define COPYRIGHT   "This code may be use as is, all modification need to be reviewed and pushed back into community repository"

// #define HOSTNAME    "irrigationcontroller"
// #define STASSID     "Burton"
// #define STAPSK      "Takeachance01"

#ifdef ESP8266
  
  #include "config.h"

  #include <Arduino.h>
  #include <math.h>
  #include "trace.h"
  #include <ESP8266WiFi.h>
  #include <ESP8266mDNS.h>
  #include <WiFiUdp.h>
  #include <ArduinoOTA.h>
  #include <FlowMeter.h>

  #include <Arduino_JSON.h>
  #include <FS.h>
  
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  
  #include "IrrigationController.h"

  
  // #define PIN_BOOST       D1
  // #define PIN_BOOST_LEVEL A0
  // #define PIN_REVERSER    D2
  
  // #define PIN_STRIKE_1    D6
  // #define PIN_STRIKE_2    D8
  // #define PIN_STRIKE_3    D5
  // #define PIN_STRIKE_4    D7

  // #define PIN_FLOW_SENSOR 3

  bool switchRemoteUpdate = false;

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