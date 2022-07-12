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
  #include <ArduinoOTA.h>
  #include <FlowMeter.h>

  #include <Arduino_JSON.h>
  #include <LittleFS.h>

  #include <DNSServer.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>

  #include "IrrigationController.h"

  bool switchRemoteUpdate = false;
  bool OTAUpdateOnProgress = false;
  bool setupMode = false;

  unsigned int OTAUpdateProgress = 0;

  void setup();
  void loop();

  void initSerialCom(long speed, long timeout);
  void initOTAUpdate();
  void initWiFi();
  void initWebServer();
  void initController();
  void initBoard();
  void initLittleFS();

  String processor(const String& var);
  IRAM_ATTR void MeterISR();

  class CaptiveRequestHandler : public AsyncWebHandler {
  public:
    CaptiveRequestHandler() {}
    virtual ~CaptiveRequestHandler() {}

    bool canHandle(AsyncWebServerRequest *request){
      request->addInterestingHeader("ANY");
      return true;
    }

    void handleRequest(AsyncWebServerRequest *request) {
      AsyncResponseStream *response = request->beginResponseStream("text/html");
      response->print("<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>");
      response->print("<p>This is out captive portal front page.</p>");
      response->printf("<p>You were trying to reach: http://%s%s</p>", request->host().c_str(), request->url().c_str());
      response->printf("<p>Try opening <a href='http://%s'>this link</a> instead</p>", WiFi.softAPIP().toString().c_str());
      response->print("</body></html>");
      request->send(response);
    }
  };

#endif