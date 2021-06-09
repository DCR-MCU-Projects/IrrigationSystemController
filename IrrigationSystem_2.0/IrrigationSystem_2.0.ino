
#include "IrrigationSystem_2.0.h"

IrrigationController irrigationController(PIN_REVERSER, PIN_BOOST);
IrrigationJobPayload jobPayload;
IrrigationStats stats;

FlowMeter *Meter;

AsyncWebServer server(80);

long latestMillis = 0;


void setup() {

  pinMode(A0, INPUT);

  Serial.begin(115200);
  Serial.setTimeout(10000);
  Serial.println("\n\n");

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.println("Setting up WiFi ...");
  initWiFi();

  Serial.println("Setting up OTA Update...");
  initOTAUpdate();

  Serial.println("Setting up SPIFFS ...");
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  Serial.println("Setting up FlowMeter ...");
  Meter = new FlowMeter(digitalPinToInterrupt(PIN_FLOW_SENSOR), FS300A, MeterISR, RISING);
  

  
  Serial.println("Setting up Controller ...");
  Serial.println("Add zones...");
  irrigationController.enableController();
  irrigationController.addZone(1, PIN_STRIKE_1);
  irrigationController.addZone(2, PIN_STRIKE_2);
  irrigationController.addZone(3, PIN_STRIKE_3);
  irrigationController.addZone(4, PIN_STRIKE_4);

  Serial.println("Enabling zones...");
  irrigationController.enableZone(1);
  irrigationController.enableZone(2);
  irrigationController.enableZone(3);
  irrigationController.enableZone(4);

  Serial.println("Setting up WebServer...");
  initWebServer();

  Serial.println("You may reach the RestAPI at:");
  
  Serial.print("\thttp://");
  Serial.print(WiFi.localIP().toString());
  Serial.println("/v1/");

  Serial.print("\thttp://");
  Serial.print(HOSTNAME);
  Serial.print(".local");
  Serial.println("/v1/");

}

void loop() {

  if (WiFi.status() != WL_CONNECTED)
    ESP.restart();

  if (jobPayload.needESPUpdate)
    ArduinoOTA.handle();
  else if (jobPayload.needBoost) {
    irrigationController.loadBooster();    
    jobPayload.needBoost = false;
    Serial.println("Boost is done");
  }
  else if (jobPayload.needBoost) {
    irrigationController.loadBooster();    
    jobPayload.needBoost = false;
    Serial.println("Boost is done");
  }
  else if (jobPayload.needTrigReverseOn) {
    irrigationController.reversePolarity(true);    
    jobPayload.needTrigReverseOn = false;
    Serial.println("TrigReverseOn is done");
  }
  else if (jobPayload.needTrigReverseOff) {
    irrigationController.reversePolarity(false);    
    jobPayload.needTrigReverseOff = false;
    Serial.println("TrigReverseOff is done");
  }
  else if (jobPayload.turnZoneOn) {
    if (irrigationController.startZone(jobPayload.zoneId)) {
      Serial.print("turnChannel_");
      Serial.print(jobPayload.zoneId);
      Serial.println("_On is done");
    } else {
      Serial.print("turnChannel_");
      Serial.print(jobPayload.zoneId);
      Serial.println("_On FAILED!");      
    }
    jobPayload.zoneId = -1;
    jobPayload.turnZoneOn = false;
  }
  else if (jobPayload.turnZoneOff) {
    if (irrigationController.stopZone(jobPayload.zoneId)) {
      Serial.print("turnChannel_");
      Serial.print(jobPayload.zoneId);
      Serial.println("_Off is done");
    } else {
      Serial.print("turnChannel_");
      Serial.print(jobPayload.zoneId);
      Serial.println("_Off FAILED!");
    }
    jobPayload.zoneId = -1;
    jobPayload.turnZoneOff = false;
  }


  // Meter->tick(millis() - latestMillis);
  // latestMillis = millis();

  // Serial.println("Currently " + String(Meter->getCurrentFlowrate()) + " l/min, " + String(Meter->getTotalVolume())+ " l total.");
  
  delay(1000);
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  if (!MDNS.begin(HOSTNAME)) {
    Serial.println("Error setting up MDNS responder!");
  }

}

void initWebServer() {

  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(404, "text/plain", "Not found");
  });

  server.on("/a", HTTP_GET, [](AsyncWebServerRequest *request){  
    request->send(SPIFFS, "/stats.json", String(), false, processor);
    //request->send(200, "application/json", "{\"aaaaa\":\"asss\"}");
  });

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  server.on("/flow", HTTP_GET, [](AsyncWebServerRequest *request){
    JSONVar ret;
    double a = 2.19;
    ret[0] = Meter->getCurrentFlowrate();
    ret[1] = Meter->getCurrentVolume();
    ret[2] = Meter->getTotalFlowrate();
    ret[3] = Meter->getTotalVolume();
    ret[4] = a;
    
    String jsonString = JSON.stringify(ret);
    
    request->send(200, "application/json", jsonString);
  });

  server.on("/update", HTTP_PUT, [](AsyncWebServerRequest *request){
    jobPayload.needESPUpdate = true;
    request->send(200, "application/json", "{\"status\": \"You may now update using OAT\"}");
  });
  
  // GET Boost Read
  server.on("/test/boost/read", HTTP_GET, [] (AsyncWebServerRequest *request) {
      char buffer[50];
      sprintf(buffer, "{\"boost\": %d}", jobPayload.latestBoostMesurement);
      request->send(200, "application/json", buffer);
  });

  // GET Boost Read
  server.on("/test/boost", HTTP_POST, [] (AsyncWebServerRequest *request) {
      jobPayload.needBoost = true;
      request->send(200, "application/json", "{\"status\": \"sent the request ...\"}");
  });

  server.on("/test/reverser/on", HTTP_POST, [] (AsyncWebServerRequest *request) {
      jobPayload.needTrigReverseOn = true;
      request->send(200, "application/json", "{\"status\": \"sent the request ...\"}");
  });

  server.on("/test/reverser/off", HTTP_POST, [] (AsyncWebServerRequest *request) {
      jobPayload.needTrigReverseOff = true;
      request->send(200, "application/json", "{\"status\": \"sent the request ...\"}");
  });


  server.on("/zone/on", HTTP_POST, [] (AsyncWebServerRequest *request) {

    if (jobPayload.turnZoneOff || jobPayload.turnZoneOn)
      request->send(304, "application/json", "{\"status\": \"There is already a request to execute on another zone. Please wait and resubmit.\"}");
    else {
      if (request->hasParam("id", true)) {
        short sensorId = (short) atoi(request->getParam("id", true)->value().c_str());
        jobPayload.turnZoneOn = true;
        jobPayload.zoneId = sensorId;
        request->send(200, "application/json", "{\"status\": \"sent the request ...\"}");
      }
      request->send(409, "application/json", "{\"status\": \"Missing id param\"}");
    }
    
  });

  server.on("/zone/off", HTTP_POST, [] (AsyncWebServerRequest *request) {

    if (jobPayload.turnZoneOff || jobPayload.turnZoneOn)
      request->send(304, "application/json", "{\"status\": \"There is already a request to execute on another zone. Please wait and resubmit.\"}");
    else {
      if (request->hasParam("id", true)) {
        short sensorId = (short) atoi(request->getParam("id", true)->value().c_str());
        jobPayload.turnZoneOff = true;
        jobPayload.zoneId = sensorId;
        request->send(200, "application/json", "{\"status\": \"sent the request ...\"}");
      }
      request->send(409, "application/json", "{\"status\": \"Missing id param\"}");
    }
    
  });

    
  server.begin();
}

void initOTAUpdate() {
  ArduinoOTA.setHostname(HOSTNAME);
  
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


IRAM_ATTR void MeterISR() { Meter->count(); }

String processor(const String& var) {
  if(var == "BOOST_LEVEL")
    return F("");
  else if (var == "CONTROLLER_STATUS")
    return F("");
  else if (var == "CONTROLLER_STATE")
    return F("");
  else if (var == "ACTIVE_ZONE")
    return F("");

  else if (var == "ZONE_1_NAME")
    return F("");
  else if (var == "ZONE_1_STATUS")
    return F("");
  else if (var == "ZONE_1_STATE")
    return F("");
  else if (var == "ZONE_1_FLOW_LAST_MIN")
    return F("");

  else if (var == "ZONE_2_NAME")
    return F("");
  else if (var == "ZONE_2_STATUS")
    return F("");
  else if (var == "ZONE_2_STATE")
    return F("");
  else if (var == "ZONE_2_FLOW_LAST_MIN")
    return F("");

  else if (var == "ZONE_3_NAME")
    return F("");
  else if (var == "ZONE_3_STATUS")
    return F("");
  else if (var == "ZONE_3_STATE")
    return F("");
  else if (var == "ZONE_3_FLOW_LAST_MIN")
    return F("");

  else if (var == "ZONE_4_NAME")
    return F("");
  else if (var == "ZONE_4_STATUS")
    return F("");
  else if (var == "ZONE_4_STATE")
    return F("");
  else if (var == "ZONE_4_FLOW_LAST_MIN")
    return F("");


  return F("UNDEFINED");
}