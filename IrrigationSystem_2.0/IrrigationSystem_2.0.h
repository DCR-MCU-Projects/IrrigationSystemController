// Project has been designed for ESP8266

#define AUTHOR      "David Cuerrier"
#define CONTACT     "david.cuerrier@gmail.com"
#define COPYRIGHT   "This code may be use as is, all modification need to be reviewed and pushed back into community repository"

#define HOSTNAME    "IrrigationController"
#define STASSID     "Burton"
#define STAPSK      "Takeachance01"

#ifdef ESP8266
  
  #include <Arduino.h>
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
  
  #define PIN_BOOST       D1
  #define PIN_BOOST_LEVEL A0
  #define PIN_REVERSER    D2
  
  #define PIN_STRIKE_1    D6
  #define PIN_STRIKE_2    D8
  #define PIN_STRIKE_3    D5
  #define PIN_STRIKE_4    D7

  #define PIN_FLOW_SENSOR 3

  void setup();
  void loop();

  void initOTAUpdate();
  void initWiFi();
  void initWebServer();

  String processor(const String& var);
  IRAM_ATTR void MeterISR();

  enum STATUS {
    IDLE = 0x0,
    STARTING = 0x1,
    RUNNING = 0x2,
    STOPPING = 0x3
  };

  enum STATE {
    ENABLE = 0x0,
    DISABLE = 0x1
  };

  enum ZONEID {
    ZONE1 = 0x0,
    ZONE2 = 0x1,
    ZONE3 = 0x2,
    ZONE4 = 0x3
  };

  struct Zone {
    String name;
    STATUS status;
    STATE state;
    double flowLastMin;
  }

  struct IrrigationStats {
    double boostLevel;
    STATUS controllerStatus;
    STATE controllerState;
    ZONEID activeZone;

    Zone[] = zones;

  };

  struct IrrigationJobPayload {

      bool needESPUpdate = false;
      
      bool needBoost = false;
      
      bool needTrigReverseOn = false;
      bool needTrigReverseOff = false;

      bool turnZoneOn = false;
      bool turnZoneOff = false;

      short zoneId = -1;
      
      short latestBoostMesurement = -1; 
  };

#endif
