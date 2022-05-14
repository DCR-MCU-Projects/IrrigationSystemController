
#include "IrrigationSystem_2.0.h"

IrrigationController irrigationController(PIN_REVERSER, PIN_BOOST);

IrrigationZone* zone[8];

//FlowMeter *Meter;

AsyncWebServer server(80);

long latestMillis = 0;


void setup() {

  pinMode(A0, INPUT);

  Serial.begin(115200);
  Serial.setTimeout(10000);
  Serial.println("");
  Serial.println("");

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

  // Serial.println("Setting up FlowMeter ...");
  // Meter = new FlowMeter(digitalPinToInterrupt(PIN_FLOW_SENSOR), FS300A, MeterISR, RISING);
  
  
  Serial.println("Setting up Controller ...");
  irrigationController.setState(ENABLE);
  irrigationController.setStatus(IDLE);

  Serial.println("Add zones...");

  zone[0] = new IrrigationZone(0, PIN_STRIKE_1);
  zone[1] = new IrrigationZone(1, PIN_STRIKE_2);
  zone[2] = new IrrigationZone(2, PIN_STRIKE_3);
  zone[3] = new IrrigationZone(3, PIN_STRIKE_4);

  zone[0]->state = ENABLE;
  zone[1]->state = ENABLE;
  zone[2]->state = ENABLE;
  zone[3]->state = ENABLE;

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
  
  MDNS.update();
  
  delay(300);

  if (WiFi.status() != WL_CONNECTED)
    ESP.restart();

  //Meter->tick(300);

  // if (irrigationController.getActiveZone() != NULL)
  //   irrigationController.getActiveZone()->flow = Meter->getCurrentFlowrate();

  irrigationController.handleRequests();

  if (switchRemoteUpdate)
    ArduinoOTA.handle();
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

  MDNS.addService("http", "tcp", 80);
}

void initWebServer() {

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(404, "text/plain", "Endpoint your are looking for does not exist.");
  });

  /* Enable Over The Air update mode */
  server.on("/ota-update", HTTP_PUT, [](AsyncWebServerRequest *request){
    switchRemoteUpdate = true;
    request->send(200, "application/json", "{\"status\": \"You may now update using OTA\"}");
  }).setAuthentication("user", "pass");

  server.on("/stats", HTTP_GET, [](AsyncWebServerRequest *request){  

    request->send(SPIFFS, "/stats.json", String(), false, processor);
  });

  /* Modify an existing zone */
  server.on("/zone", HTTP_PUT, [](AsyncWebServerRequest *request){  
    
    if (request->hasParam("action")) {
      
      String action = request->getParam("action")->value().c_str();
      action.trim();

      if (request->hasParam("id")) {
        
        short id = (short) atoi(request->getParam("id")->value().c_str());

        if (action == "rename") {
          if (request->hasParam("name", true)) {
            zone[id]->name = request->getParam("name", true)->value().c_str();
            request->send(204);
          }
          else
            request->send(404, "application/json", "{\"status\": \"Missing argument: name.\"}");
        } else if (action == "enable") {
            zone[id]->state = ENABLE;
            request->send(204);
        } else if (action == "disable") {
            zone[id]->state = DISABLE;
            request->send(204);
        } else
          request->send(404, "application/json", "{\"status\": \"The action you requested does not exist here.\"}");
      } else
        request->send(400, "application/json", "{\"status\": \"You need to specify a zone id.\"}");
    } else
      request->send(400, "application/json", "{\"status\": \"You need to specify an action.\"}");

  });

  server.on("/zone/start", HTTP_POST, [](AsyncWebServerRequest *request){

    if (irrigationController.getBoostLevel() < 90)
      request->send(405, "application/json", "{\"status\": \"Booster is running low, please wait and try again soon.\"}");

    if (irrigationController.getStatus() == IDLE) {
      if (irrigationController.getActiveZone() == NULL) {
        if (request->hasParam("id")) {
          short id = (short) atoi(request->getParam("id")->value().c_str());
          irrigationController.startZone(zone[id]);
          //request->send(204);
          request->redirect("/stats");
        } else
          request->send(400, "application/json", "{\"status\": \"You need to specify a zone id.\"}");

      } else
        request->send(409, "application/json", "{\"status\": \"The active zone is not NULL, seems like the controller is already in use...\"}");

    } else
      request->send(409, "application/json", "{\"status\": \"The controller is not in IDLE state.\"}");

  });

  server.on("/zone/stop", HTTP_POST, [](AsyncWebServerRequest *request){  

      if (irrigationController.getBoostLevel() < 90)
        request->send(405, "application/json", "{\"status\": \"Booster is running low, please wait and try again soon.\"}");
    
    // if (irrigationController.getStatus() == RUNNING) {

        if (request->hasParam("id")) {
          short id = (short) atoi(request->getParam("id")->value().c_str());
          // if (zone[id]->status == RUNNING) {
            irrigationController.stopZone(zone[id]);
            request->send(204);
          // } else
          //   request->send(409, "application/json", "{\"status\": \"The specified zone is not in RUNNING state.\"}");
        } else
          request->send(400, "application/json", "{\"status\": \"You need to specify a zone id.\"}");


    // } else
    //   request->send(409, "application/json", "{\"status\": \"The controller is not in RUNNING state.\"}");

  });


  // server.on("/flow", HTTP_GET, [](AsyncWebServerRequest *request){
  //   JSONVar ret;
  //   double a = 2.19;
  //   ret[0] = Meter->getCurrentFlowrate();
  //   ret[1] = Meter->getCurrentVolume();
  //   ret[2] = Meter->getTotalFlowrate();
  //   ret[3] = Meter->getTotalVolume();
  //   ret[4] = a;
    
  //   String jsonString = JSON.stringify(ret);
    
  //   request->send(200, "application/json", jsonString);
  // });

    
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


// IRAM_ATTR void MeterISR() { Meter->count(); }

String processor(const String& var) {
  if(var == "VERSION"){
    return String(VERSION);
  }
  else if(var == "BOOST_LEVEL"){
    return String(irrigationController.getBoostLevel());
  }
  else if (var == "CONTROLLER_STATUS") {
    return String(irrigationController.getStatus());
  }
  else if (var == "CONTROLLER_STATE") {
    return String(irrigationController.getState());
  }
  else if (var == "ACTIVE_ZONE") {
    if (irrigationController.getActiveZone() != NULL)
      return String(irrigationController.getActiveZone()->id);
    else
      return "-1";
  }

  else if (var == "ZONE_0_NAME")
    return String(zone[0]->name);
  else if (var == "ZONE_0_STATUS")
    return String(zone[0]->status);
  else if (var == "ZONE_0_STATE")
    return String(zone[0]->state);
  else if (var == "ZONE_0_FLOW_LAST_MIN")
    return String(zone[0]->flow);

  else if (var == "ZONE_1_NAME")
    return String(zone[1]->name);
  else if (var == "ZONE_1_STATUS")
    return String(zone[1]->status);
  else if (var == "ZONE_1_STATE")
    return String(zone[1]->state);
  else if (var == "ZONE_1_FLOW_LAST_MIN")
    return String(zone[1]->flow);

  else if (var == "ZONE_2_NAME")
    return String(zone[2]->name);
  else if (var == "ZONE_2_STATUS")
    return String(zone[2]->status);
  else if (var == "ZONE_2_STATE")
    return String(zone[2]->state);
  else if (var == "ZONE_2_FLOW_LAST_MIN")
    return String(zone[2]->flow);

  else if (var == "ZONE_3_NAME")
    return String(zone[3]->name);
  else if (var == "ZONE_3_STATUS")
    return String(zone[3]->status);
  else if (var == "ZONE_3_STATE")
    return String(zone[3]->state);
  else if (var == "ZONE_3_FLOW_LAST_MIN")
    return String(zone[3]->flow);


  return F("UNDEFINED");
}
