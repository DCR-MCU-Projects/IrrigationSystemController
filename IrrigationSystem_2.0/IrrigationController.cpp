#include "IrrigationController.h"


IrrigationController::IrrigationController(short polarityReverserPin, short relayBoosterPin) {

  _polarityReverserPin = polarityReverserPin;
  _relayBoosterPin = relayBoosterPin;
  
  pinMode(polarityReverserPin, OUTPUT);
  pinMode(relayBoosterPin, OUTPUT);
  
  digitalWrite(relayBoosterPin, LOW);
  digitalWrite(polarityReverserPin, LOW);

}

bool IrrigationController::isEnable() {
  return _controllerEnable;
}

bool IrrigationController::loadBooster() {
  if (isEnable()) {
    digitalWrite(_relayBoosterPin, HIGH);
    delay(5000);
    digitalWrite(_relayBoosterPin, LOW);
    return true;
  }
  Serial.println("Boost failed");
  return false;
}

short IrrigationController::getBoostLevel() {
  return map(analogRead(A0), 0, 660, 0, 100);
}

bool IrrigationController::reversePolarity(bool state) {
  if (isEnable() || !state) {
    Serial.print("Test reverse on pin ");
    Serial.print(IrrigationController::_polarityReverserPin);
    Serial.print(" with state ");
    Serial.print(state);
    digitalWrite(IrrigationController::_polarityReverserPin, state);
    return true;
  }
  return false;
}

bool IrrigationController::addZone(short id, short pin) {
  Serial.print("Add ZONE: ");
  Serial.print(id);
  Serial.print(" on PIN: ");
  Serial.println(pin);

  zoneCollection[id] = IrrigationZone(id, pin);
    
  return true;
  
}

bool IrrigationController::deleteZone(short id) {
  zoneCollection[id] = IrrigationZone();
  return true;
    
}

void IrrigationController::enableController() {
  _controllerEnable = true;
}

void IrrigationController::disableController() {

  for (int i = 0; i < MAX_ZONE; i++) {

    if (zoneCollection[i].isEnable()) {
      if (loadBooster()) {
        if (IrrigationController::reversePolarity(true)) {
          zoneCollection[i].strike();
          zoneCollection[i].disable();
        }
        reversePolarity(false);
      }
    }
    
  }
  
  IrrigationController::_controllerEnable = false;
}

void IrrigationController::enableZone(short id) { 
  zoneCollection[id].enable();
}
void IrrigationController::disableZone(short id) { 
  zoneCollection[id].disable();
}
bool IrrigationController::stopZone(short id) { 
  if (isEnable() && loadBooster())
    return zoneCollection[id].strike();
  return false;
}
bool IrrigationController::startZone(short id) {
  bool t = false;
  if (isEnable() && loadBooster()) {
    if (reversePolarity(true))
      t = zoneCollection[id].strike();
    reversePolarity(false);
  }
  return t;  
}
