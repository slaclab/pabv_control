
#include <Wire.h>

const unsigned int RelayCount     = 2;
const unsigned int RelayPins[2]   = {2,3};
const unsigned int AnalogCount    = 4;
const unsigned int AnalogPins[4]  = {2,3,4,5};
const unsigned int AnalogMillis   = 10;
const unsigned int DefRelayPeriod = 3000;
const unsigned int DefRelayOn     = 1000;

const byte I2cAddr = 41;
const byte I2cCmd  = 0xAC; // 2 cycle average = 8ms

unsigned int relayPeriod;
unsigned int relayOn;
unsigned int x;
unsigned int analogValues[4];
unsigned int analogTime;
unsigned int currTime;
unsigned int relayTime;
unsigned int rxCount;
unsigned int scanPeriod;
unsigned int scanOn;
unsigned int cycleCount;
byte i2cRaw[4];
byte i2cLow;
byte i2cHigh;

char txBuffer[100];
char rxBuffer[50];
char mark[20];
char c;
int  ret;

void setup() {

   pinMode(RelayPins[0], OUTPUT);
   pinMode(RelayPins[1], OUTPUT);
   digitalWrite(RelayPins[0], LOW);
   digitalWrite(RelayPins[1], HIGH);

   analogTime = millis();
   relayTime  = millis();
   cycleCount = 0;
   rxCount    = 0;

   relayPeriod = DefRelayPeriod;
   relayOn     = DefRelayOn;

   Serial.begin(9600);
   Wire.begin();
}

void (* resetFunc)(void) = 0;

void loop() {

   for (x=0; x < AnalogCount; x++)
      analogValues[x] = analogRead(AnalogPins[x]);

   currTime = millis();

   // Turn off time
   if ((currTime - relayTime) > relayOn ) {
      digitalWrite(RelayPins[0], HIGH);
      digitalWrite(RelayPins[1], LOW);
   }

   // End of cycle, turn on
   if ((currTime - relayTime) > relayPeriod ) {
      digitalWrite(RelayPins[0], LOW);
      digitalWrite(RelayPins[1], HIGH);
      relayTime = currTime;
      cycleCount++;
   }

   if ((currTime - analogTime) > AnalogMillis ) {

      // Read last cycles values for dlc
      Wire.requestFrom(I2cAddr, byte(4));
      for (x=0; x < 4; x++) i2cRaw[x] = Wire.read();

      // Start new cycle for dlc
      Wire.beginTransmission(I2cAddr);
      Wire.write(I2cCmd);
      Wire.endTransmission();

      // Read npa sensor
      Wire.requestFrom(I2cAddr, byte(2));
      i2cHigh = Wire.read() & 0x3F;
      i2cLow = Wire.read();
      i2cValue = (i2cHigh << 8) | i2cLow;

      sprintf(txBuffer,"ANALOG %i %i %i %i %i 0x%.2x%.2x%.2x 0x%.2x%.2x\n", cycleCount,
                       analogValues[0], analogValues[1],
                       analogValues[2], analogValues[3],
                       i2cRaw[1], i2cRaw[2], i2cRaw[3],
                       i2High, i2cLow);

      Serial.write(txBuffer);
      analogTime = currTime;
   }

   // Get serial data
   while (Serial.available()) {
      if ( rxCount == 49 ) rxCount = 0;

      c = Serial.read();
      rxBuffer[rxCount++] = c;
      rxBuffer[rxCount] = '\0';
   }

   // Check for incoming message
   if ( rxCount > 6 && rxBuffer[rxCount-1] == '\n') {

      // Parse string
      ret = sscanf(rxBuffer,"%s %i %i", mark, &scanPeriod, &scanOn);

      // Check for reset
      if ( ret > 0 && strcmp(mark,"RESET") == 0 ) resetFunc();

      // Check for Count Reset
      if ( ret > 0 && strcmp(mark,"CNTRST") == 0 ) cycleCount = 0;

      // Check marker
      else if ( ret == 3 && strcmp(mark,"PERIOD") == 0 ) {
         relayPeriod = scanPeriod;
         relayOn     = scanOn;

         sprintf(txBuffer,"PERIOD %i %i\n",scanPeriod,scanOn);
         Serial.write(txBuffer);
      }
      rxCount = 0;
   }
}

