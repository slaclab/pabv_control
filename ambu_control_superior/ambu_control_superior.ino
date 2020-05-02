
#include <Wire.h>
#include <AmbuConfig.h>
#include <RelayControl.h>
#include <SensorDlcL20D4.h>
#include <SensorSp110Sm02.h>

#define RELAY_PIN 2
#define ANALOG_MILLIS 9

AmbuConfig      * conf  = new AmbuConfig();
SensorDlcL20D4  * press = new SensorDlcL20D4();
SensorSp110Sm02 * flow  = new SensorSp110Sm02();
RelayControl    * relay = new RelayControl(conf,flow,RELAY_PIN);

unsigned int analogTime;
unsigned int currTime;

void setup() {

   Serial.begin(57600);
   Wire.begin();

   conf->setup();
   press->setup();
   flow->setup();
   relay->setup();

   analogTime = millis();
}

void loop() {

   currTime = millis();

   if ((currTime - analogTime) > ANALOG_MILLIS ) {

      press->update(currTime);
      flow->update(currTime);

      // Generate serial output
      Serial.write("STATUS");
      relay->sendString();
      conf->sendString();
      press->sendString();
      flow->sendString();
      Serial.write("\n");
      analogTime = currTime;
   }

   relay->update(currTime);

   conf->update(currTime);

}

