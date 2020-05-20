#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#define TFT_DC 9
#define TFT_CS -1
#define TFT_MOSI 12
#define TFT_MISO -1
#define TFT_RST 10
#define TFT_CLK 13
#define pin_relay 8
#define pin_switch 7
#define pin_encoderA 6 // D3
#define pin_encoderB 5 // D4

#define TFT_FONTH_1  8*1
#define TFT_FONTH_2  8*2
#define TFT_FONTH_3  8*3
#define TFT_FONTW    6


// Ventilator Parameters
// current, change_by, min, max
float pRR[]   = {20.0, 0.5, 10.0, 30.0};  // breaths/min
float pTH[]   = {-5.0, 0.1, -10.0, 0} ;   // cmH2O
float pIE[]   = {2.0, 0.05, 1.0, 2.5};    // in:out ratio 
float pVol[]  = {500., 10., 200., 1000.}; // mL
float pVmax[] = {600., 10., 250., 1000};  // mL
float pPmax[] = {60., 2., 20., 100.};     // cmH2O
float pMin[]  = {0., 2., 0., 30.};        // cmH2O
float respPeriod = 0.;  // milliseconds
long rand_long = 0;
double randp_peep = 0.0;
double randp_pip = 0.0;
double randp_vol = 0.0;
long curTime = 0;
long measTime = 0;
long dispTime = 0;
long cycleTime = 0;
int cycleState = 0;  //0 closed,  1 open
int encoderVal = 0;
int encoderPushes = 0;
int current_p = 0;
const long encoderTDeadTime = 50; //millis
const long encoderPDeadTime = 500; //millis
long encoderPushTime = 0; //millis
long encoderTurnTime = 0; //millis
bool stateA, stateB;
const int TFT_w = 240;
const int TFT_h = 320;
char s[5];
long t0, tf, tm;

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
//Adafruit_ILI9341 tft = Adafruit_ILI9341(&SPI, TFT_DC, TFT_CS, TFT_RST);
void setup_display(){
  // Screen is 240x320
  // Clear screen
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextSize(2); 
  tft.setTextColor(ILI9341_BLUE); 
  tft.setCursor(16, 1); // 40 - 12*2
  tft.print("PEEP");
  tft.setTextColor(ILI9341_GREEN); 
  tft.setCursor(102, 1); // 120 - 12*1.5
  tft.print("PIP");
  tft.setTextColor(ILI9341_CYAN); 
  tft.setCursor(182, 1);  // 200 - 12*1.5
  tft.print("Vol");
  // Values start at y=30
}

void update_display() {
  // Headers for display live parms starts on L1 and are font size 2
  // values that go below these labels:
  tft.setTextSize(3); 
  tft.setTextColor(ILI9341_BLUE); 
  tft.fillRect(0,30,80,TFT_FONTH_3,ILI9341_BLACK);
  tft.setCursor(1, 30);   // 40 - 18*2 - 3
  tft.print(randp_peep,1);
  //
  tft.setTextColor(ILI9341_GREEN); 
  tft.fillRect(80,30,80,TFT_FONTH_3,ILI9341_BLACK);
  tft.setCursor(87, 30);   // 120 - 18*2 +3
  tft.print(randp_pip,1);
  tft.setTextColor(ILI9341_CYAN); 
  tft.fillRect(160,30,80,TFT_FONTH_3,ILI9341_BLACK);
  tft.setCursor(173, 30);  // 200-18*1.5
  tft.print(randp_vol,0);
}


double get_rand(double rmin, double rmax){
  rand_long = random();
  return rmin + (rmax - rmin)* ((double)rand_long / 4.2949e9);
}


void setup() {
  Serial.begin(9600);
  //SPI.begin();

  Serial.println("Starting up");
  pinMode(pin_relay, OUTPUT);
  pinMode(pin_switch, INPUT_PULLUP);
  pinMode(pin_encoderA, INPUT_PULLUP);
  pinMode(pin_encoderB, INPUT_PULLUP);
  SPI.begin();
  tft.begin();
  tft.setRotation(0);
  respPeriod = 60. / pRR[0] * 1e3;  // millis
  Serial.println("Setup Done, running...");
  setup_display();
}

void loop() {
  curTime = millis();
  if ( (curTime - measTime) > 9 ){
    // Take a sudo measurments
    randp_peep = get_rand(11.0, 15.0);
    randp_pip  = get_rand(25.0, 31.0);
  }
  if ( cycleState == 0 ){
    // The valve is closed
    if ( (curTime - cycleTime) > ( (respPeriod / (1.0 + pIE[0])) * pIE[0] ) ){
      cycleState = 1;
      digitalWrite(pin_relay, HIGH);
      cycleTime = curTime;
      Serial.println("Breath in...");
      randp_vol  = get_rand(400.0, 455.0);
      t0 = millis();
      update_display();
      tf = millis();
      tm = millis();
      Serial.println(tf - t0 - (tm-tf));
    }
  }
  else if ( cycleState == 1 ) { 
    if ( (curTime - cycleTime) > ( (respPeriod / (1.0 + pIE[0])) ) ){
      cycleState = 0;
      digitalWrite(pin_relay, LOW);
      cycleTime = curTime;
      Serial.println("Breath out...");
    }
  }
}