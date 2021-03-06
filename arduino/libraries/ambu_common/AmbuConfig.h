
#ifndef __AMBU_CONFIG_H__
#define __AMBU_CONFIG_H__

#define CONFIG_MILLIS 1000

#include <stdint.h>
#include <Arduino.h>
#include <HardwareSerial.h>
#include "Comm.h"
class CycleControl;

class AmbuParameters {
   public:
      double respRate;
      double inhTime;
      double pipMax;
      double pipOffset;
      double volMax;
      double volFactor;
      double volMaxAdj;
      double volInThold;
      double peepMin;

      uint8_t runState;
};

class AmbuConfig {
   public:
      using cpuId=uint32_t[4];
      // Run states
      static const uint8_t StateForceOff = 0;
      static const uint8_t StateForceOn  = 1;
      static const uint8_t StateRunOff   = 2;
      static const uint8_t StateRunOn    = 3;

      // Set parameter Constants
      static constexpr  uint8_t GetConfig     = 0;
      static constexpr  uint8_t SetRespRate   = 1;
      static constexpr  uint8_t SetInhTime    = 2;
      static constexpr  uint8_t SetPipMax     = 3;
      static constexpr  uint8_t SetPipOffset  = 4;
      static constexpr  uint8_t SetVolMax     = 5;
      static constexpr  uint8_t SetVolFactor  = 6;
      static constexpr  uint8_t SetVolInThold = 7;
      static constexpr  uint8_t SetPeepMin    = 8;
      static constexpr  uint8_t SetRunState   = 9;
      static constexpr  uint8_t MuteAlarm     = 10;

   protected:

      char rxBuffer_[100];
      uint16_t rxCount_;

      uint32_t confTime_;

      AmbuParameters conf_;

      void storeConfig();
      Comm &serial_;
      Comm &display_;
      bool update_(Message &m,CycleControl &cycle);
      cpuId cpuId_;

      uint32_t cfgSerialNum_;

   public:

      AmbuConfig (Comm &serial,Comm &display);
      void deviceID(cpuId &id) ;
      virtual void setup();

      void update(uint32_t ctime, CycleControl &cycle);

      // Set/Get Parameters
      double getRespRate();
      void setRespRate(double value);

      double getInhTime();
      void setInhTime(double value);

      double getPipMax();
      void setPipMax(double value);

      double getVolMax();
      void setVolMax(double value);

      double getVolInThold();
      void setVolInThold(double value);

      double getPeepMin();
      void setPeepMin(double value);

      uint8_t getRunState();
      void setRunState(uint8_t value);

      // Unused only by internal engine
      uint32_t getOffTimeMillis();
      uint32_t getOnTimeMillis();
      double   getAdjVolMax();
      void     updateAdjVolMax(double maxVol);
      double   getAdjPipMax();

};

#endif
