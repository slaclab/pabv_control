#include "Arduino.h"
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "GUI.h"


#define SPI_DEFAULT_FREQ 20000000

#define pin_relay 8
#define pin_switch 7
#define pin_encoderA 6 // D3
#define pin_encoderB 5 // D4

#define pRR  0
#define pTH  1
#define pIH  2
#define pVol 3
#define pVmax 4
#define pPEEP 5
#define pPIP 6
const uint8_t num_parm = 7;


// Ventilator Parameters
// current, change_by, min, max
float parms[][] = { 
  {20.0, 0.5, 10.0, 30.0},  //pRR in breaths/min
  {-5.0, 0.1, -10.0, 0},    //pTH in cmH2O
  {1.0, 0.1, 0.5, 2.5},     //pIH in seconds
  {500., 10., 200., 1000.}, //pVol in mL
  {600., 10., 250., 1000},  //pVmax in mL 
  {60., 2., 20., 100.},     //pPEEP in cmH2O
  {0., 2., 0., 30.}         //pPIP in cmH2O
};
/*
float pRR[]   = {20.0, 0.5, 10.0, 30.0};  // breaths/min
float pTH[]   = {-5.0, 0.1, -10.0, 0} ;   // cmH2O
float pIH[]   = {1.0, 0.1, 0.5, 2.5};     // in:out ratio
float pVol[]  = {500., 10., 200., 1000.}; // mL
float pVmax[] = {600., 10., 250., 1000};  // mL
float pPmax[] = {60., 2., 20., 100.};     // cmH2O
float pMin[]  = {0., 2., 0., 30.};        // cmH2O
*/

bool invert_colors = 0;

//pRRcolor[] = {ILI9341_DARKCYAN, ILI9341_CYAN};
//pTHcolor
int x_ = 0;  //cursor position pixels
int y_ = 0;  //cursor position pixels
int fs_ = 0; //font size
uint16_t parm_color = 0x0000; //BLACK

float respPeriod = 0.;  // milliseconds
long rand_long = 0;  //TODO - remove
double randp_peep = 0.0;  //TODO - remove
double randp_pip = 0.0;   //TODO - remove
double randp_vol = 0.0;   //TODO - remove
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


GUI gui;



void setup_display(){
  /* 
  I setup the display with the fixed labels and such
  */
  // Doing labels for LIVE numbers (PEEP PIP Vol)
  fs_ = 2;
  y_ = 1; // move the top of the labels one pixel down from top
  // Live number labels
  //
  x_ = (int) TFT_THIRD/2 + fs_*2*6;
  gui.addItem("PEEP",{x,y,ILI9341_BLUE,fs,2},{0,0,0,0,0});   
  x_ = (int) TFT_THIRD + TFT_THIRD/2 + fs_*1.5*6;
  gui.addItem("PIP",{x,y,ILI9341_GREEEN,fs,2},{0,0,0,0,0});   
  x_ = (int) 2*TFT_THIRD + TFT_THIRD/2 + fs_*1.5*6;
  gui.addItem("VOL",{x,y,ILI9341_CYAN,fs,2},{0,0,0,0,0});   
}

void update_display() {

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
  tft.begin();
  tft.setRotation(0);
  gui_
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
