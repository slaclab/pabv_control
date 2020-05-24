#include "Arduino.h"
#include "GUI.h"
#include <string.h>

GUI::GUI(bool invert) : invert_colors(invert)
{
  for(unsigned int i=0;i<nItems;i++) {
    // This just sets empty GUI items up to nItems=16
    items[i]={0,0,nullptr,0.0f,0.0f,0.0f,0.0f,0};
  }
}

void GUI::addItem(const GUI_value &gui_value,const GUI_elem &elem) {
  items[gui_value.id]={gui_value, elem};
}

void GUI::update() {
  char valstr[64];
  for(unsigned i=0;i<nItems;i++) {
    const GUI_item &item=items[i];
    const GUI_value &val=item.val;
    const GUI_elem &elem=item.elem;
    if(val.name==0) {
      // Skip empty GUI items
      continue;  
    }
    // Get the new value formatted into valstr
    snprintf(valstr,sizeof(valstr),val.fmt,*val.val);
    // Need to clear the area behind the changing value:
    uint16_t y = elem.y + elem.label_fsize*8 + LABEL_VALUE_YSPACE;
    uint16_t x = elem.x;
    uint16_t w = strlen(valstr)*6*elem.value_fsize;
    // Clear the screen behind the value
    tft.fillRect(x, y, w, elem.value_fsize*8, _color(ILI9341_BLACK));
    tft.setTextSize(elem.value_fsize);
    tft.setTextColor(_color(elem.value_color));
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
  tft.setRotation(3);
  tft.fillScreen(_color(ILI9341_BLACK));
  for(unsigned i=0;i<nItems;i++) {
    const GUI_item &item=items[i];
    const GUI_value &val=item.val;
    const GUI_elem &elem=item.elem;
    if(val.name==0) continue;
    tft.setTextSize(elem.label_fsize);
    tft.setTextColor(_color(elem.label_color));
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
