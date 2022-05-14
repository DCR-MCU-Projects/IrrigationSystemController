#include "IrrigationController.h"


IrrigationController::IrrigationController(short polarityReverserPin, short relayBoosterPin) {

  _polarityReverserPin = polarityReverserPin;
  _relayBoosterPin = relayBoosterPin;
  
  pinMode(polarityReverserPin, OUTPUT);
  pinMode(relayBoosterPin, OUTPUT);
  
  digitalWrite(relayBoosterPin, LOW);
  digitalWrite(polarityReverserPin, LOW);
}

void IrrigationController::reversePolarity(bool state) {
  
  digitalWrite(IrrigationController::_polarityReverserPin, state);
}

void IrrigationController::startBooster() {
  digitalWrite(_relayBoosterPin, HIGH);
}

void IrrigationController::stopBooster() {
  digitalWrite(_relayBoosterPin, LOW);
}

void IrrigationController::startZoneON(IrrigationZone* zone) {
  reversePolarity(true);
  digitalWrite(zone->pin, HIGH);
  
  setStatus(STARTING);
  zone->status = STARTING;

  activeZone = zone;
  trace("Starting Zone ON");
}
void IrrigationController::startZoneOFF(IrrigationZone* zone) {
  reversePolarity(false);
  digitalWrite(zone->pin, LOW);
  setStatus(RUNNING);
  zone->status = RUNNING;
  jobPlan.action = NONE;
  trace("Starting Zone OFF");
}
void IrrigationController::stopZoneON(IrrigationZone* zone) {
  digitalWrite(zone->pin, HIGH);
  setStatus(STOPPING);
  zone->status = STOPPING;
  trace("Stopping Zone ON");
}
void IrrigationController::stopZoneOFF(IrrigationZone* zone) {
  digitalWrite(zone->pin, LOW);
  setStatus(IDLE);
  zone->status = IDLE;
  activeZone = NULL;
  jobPlan.action = NONE;
  trace("Stopping Zone OFF");
}

short IrrigationController::getBoostLevel() {
  return map(analogRead(A0), 0, 660, 0, 100);
}

bool IrrigationController::stopZone(IrrigationZone* zone) { 
  trace("Stop zone");
  if ((getState() == ENABLE)) {
    if ((zone->state == ENABLE)) {
      jobPlan.action = STOPZONE;
      jobPlan.zone = zone;
      trace("Stop zone done");
    }
  }
  return true;
}
bool IrrigationController::startZone(IrrigationZone* zone) {
  trace("Start zone");
  if ((getState() == ENABLE) && (getStatus() == IDLE)) {
  
    if ((zone->state == ENABLE) && (zone->status == IDLE)) {
  
      jobPlan.action = STARTZONE;
  
      jobPlan.zone = zone;
      trace("Start zone done");
    }
  
  }
  
  return true;
}

// This method should be called from the main thread LOOP method. It will manage all request in the pipeline.
void IrrigationController::handleRequests() {

  if (getState() == ENABLE) {

    // Handle auto-boosting when needed
    //   only if controller is not starting or stopping a zone.
    // START AUTO-BOOST
    if (getBoostLevel() < 90) {
      if ((getStatus() == IDLE) || (getStatus() == RUNNING)) {
          trace(__FILE__, INFO, "Booster is low charge, initiating load...");
          trace(__FILE__, TRACE, "Start boost charger");
          startBooster();
          setStatus(BOOSTING);
          actionTimeout = millis() + 5000;
      }
    } 
    
    if (getStatus() == BOOSTING) {
      if (millis() >= actionTimeout) {
        trace(__FILE__, TRACE, "Stop boost charger");
        stopBooster();
        setStatus(IDLE);
        actionTimeout = 0;
      }
    }
    // END AUTO-BOOST

    // We have somthing to do !!
    if (jobPlan.action != NONE) {

      // Are we ready to start a NEW action ?
      if ((getStatus() == IDLE) || (getStatus() == RUNNING)) {
        if (jobPlan.action == STARTZONE) {
          //setStatus(STARTING);
          jobPlan.timeout = millis() + 2000;
          startZoneON(jobPlan.zone);
        }
        else if (jobPlan.action == STOPZONE) {
          //setStatus(STOPPING);
          jobPlan.timeout = millis() + 2000;
          stopZoneON(jobPlan.zone);
        }
      }

      // If this is not a new action, we need to complete the action sequence !      
      else if (getStatus() == STARTING) {
        if (millis() >= jobPlan.timeout) {
          startZoneOFF(jobPlan.zone);
          jobPlan.timeout = 0;
          //setStatus(IDLE);
          jobPlan.action = NONE;
        }
      }
      else if (getStatus() == STOPPING) {
        if (millis() >= jobPlan.timeout) {
          stopZoneOFF(jobPlan.zone);
          jobPlan.timeout = 0;
          //setStatus(IDLE);
          jobPlan.action = NONE;
        }
      }
    }
  }
}

