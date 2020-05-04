
#include <Wire.h>
#include <SensorHaf50Slpm.h>
#include <SensorSp110Sm02.h>

#define SENSOR_PERIOD_MILLIS 9

SensorHaf50Slpm * ref  = new SensorHaf50Slpm();
SensorSp110Sm02 * flow = new SensorSp110Sm02();

unsigned int sensorTime;
unsigned int currTime;
unsigned int cycleCount;

char txBuffer[100];

void setup() {

   Serial.begin(57600);
   Wire.begin();

   ref->setup();
   flow->setup();

   sensorTime = millis();
   cycleCount = 0;
}

void loop() {

   currTime = millis();

   if ((currTime - sensorTime) > SENSOR_PERIOD_MILLIS ) {

      ref->update(currTime);
      flow->update(currTime);

      // Generate serial output
      sprintf(txBuffer, "STATUS %i 3000 1000 0 3",(cycleCount / 100));
      Serial.write(txBuffer);
      ref->sendString();
      flow->sendString();
      Serial.write("\n");
      sensorTime = currTime;
      cycleCount += 1;
   }
}

