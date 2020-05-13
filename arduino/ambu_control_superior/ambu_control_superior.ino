
#include <Wire.h>
#include <AmbuConfig.h>
#include <RelayControl.h>
#include <SensorDlcL20D4.h>
#include <SensorSp110Sm02Flow.h>
#include <SensorVolume.h>

//#define RELAY_PIN 2
#define RELAY_PIN 4
#define SENSOR_PERIOD_MILLIS 9

AmbuConfig          * conf  = new AmbuConfig();
SensorDlcL20D4      * press = new SensorDlcL20D4();
SensorSp110Sm02Flow * flow  = new SensorSp110Sm02Flow();
SensorVolume        * vol   = new SensorVolume(flow);
RelayControl        * relay = new RelayControl(conf,press,vol,RELAY_PIN);

unsigned int sensorTime;
unsigned int currTime;

void setup() {

   Serial.begin(57600);
   Serial.write("DEBUG Booted\n");

   Wire.begin();

   relay->setup();

   // Wait 5 seconds for pressure to settle
   Serial.write("DEBUG Wait 5 seconds\n");
   delay(5000);

   conf->setup();
   press->setup();
   flow->setup();
   vol->setup();

   sensorTime = millis();
   Serial.write("DEBUG setup done\n");
}

void loop() {

   currTime = millis();

   //if ((currTime - sensorTime) > SENSOR_PERIOD_MILLIS ) {

      press->update(currTime);
      flow->update(currTime);
      vol->update(currTime);

      // Generate serial output
      Serial.write("STATUS");
      relay->sendString();
      press->sendString();
      flow->sendString();
      vol->sendString();
      Serial.write("\n");
      sensorTime = currTime;
   //}
   relay->update(currTime);
   conf->update(currTime);
}

