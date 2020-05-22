
#include "AmbuConfig.h"

#include <HardwareSerial.h>
#include <Arduino.h>
#include "CycleControl.h"

#ifndef GIT_VERSION
const char *git_version= "unknown";
#else
#define Q(x) #x
#define QUOTE(x) Q(x)
const char *git_version=QUOTE(GIT_VERSION);
#endif

AmbuConfig::AmbuConfig () {
   memset(rxBuffer_,0,20);
   rxCount_ = 0;
}

void AmbuConfig::setup () {
   conf_.respRate   = 20.0;
   conf_.inhTime    = 1.0;
   conf_.pipMax     = 100.0;
   conf_.pipOffset  = 0.0;
   conf_.volMax     = 200.0;
   conf_.volOffset  = 0.0;
   conf_.volInThold = -10.0;
   conf_.peepMin    = 0.0;
   conf_.runState   = StateRunOn;
   confTime_ = millis();
}

void AmbuConfig::update(uint32_t ctime, CycleControl *cycle) {
   char mark[50];
   char scanParam[50];
   uint32_t param;
   bool sendConfig;

   int16_t ret;
   char c;

   sendConfig = false;

   // Get serial data
   while (Serial.available()) {
      if ( rxCount_ >= 190) rxCount_ = 0;

      c = Serial.read();
      Serial.println(c);
      rxBuffer_[rxCount_++] = c;
      rxBuffer_[rxCount_] = '\0';
   }

   // Check for incoming message
   if ( rxCount_ > 7 && rxBuffer_[rxCount_-1] == '\n') {

      // Parse string
      ret = sscanf(rxBuffer_,"%s %i %s", mark, &param, scanParam);
      Serial.print(ret);
      Serial.println(param);

      // Check marker
      if ( ret == 3 && strcmp(mark,"CONFIG") == 0 ) {
         sendConfig = true;

         switch (param) {

            case SetRespRate:
               conf_.respRate = atof(scanParam);
               storeConfig();
               break;

            case SetInhTime:
               conf_.inhTime = atof(scanParam);
               storeConfig();
               break;

            case SetPipMax:
               conf_.pipMax = atof(scanParam);
               storeConfig();
               break;

            case SetPipOffset:
               conf_.pipMax = atof(scanParam);
               storeConfig();
               break;

            case SetVolMax:
               conf_.volMax = atof(scanParam);
               storeConfig();
               break;

            case SetVolOffset:
               conf_.volOffset = atof(scanParam);
               storeConfig();
               break;

            case SetVolInThold:
               conf_.volInThold = atof(scanParam);
               storeConfig();
               break;

            case SetPeepMin:
               conf_.peepMin = atof(scanParam);
               storeConfig();
               break;

            case SetRunState:
               conf_.runState = atoi(scanParam);
               storeConfig();
               break;

            case ClearAlarm:
               cycle->clearAlarm();
               break;

            default:
               // echo config
               break;
         }
      }
      rxCount_ = 0;
   }

   if ((ctime - confTime_) > CONFIG_MILLIS) sendConfig = true;

   if (sendConfig) {
       Serial.print("VERSION ");
       Serial.print(git_version);
       Serial.print("\n");
       Serial.print("CONFIG ");
       Serial.print(conf_.respRate,4);
       Serial.print(" ");
       Serial.print(conf_.inhTime,4);
       Serial.print(" ");
       Serial.print(conf_.pipMax,4);
       Serial.print(" ");
       Serial.print(conf_.pipOffset,4);
       Serial.print(" ");
       Serial.print(conf_.volMax,4);
       Serial.print(" ");
       Serial.print(conf_.volOffset,4);
       Serial.print(" ");
       Serial.print(conf_.volInThold,4);
       Serial.print(" ");
       Serial.print(conf_.peepMin,4);
       Serial.print(" ");
       Serial.print(conf_.runState);
       Serial.print(" ");
       Serial.print("\n");
       confTime_ = ctime;
   }
}

double AmbuConfig::getRespRate() {
   return conf_.respRate;
}

void AmbuConfig::setRespRate(double value) {
   conf_.respRate = value;
   storeConfig();
}

double AmbuConfig::getInhTime() {
   return conf_.inhTime;
}

void AmbuConfig::setInhTime(double value) {
   conf_.inhTime = value;
   storeConfig();
}

double AmbuConfig::getPipMax() {
   return conf_.pipMax;
}

void AmbuConfig::setPipMax(double value) {
   conf_.pipMax = value;
   storeConfig();
}

double AmbuConfig::getVolMax() {
   return conf_.volMax;
}

void AmbuConfig::setGetVolMax(double value) {
   conf_.volMax = value;
   storeConfig();
}

double AmbuConfig::getVolInThold() {
   return conf_.volInThold;
}

void AmbuConfig::setVolInThold(double value) {
   conf_.volInThold = value;
   storeConfig();
}

double AmbuConfig::setPeepMin() {
   return conf_.peepMin;
}

void AmbuConfig::setPeepMin(double value) {
   conf_.peepMin = value;
   storeConfig();
}

uint8_t AmbuConfig::getRunState() {
   return conf_.runState;
}

void AmbuConfig::setRunState(uint8_t value) {
   conf_.runState = value;
   storeConfig();
}

uint32_t AmbuConfig::getOffTimeMillis() {
   double period;
   uint32_t ret;

   period = (1.0 / conf_.respRate) * 60.0;

   ret = uint32_t((period - conf_.inhTime) * 1000.0);

   return ret;
}

uint32_t AmbuConfig::getOnTimeMillis() {
   uint32_t ret;

   ret = uint32_t(conf_.inhTime * 1000.0);

   return ret;
}

double   AmbuConfig::getAdjVolMax() {
   return (conf_.volMax + conf_.volOffset);
}

double   AmbuConfig::getAdjPipMax() {
   return (conf_.pipMax + conf_.pipOffset);
}

