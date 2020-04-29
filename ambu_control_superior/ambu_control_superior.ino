#include <Wire.h>

const unsigned int RelayPin       = 2;
const unsigned int AnalogCount    = 4;
const unsigned int AnalogPins[4]  = {2,3,4,5};
const unsigned int AnalogMillis   = 9;
const unsigned int DefRelayPeriod = 3000;
const unsigned int DefRelayOn     = 1000;
const unsigned int DefStartThold  = 0x8a14;
const unsigned int DefRunState    = 2;

const byte I2cAddrDlc = 41;
//const byte I2cAddrNpa = 40;
const byte I2cAddrSP110 = 0x28;
const byte I2cCmdDlc  = 0xAC; // 2 cycle average = 8ms

unsigned int startThold;
unsigned int relayPeriod;
unsigned int relayOn;
unsigned int x;
unsigned int analogTime;
unsigned int currTime;
unsigned int relayTime;
unsigned int rxCount;
unsigned int scanPeriod;
unsigned int scanOn;
unsigned int scanThold;
unsigned int scanRun;
unsigned int cycleCount;
unsigned int autoStart;
unsigned int compValue;
unsigned int inhalation;
unsigned int runState;
byte i2cRaw[4];
byte i2cLow;
byte i2cHigh;

char txBuffer[100];
char rxBuffer[50];
char mark[20];
char c;
int  ret;
int sp110Press;

void setup() {

   pinMode(RelayPin, OUTPUT);
   digitalWrite(RelayPin, LOW);

   analogTime = millis();
   relayTime  = millis();
   cycleCount = 0;
   rxCount    = 0;

   relayPeriod = DefRelayPeriod;
   relayOn     = DefRelayOn;
   startThold  = DefStartThold;
   runState    = DefRunState;
   inhalation  = 0;

   Serial.begin(57600);
   Wire.begin();

   // setup for the Superior SP110 sensor
   Wire.beginTransmission(I2cAddrSP110);
   // Sets 2cm with single point 0 offset correction
   // 250 Hz, so it should always be faster than the 9ms AnalogMillis
   Wire.write(0b10011101); 
   Wire.endTransmission();
   delay(200);
   Wire.beginTransmission(I2cAddrSP110);
   Wire.write(0b10111101);
   Wire.endTransmission(); // Auto-Zero is done
   delay(200);
}

void (* resetFunc)(void) = 0;

void loop() {
   autoStart = 0;

   currTime = millis();

   if ((currTime - analogTime) > AnalogMillis ) {

      // Read last cycles values for dlc
      Wire.requestFrom(I2cAddrDlc, byte(4));
      for (x=0; x < 4; x++) i2cRaw[x] = Wire.read();

      // Start new cycle for dlc
      Wire.beginTransmission(I2cAddrDlc);
      Wire.write(I2cCmdDlc);
      Wire.endTransmission();

      compValue = (i2cRaw[1] << 8) | i2cRaw[2];
      if ( compValue > startThold ) autoStart = 1;

      // Read npa sensor - DISABLED FOR MIB 4/29
      //Wire.requestFrom(I2cAddrNpa, byte(2));
      //i2cHigh = Wire.read() & 0x3F;
      //i2cLow = Wire.read();

      // Read Superior SP110 sensor, this returns a signed INT
      Wire.requestFrom(I2cAddrSP110, byte(2));
      i2cHigh = Wire.read();
      i2cLow = Wire.read();
      // puts the range in uint territory for compatability w/ GUI
      sp110Press = ((i2cHigh << 8) | i2cLow) + 32768; 

      sprintf(txBuffer,"STATUS %i %i %i 0x%x %i 0x%.2x%.2x%.2x 0x%.4x\n", cycleCount,
                       relayPeriod, relayOn, startThold, runState,
                       i2cRaw[1], i2cRaw[2], i2cRaw[3],
                       sp110Press);

      Serial.write(txBuffer);
      analogTime = currTime;
   }

   // Turn off time
   if ((currTime - relayTime) > relayOn ) inhalation  = 0;

   // End of cycle, turn on
   if ((inhalation == 0) && ((autoStart == 1) || ((currTime - relayTime) > relayPeriod ))) {
      relayTime = currTime;
      inhalation  = 1;
      cycleCount++;
   }

   // override relay
   if ( runState == 0 ) digitalWrite(RelayPin, LOW);
   else if ( runState == 1 ) digitalWrite(RelayPin, HIGH);
   else if ( runState == 2 ) {
      if ( inhalation == 1 ) digitalWrite(RelayPin, HIGH);
      else digitalWrite(RelayPin, LOW);
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
      ret = sscanf(rxBuffer,"%s %u %u %u %u", mark, &scanPeriod, &scanOn, &scanThold, &scanRun);

      // Check for reset
      if ( ret > 0 && strcmp(mark,"RESET") == 0 ) resetFunc();

      // Check for Count Reset
      if ( ret > 0 && strcmp(mark,"CNTRST") == 0 ) cycleCount = 0;

      // Check marker
      else if ( ret == 5 && strcmp(mark,"CONFIG") == 0 ) {
         relayPeriod = scanPeriod;
         relayOn     = scanOn;
         startThold  = scanThold;
         runState    = scanRun;
      }
      rxCount = 0;
   }
}

