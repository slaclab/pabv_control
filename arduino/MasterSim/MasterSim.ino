#include <Arduino.h>

#include <GUI.h>
#include <Comm.h>
Comm displayComm;
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
   .fmt="% 2.1f"
  },
  {.name="Vmax",  //Threshold where volume will stop IH short
   .id=pVmax,
   .val=&parms[pVmax],
   .dval=800.0f, 
   .dx=10.0f, 
   .min=250.0f, 
   .max=990.0f,
   .fmt="%3.0f"
  },
  {.name="Pmin",  //Threshold where pressure will alarm
   .id=pPmin,
   .val=&parms[pPmin],
   .dval=-10.0f, 
   .dx=2.0f, 
   .min=-10.0f, 
   .max=30.0f,
   .fmt="% 2.1f"
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


void setup() {
  asm(".global _printf_float");
  asm(".global _scanf_float");
  Serial.begin(9600);
  displayComm.begin();
  for(unsigned i=0;i<nParam;i++) 
    parms[i]=gui_value[i].dval;
}


uint32_t measTime = 0;
unsigned counter=0;
double get_rand(double rmin, double rmax){
  unsigned long rand_long = random();
  return rmin + (rmax - rmin)*((double)rand_long / 4.2949e9);
}
uint32_t curTime;
void loop() {
  curTime = millis();

  if ( (curTime - measTime) > 1000 ){
    // Take a sudo measurments of PEEP/PIP/Vol
    for(unsigned i=0;i<3;i++) {
      parms[i]=get_rand(gui_value[i].min, gui_value[i].max);
    }       
    displayComm.send(parms);
    measTime=curTime;
  }
}
