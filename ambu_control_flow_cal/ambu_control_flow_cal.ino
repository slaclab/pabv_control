
#include <Wire.h>
#include <SensorHaf50Slpm.h>
#include <SensorSp110Sm02.h>

#define ANALOG_MILLIS 9

SensorHaf50Slpm * ref  = new SensorHaf50Slpm();
SensorSp110Sm02 * flow = new SensorSp110Sm02();

unsigned int analogTime;
unsigned int currTime;
unsigned int cycleCount;

char txBuffer[20];

void setup() {

   Serial.begin(57600);
   Wire.begin();

   ref->setup();
   flow->setup();

   analogTime = millis();
   cycleCount = 0;
}

void loop() {

   currTime = millis();

   if ((currTime - analogTime) > ANALOG_MILLIS ) {

      ref->update(currTime);
      flow->update(currTime);

      // Generate serial output
      sprintf(txBuffer, "STATUS %i 3000 1000 0 3",(cycleCount / 100));
      Serial.write(txBuffer);
      ref->sendString();
      flow->sendString();
      Serial.write("\n");
      analogTime = currTime;
      cycleCount += 1;
   }
}

