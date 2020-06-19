
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

AmbuConfig::AmbuConfig (Comm &serial,Comm &display) : rxCount_(0),serial_(serial),display_(display),cfgSerialNum_(1) {
  deviceID(cpuId_);
  memset(rxBuffer_,0,20);
}

void AmbuConfig::setup () {
   conf_.respRate   = 20.0;
   conf_.inhTime    = 1.0;
   conf_.pipMax     = 40.0;
   conf_.pipOffset  = 0.0;
   conf_.volMax     = 200.0;
   conf_.volFactor  = 0.5;
   conf_.volMaxAdj  = conf_.volMax * conf_.volFactor;
   conf_.volInThold = -2.0;
   conf_.peepMin    = 0.0;
   conf_.runState   = StateRunOn;
   confTime_ = millis();
}
bool AmbuConfig::update_(Message &m,CycleControl &cycle) {
  bool sendConfig=false;
  uint8_t id=m.id();
  if(  (m.status()==Message::ERR_OK) && (m.nInt()>0)) {
    uint32_t param=m.getInt()[0];
    sendConfig = true;
    if(id==Message::PARAM_FLOAT && m.nFloat()==1) {
      float f=m.getFloat()[0];
      Serial.print("Param: ");
      Serial.println(param,HEX);
      Serial.print("Value: ");
      Serial.println(f);
      if(     param==SetRespRate)        conf_.respRate = f;
      else if(param==SetInhTime)    conf_.inhTime = f;
      else if(param==SetPipMax)     conf_.pipMax = f;
      else if(param==SetPipOffset)  conf_.pipOffset = f;
      else if(param==SetVolMax)     conf_.volMax = f;
      else if(param==SetVolFactor)  conf_.volFactor = f;
      else if(param==SetVolInThold) conf_.volInThold = f;
      else if(param==SetPeepMin)    conf_.peepMin = f;
      storeConfig();
    } else if (id==Message::PARAM_INTEGER  && m.nInt()==2) {
      uint32_t d=m.getInt()[1];
      //      Serial.print("Param: ");
      //Serial.println(param,HEX);
      //Serial.print("Value: ");
      //Serial.println(d);
      if(param==SetRunState)        conf_.runState = d;
      storeConfig();
    } else if (id==Message::PARAM_SET && m.nFloat()==0 && m.nInt()==1 ) {
       if(param==MuteAlarm) {
         cycle.muteAlarm();
         Serial.println("Clear Alarm");
       }
    }
  }
  return sendConfig;
}


void AmbuConfig::update(uint32_t ctime, CycleControl &cycle) {
   bool sendConfig;
   sendConfig = false;
   Message m;
   serial_.read(m);
   sendConfig=sendConfig || update_(m,cycle);
   display_.read(m);
   sendConfig=sendConfig || update_(m,cycle);
   if(sendConfig)
     cfgSerialNum_++;
   if ((ctime - confTime_) > CONFIG_MILLIS) sendConfig = true;

   if (sendConfig) {
     Message m;
     float config[8];
     uint32_t intConf[2];
     config[0]=conf_.respRate;
     config[1]=conf_.inhTime;
     config[2]=conf_.pipMax;
     config[3]=conf_.pipOffset;
     config[4]=conf_.volMax;
     config[5]=conf_.volFactor;
     config[6]=conf_.volInThold;
     config[7]=conf_.peepMin;
     intConf[0]=conf_.runState;
     intConf[1]=cfgSerialNum_;
     m.writeData(Message::CONFIG,ctime,8,config,2,intConf);
     serial_.send(m);
     m.writeString(Message::VERSION,ctime,git_version);
     serial_.send(m);
     m.writeData(Message::CPU_ID,ctime,0,0,4,cpuId_);
     serial_.send(m);
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

void AmbuConfig::setVolMax(double value) {
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

double AmbuConfig::getPeepMin() {
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

double AmbuConfig::getAdjVolMax() {
   return conf_.volMaxAdj;
}

void AmbuConfig::updateAdjVolMax(double maxVol) {
   conf_.volMaxAdj -= conf_.volFactor * (maxVol - conf_.volMax);
}

void AmbuConfig::initAdjVolMax() {
   conf_.volMaxAdj = conf_.volMax * conf_.volFactor;
}

double   AmbuConfig::getAdjPipMax() {
   return (conf_.pipMax + conf_.pipOffset);
}

void AmbuConfig::deviceID(cpuId &id) {
#ifdef ARDUINO_ARCH_MBED;
  // https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.nrf52832.ps.v1.1%2Fficr.html
  uint32_t *deviceID=(uint32_t*) 0x10000060;
  id[0]=deviceID[0];
  id[1]=deviceID[1];
  id[2]=0;
  id[3]=0;
#elif ARDUINO_ARCH_SAMD
#define SERIAL_NUMBER_WORD_0	*(volatile uint32_t*)(0x0080A00C)
#define SERIAL_NUMBER_WORD_1	*(volatile uint32_t*)(0x0080A040)
#define SERIAL_NUMBER_WORD_2	*(volatile uint32_t*)(0x0080A044)
#define SERIAL_NUMBER_WORD_3	*(volatile uint32_t*)(0x0080A048)
  // https://cdn.sparkfun.com/assets/6/3/d/d/2/Atmel-42181-SAM-D21_Datasheet.pdf
  id[0] = SERIAL_NUMBER_WORD_0;
  id[1] = SERIAL_NUMBER_WORD_1;
  id[2] = SERIAL_NUMBER_WORD_2;
  id[3] = SERIAL_NUMBER_WORD_3;
#else
  #error Unsupported platform
#endif
}

void AmbuConfig::storeConfig() {
  // to be implemented
}
