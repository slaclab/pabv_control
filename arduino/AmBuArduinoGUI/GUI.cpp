#include "Arduino.h"
#include "GUI.h"
#include <string.h>

GUI::GUI(bool invert) : invert_colors(invert)
{
  for(unsigned int i=0;i<nItems;i++) items[i]={0,0,nullptr,0.0f,0.0f,0.0f,0.0f,0};
}

void GUI::addItem(const GUI_value &gui_value,const GUI_elem &elem) {
  items[gui_value.id]={gui_value,elem};
}

void GUI::update() {
  char valstr[64];
  for(unsigned i=0;i<nItems;i++) {
    const GUI_item &item=items[i];
    const GUI_value &val=item.val;
    const GUI_elem &elem=item.elem;
    
    if(val.name==0) continue;  
    tft.fillRect(0,30,80,TFT_FONTH_3,ILI9341_BLACK); // optimize me
    tft.setTextSize(elem.valsize);
    tft.setTextColor(_color(elem.color));
    uint16_t y=elem.y+elem.fsize*2*6*1.5;
    uint16_t x=elem.x;
    tft.setCursor(x,y);
    snprintf(valstr,sizeof(valstr),val.fmt,*val.val);
    tft.print(valstr);
  }

}

void GUI::setup(){
  /* 
  I setup the display with the fixed labels and such
  */
  tft=Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
  SPI.begin();
  tft.begin();
  tft.setRotation(0);
  tft.fillScreen(_color(ILI9341_BLACK));
  for(unsigned i=0;i<nItems;i++) {
    const GUI_item &item=items[i];
    const GUI_value &val=item.val;
    const GUI_elem &elem=item.elem;
    if(val.name==0) continue;
    tft.setTextSize(2);
    tft.setTextColor(_color(elem.color));
    tft.setCursor(elem.x,elem.y);
    tft.print(val.name);    
  }
}

uint16_t GUI::_color(uint16_t color) {
  if(!invert_colors) return color;
  if(color==ILI9341_MAROON) return ILI9341_PINK ;
  if(color==ILI9341_DARKGREEN) return ILI9341_GREEN;    
  if(color==ILI9341_DARKCYAN) return ILI9341_CYAN;
  if(color==ILI9341_WHITE) return ILI9341_BLACK;  
  return color;

}


Adafruit_ILI9341 GUI::tft =Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST); 
