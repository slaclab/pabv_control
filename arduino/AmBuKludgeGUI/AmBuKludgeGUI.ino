#include "Arduino.h"
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
//#include "GUI.h"
#include "Comm.h"
#include "AmbuConfig.h"

#define SerialPort Serial1 //RX-TX pins on board
Comm masterComm(SerialPort);

// Arduino Pins Connections -- Encoder
#define pin_switch 2
#define pin_encoderA 3
#define pin_encoderB 4

// Arduino Pins to ILI9341-LCD setup
static constexpr uint8_t TFT_DC= 10;     //D10
static constexpr uint8_t TFT_CS= -1;     //GND
static constexpr uint8_t TFT_MOSI= 11;   //MOSI
static constexpr uint8_t TFT_MISO= -1;   //Not used
static constexpr uint8_t TFT_RST= 9;     //D9
static constexpr uint8_t TFT_CLK= 13; 	 //SCK
// SETUP THE SCREEN
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

void setup(){
  asm(".global _printf_float");
  asm(".global _scanf_float");
  SerialPort.begin(9600);
  Serial.begin(9600);
  pinMode (pin_encoderA, INPUT_PULLUP);
  pinMode (pin_encoderB, INPUT_PULLUP);
  pinMode (pin_switch, INPUT_PULLUP);

  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);
  setup_display();
  delay(1000);
  Serial.println("Setup Done");
}


// Display Properties
static constexpr uint16_t TFT_WIDTH= 240;
static constexpr uint16_t TFT_HEIGHT= 320; 
static constexpr uint8_t TFT_FONTH_1 = 8*1;
static constexpr uint8_t TFT_FONTH_2=  8*2;
static constexpr uint8_t TFT_FONTH_3 = 8*3;
static constexpr uint8_t TFT_FONTW = 6;


// ROW 1 (PEEP PIP VOLUME I:E)
static inline constexpr uint8_t r1_label_y = 5;
static inline constexpr uint8_t r1_value_y = 34;
//static inline constexpr uint8_t r1_fs = 3; //font-
static inline constexpr uint8_t tPEEP_x = 5;
static inline constexpr uint8_t tPIP_x = 85; //(5+4*3*6+3*6) = 95
static inline constexpr uint8_t tVOL_x = 155; //(95+3*3*6+3*6) = 167
static inline constexpr uint8_t tIE_x = 220; //(167+3*3*6+3*6) = 239
// ROW 2 (RR InhT VMax  Mode)
static inline constexpr uint8_t r2_label_y = 66;
static inline constexpr uint8_t r2_value_y = 84;
//static inline constexpr uint8_t r2_fs = 2; //font-size
static inline constexpr uint8_t tRR_x = 5;
static inline constexpr uint8_t tIT_x = 85; //(5+4*3*6+3*6) = 95
static inline constexpr uint8_t tVMax_x = 165; //(95+3*3*6+3*6) = 167
static inline constexpr uint8_t tMode_x = 245; //(167+3*3*6+3*6) = 239
// ROW 3 (TH PMax PMin)
static inline constexpr uint8_t r3_label_y = 110;
static inline constexpr uint8_t r3_value_y = 128;
//static inline constexpr uint8_t r3_fs = 2; //font-size
static inline constexpr uint8_t tTH_x = 5;
static inline constexpr uint8_t tPMin_x = 85; //(5+4*3*6+3*6) = 95
static inline constexpr uint8_t tPMax_x = 165; //(95+3*3*6+3*6) = 167
static inline constexpr uint8_t tRun_x = 245; //(95+3*3*6+3*6) = 167

// Display Parameters
float vPEEP = 0.0; //
float vPIP = 0.0;  //
float vVOL = 0.0;  //
float vIE = 0.0;   //
// Settings:
float vRR = 0.0;
float vIT = 0.0;
float vTH = 0.0;
float vVMax = 0.0;
float vPMax = 0.0;
float vPMin = 0.0;
bool vMode = false;    // 0=Volume, 1=Pressure
bool vRun = false;     // 0=OFF, 1=On
// setting limits - structure is {delta, min, max}
float pRR[]   = {1.0, 10.0, 30.0};  // breaths/min
float pIT[]   = {0.1, 0.5, 2.5};    // inspiration time
float pTH[]   = {0.1, -10.0, 0} ;   // threshold
float pVMax[] = {10., 250., 850.};  // mL
float pPMax[] = {2., 20., 100.};    // cmH2O
float pPMin[] = {2., 0., 30.};     // cmH2O
// setting limits - structure is {delta, min, max}
void change_RR(int8_t dv){
  vRR = round(vRR) + dv*pRR[0];
  if (vRR < pRR[1]) vRR = pRR[1];
  if (vRR > pRR[2]) vRR = pRR[2];
}
void change_IT(int8_t dv){
  vIT = round(vIT/pIT[0])* pIT[0] + dv*pIT[0];
  if (vIT < pIT[1]) vIT = pIT[1];
  if (vIT > pIT[2]) vIT = pIT[2];
}
void change_TH(int8_t dv){
  vTH = round(vTH/pTH[0])* pTH[0] + dv*pTH[0];
  if (vTH < pTH[1]) vTH = pTH[1];
  if (vTH > pTH[2]) vTH = pTH[2];
}
void change_VMax(int8_t dv){
  vVMax = round(vVMax/pVMax[0])* pVMax[0] + dv*pVMax[0];
  if (vVMax < pVMax[1]) vVMax = pVMax[1];
  if (vVMax > pVMax[2]) vVMax = pVMax[2];
}
void change_PMax(int8_t dv){
  vPMax = round(vPMax/pPMax[0])* pPMax[0] + dv*pPMax[0];
  if (vPMax < pPMax[1]) vPMax = pPMax[1];
  if (vPMax > pPMax[2]) vPMax = pPMax[2];
}
void change_PMin(int8_t dv){
  vPMin = round(vPMin/pPMin[0])* pPMin[0] + dv*pPMin[0];
  if (vPMin < pPMin[1]) vPMin = pPMin[1];
  if (vPMin > pPMin[2]) vPMin = pPMin[2];
}
void change_Mode(){
  vMode = !vMode;
}
void change_Run(){
  vRun = !vRun;
}

void setup_display() {
  // Put the Parameter labels up on the screen
  tft.setTextSize(3);
  // PEEP
  tft.setTextColor(ILI9341_PINK);
  tft.setCursor(tPEEP_x, r1_label_y);
  tft.print("PEEP");
  // PIP
  tft.setTextColor(ILI9341_GREEN);
  tft.setCursor(tPIP_x, r1_label_y);
  tft.print("PIP");
  // VOL
  tft.setTextColor(ILI9341_CYAN);
  tft.setCursor(tVOL_x, r1_label_y);
  tft.print("VOL");
  // I:E
  tft.setTextColor(ILI9341_YELLOW);
  tft.setCursor(tIE_x, r1_label_y);
  tft.print("I:E");

  // -- ROW 2
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(tRR_x, r2_label_y);
  tft.print("R.R.");
  // 
  tft.setCursor(tIT_x, r2_label_y);
  tft.print("InspT");
  // 
  tft.setCursor(tVMax_x, r2_label_y);
  tft.print("VMax");
  // 
  tft.setCursor(tMode_x, r2_label_y);
  tft.print("Mode");

  // -- ROW 3
  tft.setCursor(tTH_x, r3_label_y);
  tft.print("InspP");
  // 
  tft.setCursor(tPMin_x, r3_label_y);
  tft.print("PMin");
  // 
  tft.setCursor(tPMax_x, r3_label_y);
  tft.print("PMax");
  //
  tft.setCursor(tRun_x, r3_label_y);
  tft.print("Run");
}


// Parameters used in the display to control the interface
uint32_t measTime = 0;
uint32_t curTime;
uint32_t prevUpdateTime;
int8_t encDT;   // -1 for CCW, +1 for CW turn
bool encPushed;
int8_t guiParamSelected = 0;  //0 for none and 1... for n
const uint32_t guiTimeout = 10000; //millis till gui times out
const uint32_t guiRefresh = 1000;  //millis to update cycle parameters
uint32_t guiPrevActionTime;
const uint32_t encTDeadTime = 100; //millis
const uint32_t encPDeadTime = 400; //millis
long encLastPTime = 0; //millis
long encLastTTime = 0; //millis
bool stateA, stateB;
double rand_long;


String num2intstr(float parm){
  // We assume the float decimals are not interisting to print in this case
  int val = int(parm);
  String out = String(val);
  return out;
}

String num2iefmt(float parm){
  if (parm<1.0){
    return String("1:")+String(1.0/parm, 1);
  }
  else{
    return String(parm, 1)+String(":1");
  }
}


void update_cycle_values(){
  // Every guiRefresh time refresh these values
  tft.writeFillRect(0, r1_value_y, TFT_HEIGHT, 24,ILI9341_BLACK);
  tft.setTextSize(3);  
  //
  tft.setTextColor(ILI9341_PINK);
  tft.setCursor(tPEEP_x,r1_value_y);
  tft.print(num2intstr(vPEEP));
  //
  tft.setTextColor(ILI9341_GREEN);
  tft.setCursor(tPIP_x,r1_value_y);
  tft.print(num2intstr(vPIP));
  //
  tft.setTextColor(ILI9341_CYAN);
  tft.setCursor(tVOL_x,r1_value_y);
  if (vVOL > 999.9){
    tft.setTextSize(2);
    tft.print(num2intstr(vVOL));
    tft.setTextSize(3);
  }
  else {
    tft.print(num2intstr(vVOL));
  }
  //
  tft.setTextColor(ILI9341_YELLOW);
  tft.setCursor(tIE_x,r1_value_y);
  tft.print(num2iefmt(vIE));
}
void update_parms(){
  /*
  Clears the area behind all values in row-2/3
  Updates all parameters in white (not selected)
  */
  tft.setTextSize(2);
  tft.writeFillRect(0, r2_value_y, TFT_HEIGHT, 16, ILI9341_BLACK);
  tft.writeFillRect(0, r3_value_y, TFT_HEIGHT, 16, ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  //
  tft.setCursor(tRR_x, r2_value_y);
  tft.print(String(vRR,1));
  //
  tft.setCursor(tIT_x, r2_value_y);
  tft.print(String(vIT, 1));
  //
  tft.setCursor(tVMax_x, r2_value_y);
  tft.print(String(vVMax, 1));
  //
  tft.setCursor(tMode_x, r2_value_y);
  if (vMode) tft.print("Vol");
  else tft.print("Pres");
  //
  tft.setCursor(tTH_x, r3_value_y);
  tft.print(String(vTH, 1));
  //
  tft.setCursor(tPMin_x, r3_value_y);
  tft.print(String(vPMin, 1));  
  //
  tft.setCursor(tPMax_x, r3_value_y);
  tft.print(String(vPMax, 1));
  //
  tft.setCursor(tRun_x, r3_value_y);
  if (vRun) tft.print("ON");
  else tft.print("OFF");
}

void highlight_changeable_parameter(uint8_t nparm){
  /*
  This sets the parameter to highlighted but not selected
  It needs to clear the previous selection probably and highlight
  the new one so that's why we run over all labels to change colors.
  It is perhaps lazy because we could probably figure out which was 
  the previous selected and only update the recent ones... 
  (lets see how performance is first)
  */
  tft.setTextSize(2);
  if (nparm == 1) tft.setTextColor(ILI9341_GREENYELLOW);
  else tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(tRR_x, r2_label_y);
  tft.print("R.R.");
  if (nparm == 2) tft.setTextColor(ILI9341_GREENYELLOW);
  else tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(tIT_x, r2_label_y);
  tft.print("InspT");
  if (nparm == 3) tft.setTextColor(ILI9341_GREENYELLOW);
  else tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(tVMax_x, r2_label_y);
  tft.print("VMax");
  if (nparm == 4) tft.setTextColor(ILI9341_GREENYELLOW);
  else tft.setTextColor(ILI9341_WHITE); 
  tft.setCursor(tMode_x, r2_label_y);
  tft.print("Mode");
  //
  if (nparm == 5) tft.setTextColor(ILI9341_GREENYELLOW);
  else tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(tTH_x, r3_label_y);
  tft.print("InspP");
  if (nparm == 6) tft.setTextColor(ILI9341_GREENYELLOW);
  else tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(tPMin_x, r3_label_y);
  tft.print("PMin");
  if (nparm == 7) tft.setTextColor(ILI9341_GREENYELLOW);
  else tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(tPMax_x, r3_label_y);
  tft.print("PMax");
  if (nparm == 8) tft.setTextColor(ILI9341_GREENYELLOW);
  else tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(tRun_x, r3_label_y);
  tft.print("Run");
}

void select_changeable_parameter(uint8_t nparm){
  /*
  Runs over all labels and vlaues. Setting the highlighted one
  to orange-label and greenyellow-value.
  Can also be used to clear highlighting by seinging nparm=0
  */
  tft.setTextSize(2);
  //
  if (nparm == 1) tft.setTextColor(ILI9341_ORANGE);
  else tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(tRR_x, r2_label_y);
  tft.print("R.R.");
  if (nparm == 1) tft.setTextColor(ILI9341_GREENYELLOW);
  else tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(tRR_x, r2_value_y);
  tft.print(String(vRR,1));
  //
  if (nparm == 2) tft.setTextColor(ILI9341_ORANGE);
  else tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(tIT_x, r2_label_y);
  tft.print("InspT");
  if (nparm == 2) tft.setTextColor(ILI9341_GREENYELLOW);
  else tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(tIT_x, r2_value_y);
  tft.print(String(vIT, 1));
  //
  if (nparm == 3) tft.setTextColor(ILI9341_ORANGE);
  else tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(tVMax_x, r2_label_y);
  tft.print("VMax");
  if (nparm == 3) tft.setTextColor(ILI9341_GREENYELLOW);
  else tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(tVMax_x, r2_value_y);
  tft.print(String(vVMax, 1));
  //
  if (nparm == 4) tft.setTextColor(ILI9341_ORANGE);
  else tft.setTextColor(ILI9341_WHITE); 
  tft.setCursor(tMode_x, r2_label_y);
  tft.print("Mode");
  if (nparm == 4) tft.setTextColor(ILI9341_GREENYELLOW);
  else tft.setTextColor(ILI9341_WHITE); 
  tft.setCursor(tMode_x, r2_value_y);
  if (vMode) tft.print("Vol");
  else tft.print("Pres");
  //
  if (nparm == 5) tft.setTextColor(ILI9341_ORANGE);
  else tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(tTH_x, r3_label_y);
  tft.print("InspP");
  if (nparm == 5) tft.setTextColor(ILI9341_GREENYELLOW);
  else tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(tTH_x, r3_value_y);
  tft.print(String(vTH, 1));
  //
  if (nparm == 6) tft.setTextColor(ILI9341_ORANGE);
  else tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(tPMin_x, r3_label_y);
  tft.print("PMin");
  if (nparm == 6) tft.setTextColor(ILI9341_GREENYELLOW);
  else tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(tPMin_x, r3_value_y);
  tft.print(String(vPMin, 1));  
  //
  if (nparm == 7) tft.setTextColor(ILI9341_ORANGE);
  else tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(tPMax_x, r3_label_y);
  tft.print("PMax");
  if (nparm == 7) tft.setTextColor(ILI9341_GREENYELLOW);
  else tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(tPMax_x, r3_value_y);
  tft.print(String(vPMax, 1));
  //
  if (nparm == 8) tft.setTextColor(ILI9341_ORANGE);
  else tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(tRun_x, r3_label_y);
  tft.print("Run");
  if (nparm == 8) tft.setTextColor(ILI9341_GREENYELLOW);
  else tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(tRun_x, r3_value_y);
  if (vRun) tft.print("ON");
  else tft.print("OFF");
}


void change_parameter_value(uint8_t nparm, int8_t delta){
  /* 
  If the value is changed by the encoder that means it must be
  selected and therefore should be greenyellow
  */
  tft.setTextColor(ILI9341_GREENYELLOW);
  tft.setTextSize(2);
  switch (nparm){
    case 1: //RR
      change_RR(delta);
      tft.writeFillRect(tRR_x, r2_value_y, 70, 16, ILI9341_BLACK);
      tft.setCursor(tRR_x, r2_value_y);
      tft.print(String(vRR,1));  
      break;
    case 2:
      change_IT(delta);
      tft.writeFillRect(tIT_x, r2_value_y, 70, 16, ILI9341_BLACK);
      tft.setCursor(tIT_x, r2_value_y);
      tft.print(String(vIT,1));
      break;
    case 3:
      change_VMax(delta);
      tft.writeFillRect(tVMax_x, r2_value_y, 70, 16, ILI9341_BLACK);
      tft.setCursor(tVMax_x, r2_value_y);
      tft.print(int(vVMax));
      break;
    case 4:
      change_Mode();
      tft.writeFillRect(tMode_x, r2_value_y, 70, 16, ILI9341_BLACK);
      tft.setCursor(tMode_x, r2_value_y);
      if (vMode) tft.print("Vol");
      else tft.print("Pres");
      break;
    case 5:
      change_TH(delta);
      tft.writeFillRect(tTH_x, r3_value_y, 70, 16, ILI9341_BLACK);
      tft.setCursor(tTH_x, r3_value_y);
      tft.print(String(vTH, 1));
      break;
    case 6:
      change_PMin(delta);
      tft.writeFillRect(tPMin_x, r3_value_y, 70, 16, ILI9341_BLACK);
      tft.setCursor(tPMin_x, r3_value_y);
      tft.print(String(vPMin,1));
      break;
    case 7:
      change_PMax(delta);
      tft.writeFillRect(tPMax_x, r3_value_y, 70, 16, ILI9341_BLACK);
      tft.setCursor(tPMax_x, r3_value_y);
      tft.print(String(vPMax,1));
      break;
    case 8:
      change_Run();
      tft.writeFillRect(tRun_x, r3_value_y, 70, 16, ILI9341_BLACK);
      tft.setCursor(tRun_x, r3_value_y);
      if (vRun) tft.print("ON");
      else tft.print("OFF");      
      break;
  }
}


float get_rand(double rmin, double rmax){
  rand_long = random();
  unsigned long rand_long = random();
  return float(rmin + (rmax - rmin)* ((double)rand_long / 4.2949e9));
}
bool parseMode(uint32_t p){
  //Run Mode (0=Vol, 1=Press)
  if (p == 0) return true;
  return false;
}
bool parseRun(uint32_t p){
  // Run State (0=forceOff, 1=forceOn, 2=runOff, 3=runOn)
  if (p == 3) return true;
  return false;
}


// Mesage Parameters from Control-Arduino
float ambu_float[10];
uint32_t ambu_int[2];
bool gotMsg;
bool updateParms;
bool guiParamEditing = false;  // encoder turns change value 

void loop(){
  curTime = millis();
  gotMsg = false;
  updateParms = false;
  //
  // Check encoder for update
  //
  encDT = 0;
  encPushed = false;
  if ( digitalRead(pin_switch) == false ) {
    if (curTime - encLastPTime > encPDeadTime) {
      encLastPTime = curTime;
      encPushed = true;
    }
  }
  stateA = digitalRead(pin_encoderA);
  stateB = digitalRead(pin_encoderB);
  if (stateA == false  &&  encPushed == false) {
    // Here deal with turned encoder input.
    // I dont want to process pushes and turns at the same
    // time since they shouldn't happen in the same cycle. 
    if (curTime - encLastTTime > encTDeadTime) {
      encLastTTime = curTime;
      if (stateB != stateA) encDT = -1;
      else encDT = 1;
    //Serial.println(encDT);
    }
  }
  if (encDT!=0 || encPushed) {
    // update the timeout timer since some input was recieved
    guiPrevActionTime = curTime;
  }
  //
  // Take actions based on encoder input
  //
  if ( encDT != 0 ) { 
    Serial.println("Encoder Turned");
    if (guiParamSelected == 0){
      // if no parameter is selected, select the first
      guiParamSelected = 1;
      highlight_changeable_parameter(guiParamSelected);
      //select_changeable_parameter(guiParamSelected);
    }
    else {
      // A parameter is activly selected or highlighted
      if (guiParamEditing == false) {
        // the parameter is only highlighted
        // turning encoder should move to the next parameter
        // change the previous selected one to unhighlighted:
        guiParamSelected = guiParamSelected + encDT;
        if (guiParamSelected < 1)       guiParamSelected = 8;
        else if (guiParamSelected > 8)  guiParamSelected = 1;
        highlight_changeable_parameter(guiParamSelected);
        Serial.print("Enc Turned and edit=false, encDT=");
        Serial.println(encDT);
      }
      else {
        // the parmater is selected and the value should be changed
        Serial.print("Enc Turned and edit=true, encDT=");
        Serial.println(encDT);
        change_parameter_value( guiParamSelected, encDT );
        // Send the changed value to AmBuConfig
        /*
        */
        Message msg;
        Serial.print("Sending MSG: ");
        Serial.print(guiParamSelected);
        Serial.print("=");
        // msg.writeData(Message::PARAM_FLOAT,0,1,&floatData,1,intData);
        uint32_t iA[2];
        switch (guiParamSelected){
          case 1: // RR = 0
            iA[0] = 1;
            msg.writeData(Message::PARAM_FLOAT,0,1,&vRR,1,&iA[0]);
            Serial.print(String(vRR));
            break;
          case 2: // IT = 1
            iA[0] = 2;
            msg.writeData(Message::PARAM_FLOAT,0,1,&vIT,1,&iA[0]);
            Serial.print(String(vIT));
            break;
          case 3: // VMax
            iA[0] = 5;
            msg.writeData(Message::PARAM_FLOAT,0,1,&vVMax,1,&iA[0]);
            Serial.print(String(vVMax));
            break;
          case 4: // Mode
            iA[0] = 11;
            if (vMode == true) iA[1]=0; // 0=Vol
            else iA[1] = 1; // 1=Press
            msg.writeData(Message::PARAM_INTEGER,0,0,NULL,2,iA);
            Serial.print("(VMode)");
            Serial.print(vMode);
            break;
          case 5: // TH
            iA[0] = 7;
            msg.writeData(Message::PARAM_FLOAT,0,1,&vTH,1,&iA[0]);
            Serial.print(String(vTH));
            break;
          case 6: // PMin
            iA[0] = 8;
            msg.writeData(Message::PARAM_FLOAT,0,1,&vPMin,1,&iA[0]);
            Serial.print(String(vPMin));
            break;
          case 7: // PMax
            iA[0] = 3;
            msg.writeData(Message::PARAM_FLOAT,0,1,&vPMax,1,&iA[0]);
            Serial.print(String(vPMax));
            break;
          case 8: // Run
            iA[0] = 9;
            if (vRun == true) iA[1]=3; //2=runOff, 3=runOn
            else iA[1] = 2;
            msg.writeData(Message::PARAM_INTEGER,0,0,NULL,2,iA);
            Serial.print("(Run)");
            Serial.print(vRun);
            break;
        }
        masterComm.send(msg);
      }
    }
  }
  else if (encPushed) {
    Serial.println("Encoder Pushed");
    if (guiParamEditing == false){
      guiParamEditing = true;
    } 
    else {
      guiParamEditing = false;
    }
    select_changeable_parameter(guiParamSelected);
  }

  if ( curTime - guiPrevActionTime > guiTimeout && guiParamSelected != 0 ) {
    // If timeout time is exceeded w/out input reset the selected/highlighted
    guiParamSelected = 0;
    guiParamEditing = false;
    select_changeable_parameter(0); //sets both label and value to white
    // Serial.println("GUI timeout");
  }

  if (curTime - prevUpdateTime > guiRefresh){
    Message msg;
    masterComm.read(msg);
    Serial.print("Got Message ");
    Serial.print(msg.nFloat());
    Serial.print("/");
    Serial.print(msg.nInt());
    Serial.println("F/I");
    /* 
    From nano_control_superior.ino (~L122) expect 10 floats and 2 ints
      void getFloat(float *f) {for (unsigned i=0;i<_nFloat;i++) f[i]=_tempFloat[i]; }
      void getInt(uint32_t *d)  {for (unsigned i=0;i<_nInt;i++) d[i]=_tempInt[i];   }
    */
    if (msg.nFloat()==10 && msg.nInt()==2){
      gotMsg = true;
      Serial.println("Got Valid Update Message");
      msg.getFloat(ambu_float);
      msg.getInt(ambu_int);
      vPEEP = ambu_float[0]; 
      vPIP  = ambu_float[1];
      vVOL  = ambu_float[2];
      vIE   = ambu_float[3];
      if (vRR != ambu_float[4] && !(guiParamSelected==1 && guiParamEditing==true)){
        vRR   = ambu_float[4];
        updateParms = true;
      }
      if (vIT != ambu_float[5] && !(guiParamSelected==2 && guiParamEditing==true)){
        vIT   = ambu_float[5];
        updateParms = true;
      }
      if (vTH != ambu_float[6] && !(guiParamSelected==5 && guiParamEditing==true)){
        vTH   = ambu_float[6];
        updateParms = true;
      }
      if (vVMax != ambu_float[7] && !(guiParamSelected==3 && guiParamEditing==true)){
        vVMax = ambu_float[7];
        updateParms = true;
      }
      if (vPMin != ambu_float[8] && !(guiParamSelected==6 && guiParamEditing==true)){
        vPMin = ambu_float[8];
        updateParms = true;
      }
      if (vPMax != ambu_float[9] && !(guiParamSelected==7 && guiParamEditing==true)){
        vPMax = ambu_float[9];
        updateParms = true;
      }
      if (vMode != parseMode(ambu_int[1]) && !(guiParamSelected==4 && guiParamEditing==true)){
        //Run Mode (0=Vol, 1=Press)
        vMode = parseMode(ambu_int[1]);
        updateParms = true;
      }
      if (vRun != parseRun(ambu_int[0]) && !(guiParamSelected==8 && guiParamEditing==true)){
        // Run State (0=forceOff, 1=forceOn, 2=runOff, 3=runOn)
        vRun = parseRun(ambu_int[0]);
        updateParms = true;
      }
    }
    update_cycle_values();
    if (updateParms) update_parms();
    prevUpdateTime = curTime;
  }
}