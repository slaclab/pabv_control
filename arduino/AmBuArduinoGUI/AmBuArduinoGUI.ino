#include "Arduino.h"
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "GUI.h"


#define SPI_DEFAULT_FREQ 20000000

//encoder
#define pin_switch 7
#define pin_encoderA 6
#define pin_encoderB 5
uint8_t encoderVal = 0;
uint8_t encoderPushes = 0;
uint8_t gui_selected_p = 0;
const long encTDeadTime = 50; //millis
const long encPDeadTime = 500; //millis
long encLastPTime = 0; //millis
long encLastTTime = 0; //millis
bool stateA, stateB;

static constexpr uint8_t pPEEP=0;
static constexpr uint8_t pPIP= 1;
static constexpr uint8_t pVol=  2;
static constexpr uint8_t pRR= 3;
static constexpr uint8_t pIH= 4;
static constexpr uint8_t pTH= 5;
static constexpr uint8_t pVmax= 6;
static constexpr uint8_t pPmin= 7;
static constexpr uint8_t pPmax= 8;
static constexpr uint8_t nParam = 9;


//gets data from master
float parms[nParam]={0.f};

GUI_value gui_value[nParam]={
  {.name="PEEP",  //Last cycle min pressure
   .id=pPEEP,
   .val=&parms[pPEEP],
   .dval=10.0f, 
   .dx=2.0f, 
   .min=0.0f, 
   .max=30.0f,
   .fmt="%02.1f"
  },
  {.name="PIP",  //last cycle max pressure
   .id=pPIP,
   .val=&parms[pPIP],
   .dval=30.0f, 
   .dx=2.0f, 
   .min=20.0f, 
   .max=50.0f,
   .fmt="%02.1f"
  },
  {.name="Vol",  // last cycle volume mL
   .id=pVol,
   .val=&parms[pVol],
   .dval=500.0f, 
   .dx=10.0f, 
   .min=200.0f, 
   .max=1000.0f,
   .fmt="%3.0f"
  },
  {.name="RR",    // resp rate in Hz
   .id=pRR,
   .val=&parms[pRR],
   .dval=20.0f, 
   .dx=0.5f, 
   .min=10.0f,
   .max= 30.0f,
   .fmt="%02.1f"
  },
  {.name="IH",    //Inhale time in seconds
   .id=pIH,
   .val=&parms[pIH],
   .dval=1.0f,
   .dx= 0.1f, 
   .min=  0.5f, 
   .max = 2.5f,
   .fmt="%02.1f"
  },
  {.name="TH",    //threhsold cmH2O
   .id=pTH,
   .val=&parms[pTH],
   .dval=0.0f,       
   .dx=0.1f, 
   .min=-10.0f, 
   .max=30.0f,
   .fmt="%02.1f"
  },
  {.name="Vmax",  //Threshold where volume will stop IH short
   .id=pVmax,
   .val=&parms[pVmax],
   .dval=800.0f, 
   .dx=10.0f, 
   .min=250.0f, 
   .max=1000.0f,
   .fmt="%3.0f"
  },
  {.name="Pmin",  //Threshold where pressure will alarm
   .id=pPmin,
   .val=&parms[pPmin],
   .dval=-10.0f, 
   .dx=2.0f, 
   .min=-10.0f, 
   .max=30.0f,
   .fmt="%02.1f"
  },
  {.name="Pmax",  //Threshold where pressure will alarm
   .id=pPmax,
   .val=&parms[pPmax],
   .dval=10.0f, 
   .dx=2.0f, 
   .min=10.0f, 
   .max=60.0f,
   .fmt="%02.1f"
  }
};


GUI gui;

void setup_display(){
  //uint8_t fs = 2;
  uint16_t  y = 10; 
  uint16_t x;
  uint16_t vspace = 25;
  uint16_t hspace = 5;
  x =  hspace;
  gui.addItem( gui_value[pPEEP], {x,y,ILI9341_PINK,2,ILI9341_PINK,3});   
  x =  GUI::TFT_THIRD + hspace;
  gui.addItem(gui_value[pPIP],  {x,y,ILI9341_GREEN,2,ILI9341_GREEN,3});   
  x =  2*GUI::TFT_THIRD + hspace;
  gui.addItem( gui_value[pVol], {x,y,ILI9341_CYAN,2,ILI9341_CYAN,3});   
  y =  1 + 8*2 + 8*3 + 7 + vspace;
  x = hspace;
  gui.addItem( gui_value[pRR], {x,y,ILI9341_WHITE,2,ILI9341_WHITE,2});
  x =  GUI::TFT_THIRD + hspace;
  gui.addItem( gui_value[pIH], {x,y,ILI9341_WHITE,2,ILI9341_WHITE,2});
  x =  2*GUI::TFT_THIRD + hspace;
  gui.addItem( gui_value[pTH], {x,y,ILI9341_WHITE,2,ILI9341_WHITE,2});
  y =  1 + 8*2 + 8*3 + 7 + 14 + 8*2 + 7 + 8*2 + vspace;
  x = hspace;
  gui.addItem( gui_value[pVmax], {x,y,ILI9341_WHITE,2,ILI9341_WHITE,2});
  x =  GUI::TFT_THIRD + hspace;
  gui.addItem( gui_value[pPmin], {x,y,ILI9341_WHITE,2,ILI9341_WHITE,2});
  x =  2*GUI::TFT_THIRD + hspace;
  gui.addItem( gui_value[pPmax], {x,y,ILI9341_WHITE,2,ILI9341_WHITE,2});
  gui.setup();
}

void update_display() {
  gui.update();
}

double get_rand(double rmin, double rmax){
  unsigned long rand_long = random();
  return rmin + (rmax - rmin)*((double)rand_long / 4.2949e9);
}


void setup() {
  asm(".global _printf_float");
  Serial.begin(9600);
  Serial.print("startup\n");
  // Encoder Pins
  pinMode (pin_encoderA, INPUT_PULLUP);
  pinMode (pin_encoderB, INPUT_PULLUP);
  pinMode (pin_switch, INPUT_PULLUP);

  setup_display();
  delay(2000);
}


uint32_t measTime = 0;
unsigned counter=0;
char s[64];
uint32_t curTime;

void loop() {
  curTime = millis();
  // Check encoder for update
  if ( digitalRead(pin_switch) == false ) {
    if (curTime - encLastPTime > encPDeadTime) {
      encLastPTime = curTime;
      Serial.println("Switched");
      encoderPushes++;
      gui_selected_p = encoderPushes % 3;
      //update_display();
    }
  }
  stateA = digitalRead(pin_encoderA);
  stateB = digitalRead(pin_encoderB);
  if (stateA == false) {
    if (curTime - encLastTTime > encTDeadTime) {
      encLastTTime = curTime;
      //prevStateA = stateA;
      if (stateB != stateA) {
        encoderVal++;
        //update_parameter(1);
      }
      else {
        encoderVal--;
        //update_parameter(-1);
      }
    Serial.println(encoderVal);
    }
  }
  
  // Update cycle parameters:
  if ( (curTime - measTime) > 1000 ){
    // Take a sudo measurments of PEEP/PIP/Vol
    for(unsigned i=0;i<3;i++) {
      parms[i]=get_rand(gui_value[i].min, gui_value[i].max);
    }       
    Serial.print("Measure ");
    for(unsigned int i=0;i<nParam;i++) {
      sprintf(s, gui_value[i].fmt, *gui_value[i].val);
      Serial.print(s);
      Serial.print(" ");
    }
    Serial.print("\n");
    update_display();
    measTime=curTime;
  }
  
}
