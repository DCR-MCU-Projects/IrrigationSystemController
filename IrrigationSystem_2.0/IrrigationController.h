#ifndef IRRIGATIONCONTROLLER
  #define IRRIGATIONCONTROLLER
  
  #include "IrrigationZone.h"
  
  class IrrigationController {

    private:
      IrrigationZone zoneCollection[MAX_ZONE];

      short _polarityReverserPin = -1;
      short _relayBoosterPin = -1;

      bool _controllerInUse = false;
      bool _controllerEnable = false;

      long latestTickTime = 0;

    public:
      IrrigationController(short polarityReverserPin, short relayBoosterPin);
      

      bool loadBooster();
      bool reversePolarity(bool);

      // Needed for OTA Update, ensure all valve are closed before upgrading, then initiate the controller with enable.
      void enableController();
      void disableController();

      short getBoostLevel();

      // See if water is flowing into the controller.
      bool isRunning();
      
      // See if the controller is enable
      bool isEnable();

      // Manage and take action on Zones
      bool addZone(short id, short pin);
      bool deleteZone(short id);
      
      void enableZone(short id);
      void disableZone(short id);
      
      bool startZone(short id);
      bool stopZone(short id);      
      
  };

#endif
