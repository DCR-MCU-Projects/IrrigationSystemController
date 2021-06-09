#ifndef IRRIGATIONZONE
  #define IRRIGATIONZONE
  #define MAX_ZONE 8
  
  #include <Arduino.h>
    
  class IrrigationZone {
  
    private:
      short _id = -1;
      short _pin = -1;
      bool _enable = false;
  
    public:
  
      IrrigationZone();
      IrrigationZone(short id, short pin);

      short id() { return _id; }
      short pin() { return _pin; }
    
      bool strike();
      
      void enable();
      void disable();
      
      bool isEnable();
    
  };

#endif
