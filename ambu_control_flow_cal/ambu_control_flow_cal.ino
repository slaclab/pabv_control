
#include <Wire.h>

const unsigned int RelayPin       = 2;
const unsigned int AnalogCount    = 4;
const unsigned int AnalogPins[4]  = {2,3,4,5};
const unsigned int AnalogMillis   = 9;
const unsigned int DefRelayPeriod = 3000;
const unsigned int DefRelayOn     = 1000;
const unsigned int DefStartThold  = 0x8a14;
const unsigned int DefRunState    = 2;

const byte I2cAddrHw  = 73;
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


      // Read Superior SP110 sensor, this returns a signed INT
      Wire.requestFrom(I2cAddrSP110, byte(2));
      i2cHigh = Wire.read();
      i2cLow = Wire.read();
      // puts the range in uint territory for compatability w/ GUI
      sp110Press = ((i2cHigh << 8) | i2cLow) + 32768;

      // Read honywell
      Wire.requestFrom(I2cAddrHw, byte(2));
      for (x=0; x < 2; x++) i2cRaw[x] = Wire.read();

      sprintf(txBuffer,"STATUS %i %i %i 0x%x %i 0x%.2x%.2x 0x%.4x\n", (cycleCount/100),
                       relayPeriod, relayOn, startThold, runState,
                       i2cRaw[0], i2cRaw[1],
                       sp110Press);

      Serial.write(txBuffer);
      analogTime = currTime;
      cycleCount += 1;
   }
}

