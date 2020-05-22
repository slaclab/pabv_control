#include "Arduino.h"
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "gui_parameter.h"

#define SPI_DEFAULT_FREQ 20000000
#define TFT_DC 9
#define TFT_CS -1
#define TFT_MOSI 11
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
#define TFT_WIDTH 240
#define TFT_HEIGHT 320  
#define TFT_THIRD = TFT_WIDTH/3
#define TFT_HALF = TFT_WIDTH/2

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

//Screen Constructors:
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO); // Software SPI
//Adafruit_ILI9341 tft = Adafruit_ILI9341(&SPI, TFT_DC, TFT_CS, TFT_RST);  // custom SPI definition
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);  //Default HW SPI define

/*
Class Structure
Print:
  Adafruit_GFX:
    Adafruit_SPITFT:
      Adafruit_ILI9341

Adafruit_ILI9341::
  ILI9341_TFTWIDTH 240
  ILI9341_TFTHEIGHT 320
  ILI9341_BLACK 0x0000
  ILI9341_NAVY 0x000F 
  ILI9341_DARKGREEN 
  ILI9341_DARKCYAN 
  ILI9341_MAROON 
  ILI9341_PURPLE 
  ILI9341_OLIVE 
  ILI9341_LIGHTGREY 
  ILI9341_DARKGREY 
  ILI9341_BLUE 
  ILI9341_GREEN 
  ILI9341_CYAN 
  ILI9341_RED 
  ILI9341_MAGENTA 
  ILI9341_YELLOW 
  ILI9341_WHITE 
  ILI9341_ORANGE 
  ILI9341_GREENYELLOW 
  ILI9341_PINK 
  begin(uint32_t freq = 0)
  setRotation(uint8_t r)

Adafruit_SPITFT::
  drawPixel(int16_t x, int16_t y, uint16_t color)
  fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
  drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
  drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)

Adafruit_GFX::
  drawPixel(int16_t x, int16_t y, uint16_t color)
  drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
  drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
  fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
  fillScreen(uint16_t color)
  drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
  drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
  drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
  drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color)
  fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
  fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, 
                      int16_t delta, uint16_t color)
  drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color)
  fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color)
  drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size)
  drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color,
                uint16_t bg, uint8_t size_x, uint8_t size_y)
  getTextBounds(const char *string, int16_t x, int16_t y, int16_t *x1,
                     int16_t *y1, uint16_t *w, uint16_t *h)
  setTextSize(uint8_t s)  // Desired text size. 1 is default 6x8, 2 is 12x16, 3 is 18x24, etc
  setTextSize(uint8_t sx, uint8_t sy)
  setFont(const GFXfont *f = NULL)     
  setCursor(int16_t x, int16_t y)
  setTextColor(uint16_t c)           
  setTextColor(uint16_t c, uint16_t bg)
  setTextWrap(boolean w)
*/


void setup_display(){
  /* 
  I setup the display with the fixed labels and such
  */
  if (invert_colors == false)  tft.fillScreen(ILI9341_WHITE);
  else tft.fillScreen(ILI9341_BLACK); // inverted
  // Doing labels for LIVE numbers (PEEP PIP Vol)
  fs_ = 2;
  tft.setTextSize(fs_);
  y_ = 1; // move the top of the labels one pixel down from top
  // Live number labels
  //
  tft.setTextColor(ILI9341_MAROON);
  if (invert_colors == true)  tft.setTextColor(ILI9341_PINK);
  x_ = (int) TFT_THIRD/2 + fs_*2*6;
  tft.setCursor(x_, y_); // 40 - 12*2
  tft.print("PEEP");
  //
  tft.setTextColor(ILI9341_DARKGREEN);
  if (invert_colors == true)  tft.setTextColor(ILI9341_GREEN);
  x_ = (int) TFT_THIRD + TFT_THIRD/2 + fs_*1.5*6;
  tft.setCursor(x_, y_); // 120 - 12*1.5
  tft.print("PIP");
  //
  tft.setTextColor(ILI9341_DARKCYAN);
  if (invert_colors == true) tft.setTextColor(ILI9341_CYAN);
  x_ = (int) 2*TFT_THIRD + TFT_THIRD/2 + fs_*1.5*6;
  tft.setCursor(x_, y_);  // 200 - 12*1.5
  tft.print("Vol");
  // Values start at y_ = 1 + 2*8 + space = 25
  // Values end at 
  // Next is parameters we can set
  y_ = 70;
  if 
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
