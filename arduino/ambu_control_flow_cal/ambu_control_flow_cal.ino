
#include <Wire.h>
#include <SensorHaf50Slpm.h>
#include <SensorSp110Sm02.h>

#define SENSOR_PERIOD_MILLIS 9

SensorHaf50Slpm * ref  = new SensorHaf50Slpm();
SensorSp110Sm02 * flow = new SensorSp110Sm02();

uint32_t sensorTime;
uint32_t cycleCount;

void setup() {

   Serial.begin(57600);
   Serial.print("DEBUG Booted\n");

   Wire.begin();

   // Wait 5 seconds for pressure to settle
   Serial.print("DEBUG Wait 5 seconds\n");
   delay(5000);

   ref->setup();
   flow->setup();

   sensorTime = millis();
   Serial.print("DEBUG setup done\n");

   cycleCount = 0;
}

void loop() {

   uint32_t currTime;

   currTime = millis();

   if ((currTime - sensorTime) > SENSOR_PERIOD_MILLIS ) {

      ref->update(currTime);
      flow->update(currTime);

      // Generate serial output
      Serial.print("STATUS ");
      Serial.print(cycleCount / 100);
      ref->sendString();
      flow->sendString();
      Serial.print(" 0\n");
      sensorTime = currTime;
      cycleCount += 1;
   }
}

