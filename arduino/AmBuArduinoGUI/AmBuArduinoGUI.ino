#include "Arduino.h"
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "GUI.h"


#define SPI_DEFAULT_FREQ 20000000

static constexpr uint8_t pPEEP=0;
static constexpr uint8_t pPIP= 1;
static constexpr uint8_t pVol=  2;
static constexpr uint8_t pRR= 3;
static constexpr uint8_t pIH= 4;
static constexpr uint8_t pTH= 5;
static constexpr uint8_t pVmax= 6;
static constexpr uint8_t pPmax= 7;
static constexpr uint8_t pPmin= 8;
static constexpr uint8_t nParam = 9;


//gets data from master
float parms[nParam]={0.f};

GUI_value gui_value[nParam]={
  {.name="PEEP",  //Last cycle min pressure
   .id=pPEEP,
   .val=&parms[pPEEP],
   .dval=60.0f, 
   .dx=2.0f, 
   .min=20.0f, 
   .max=100.0f,
    .fmt="%02.1f"
  },
  {.name="PIP",  //last cycle max pressure
   .id=pPIP,
   .val=&parms[pPIP],
   .dval=0.0f, 
   .dx=2.0f, 
   .min=0.0f, 
   .max=30.0f,
   .fmt="%02.1f"
  },
  {.name="Vol",  // last cycle volume mL
   .id=pVol,
   .val=&parms[pVol],
   .dval=500.0f, 
   .dx=10.0f, 
   .min=200.0f, 
   .max=1000.0f,
   .fmt="%03d"
  },
  {.name="RR",    // resp rate in Hz
   .id=pRR,
   .val=&parms[pRR],
   .dval=0.0f, 
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
   .dval=5.0f,       
   .dx=0.1f, 
   .min=-10.0f, 
   .max=0.0f,
   .fmt="%02.1f"
  },
  {.name="Vmax",  //Threshold where volume will stop IH short
   .id=pVmax,
   .val=&parms[pVmax],
   .dval=600.0f, 
   .dx=10.0f, 
   .min=250.0f, 
   .max=1000.0f,
   .fmt="%03d"
  },
  {.name="Pmax",  //Threshold where pressure will stop IH short
   .id=pVmax,
   .val=&parms[pVmax],
   .dval=600.0f, 
   .dx=10.0f, 
   .min=250.0f, 
   .max=1000.0f,
   .fmt="%03d"
  },
  {.name="Pmin",  //Threshold where pressure will alarm
   .id=pVmax,
   .val=&parms[pVmax],
   .dval=600.0f, 
   .dx=10.0f, 
   .min=250.0f, 
   .max=1000.0f,
   .fmt="%03d"
  }
};


GUI gui;

void setup_display(){
  uint8_t fs = 2;
  uint16_t  y = 1; 
  uint16_t x;
  x =  fs*1.5*6;
  gui.addItem( gui_value[pPEEP], {x,y,ILI9341_BLUE,fs,3});   
  x =  GUI::TFT_THIRD + fs*1.5*6;
  gui.addItem(gui_value[pPIP],  {x,y,ILI9341_GREEN,fs,3});   
  x =  2*GUI::TFT_THIRD + fs*1.5*6;
  gui.addItem( gui_value[pVol], {x,y,ILI9341_CYAN,fs,3});   
  gui.setup();
}

void update_display() {
  gui.update();
}

double get_rand(double rmin, double rmax){
  unsigned long rand_long = random();
  return rmin + (rmax - rmin)* ((double)rand_long / 4.2949e9);
}


void setup() {
  delay(2000);
  Serial.begin(9600);
  Serial.print("startup\n");
  setup_display();
}
uint32_t measTime = 0;
 unsigned counter=0;
void loop() {
  char s[64];
  uint32_t curTime = millis();
  
  if ( (curTime - measTime) > 1000 ){
    // Take a sudo measurments
    for(unsigned i=0;i<nParam;i++) {
      parms[i]=get_rand(gui_value[i].min,gui_value[i].max);
    }       
    Serial.print("Measure ");
    for(unsigned int i=0;i<nParam;i++) {
      sprintf(s,"%2.2f ",*gui_value[i].val);
      Serial.print(s);
    }
    Serial.print("\n");
    update_display();
    measTime=curTime;
  }
  

}
