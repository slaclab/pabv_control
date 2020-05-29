
#ifndef __AMBU_CONFIG_H__
#define __AMBU_CONFIG_H__

#define CONFIG_MILLIS 1000

#include <stdint.h>
#include <Arduino.h>
#include <HardwareSerial.h>

class CycleControl;

class AmbuParameters {
   public:
      double respRate;
      double inhTime;
      double pipMax;
      double pipOffset;
      double volMax;
      double volOffset;
      double volInThold;
      double peepMin;

      uint8_t runState;
};

class AmbuConfig {
   public:

      // Run states
      static const uint8_t StateForceOff = 0;
      static const uint8_t StateForceOn  = 1;
      static const uint8_t StateRunOff   = 2;
      static const uint8_t StateRunOn    = 3;

      // Set parameter Constants
      static const uint8_t GetConfig     = 0;
      static const uint8_t SetRespRate   = 1;
      static const uint8_t SetInhTime    = 2;
      static const uint8_t SetPipMax     = 3;
      static const uint8_t SetPipOffset  = 4;
      static const uint8_t SetVolMax     = 5;
      static const uint8_t SetVolOffset  = 6;
      static const uint8_t SetVolInThold = 7;
      static const uint8_t SetPeepMin    = 8;
      static const uint8_t SetRunState   = 9;
      static const uint8_t ClearAlarm    = 10;

   protected:

      char rxBuffer_[100];
      uint16_t rxCount_;

      uint32_t confTime_;

      AmbuParameters conf_;

      virtual void storeConfig() = 0;

      Stream *serial_;

   public:

      AmbuConfig (Stream *serial);

      virtual void setup();

      void update(uint32_t ctime, CycleControl *cycle);

      // Set/Get Parameters
      double getRespRate();
      void setRespRate(double value);

      double getInhTime();
      void setInhTime(double value);

      double getPipMax();
      void setPipMax(double value);

      double getVolMax();
      void setGetVolMax(double value);

      double getVolInThold();
      void setVolInThold(double value);

      double setPeepMin();
      void setPeepMin(double value);

      uint8_t getRunState();
      void setRunState(uint8_t value);

      // Unused only by internal engine
      uint32_t getOffTimeMillis();
      uint32_t getOnTimeMillis();
      double   getAdjVolMax();
      double   getAdjPipMax();

};

#endif
