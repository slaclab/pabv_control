
#include <Wire.h>
#include <AmbuConfig.h>
#include <RelayControl.h>
#include <SensorDlcL20D4.h>
#include <SensorNpa700B02WD.h>

#define RELAY_PIN 2
#define SENSOR_PERIOD_MILLIS 9

AmbuConfig        * conf  = new AmbuConfig();
SensorDlcL20D4    * press = new SensorDlcL20D4();
SensorNpa700B02WD * flow  = new SensorNpa700B02WD();
RelayControl      * relay = new RelayControl(conf,flow,RELAY_PIN);

unsigned int sensorTime;
unsigned int currTime;

void setup() {

   Serial.begin(57600);
   Wire.begin();

   conf->setup();
   press->setup();
   flow->setup();
   relay->setup();

   sensorTime = millis();
}

void loop() {

   currTime = millis();

   if ((currTime - sensorTime) > SENSOR_PERIOD_MILLIS ) {

      press->update(currTime);
      flow->update(currTime);

      // Generate serial output
      Serial.write("STATUS");
      relay->sendString();
      conf->sendString();
      press->sendString();
      flow->sendString();
      Serial.write("\n");
      sensorTime = currTime;
   }

   relay->update(currTime);

   conf->update(currTime);

}

