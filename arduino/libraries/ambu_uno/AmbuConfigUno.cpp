
#include "AmbuConfigUno.h"
#include <EEPROM.h>

#include <HardwareSerial.h>
#include <Arduino.h>


AmbuConfigUno::AmbuConfigUno (Stream *serial) : AmbuConfig (serial) { }

void AmbuConfigUno::setup() {
   uint32_t csum;
   uint32_t csumR;
   uint8_t  c;
   uint16_t x;

   AmbuParameters rconf;

   csumR = 0;

   for (x=0; x < sizeof(AmbuParameters); x++) {
      c = EEPROM.read(x+4);
      csumR += c;
      ((uint8_t *)&rconf)[x] = c;
   }

   for (x=0; x < 4; x++ ) {
      c = EEPROM.read(x);
      ((uint8_t *)&csum)[x] = c;
   }

   // verify checksum
   if ( csum == csumR ) conf_ = rconf;

   else {
      AmbuConfig::setup();
      storeConfig();
   }
   confTime_ = millis();
}

void AmbuConfigUno::storeConfig() {
   uint32_t csum;
   uint8_t  c;
   uint16_t x;

   csum = 0;

   for (x=0; x < sizeof(AmbuParameters); x++) {
      c = ((uint8_t *)&conf_)[x];
      EEPROM.write(x+4,c);
      csum += c;
   }

   for (x=0; x < 4; x++ ) {
      c = ((uint8_t *)&csum)[x];
      EEPROM.write(x,c);
   }
}

