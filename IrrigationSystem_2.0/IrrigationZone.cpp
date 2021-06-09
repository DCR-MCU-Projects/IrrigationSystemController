
#include "IrrigationZone.h"

IrrigationZone::IrrigationZone() {
  IrrigationZone(-1, -1);
}

IrrigationZone::IrrigationZone(short id, short pin) {
  _id = id;
  _pin = pin;
  _enable = false;

  Serial.print("Creating ");
  Serial.print(_id);
  Serial.print(" ");
  Serial.println(_pin);
  

  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
  
}

bool IrrigationZone::strike() {
  if (isEnable()) {
    Serial.print("Striking ZONE: ");
    Serial.print(_id);
    Serial.print(" on PIN: ");
    Serial.println(_pin);
    
    digitalWrite(_pin, HIGH);
    delay(2000);
    digitalWrite(_pin, LOW);
    return true;
  }
  return false;
}

void IrrigationZone::enable() {
  _enable = true;
}

void IrrigationZone::disable() {
  _enable = false;
}

bool IrrigationZone::isEnable() {
  return _enable;
}
