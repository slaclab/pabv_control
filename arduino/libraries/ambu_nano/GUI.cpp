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

void GUI::change_value(uint8_t p, int8_t c) {
  // I expect dir to be +1 or -1 and we will adjust the value to the
  // next value by step dval
  const GUI_item &item = items[p];
  const GUI_value &val = item.val;
  *val.val += c*val.dx;
  if (*val.val > val.max) *val.val=val.max;
  if (*val.val < val.min) *val.val=val.min;
}

void GUI::update() {
  for(unsigned i=0;i<nItems;i++) {
    const GUI_item &item=items[i];
    const GUI_value &val=item.val;
    const GUI_elem &elem=item.elem;
    if(val.name==0) {
      // Skip empty GUI items
      continue;  
    }
    char valstr[64];
    // Get the new value formatted into valstr
    snprintf(valstr,sizeof(valstr),val.fmt,*val.val);
    // Need to clear the area behind the changing value:
    uint16_t y = elem.y + elem.label_fsize*8 + LABEL_VALUE_YSPACE;
    uint16_t x = elem.x;
    uint16_t w = strlen(valstr)*6*elem.value_fsize;
    // Clear the screen behind the value
    tft.fillRect(x, y, w, elem.value_fsize*8, _color(ILI9341_BLACK));
    tft.setTextSize(elem.value_fsize);
    if ( elem.selected ) {
      // if the item is selected for changing make it green
      tft.setTextColor(_color(ILI9341_GREEN));
    }
    else tft.setTextColor(_color(elem.value_color));
    tft.setCursor(x,y);
    snprintf(valstr,sizeof(valstr),val.fmt,*val.val);
    tft.print(valstr);
    if ( elem.highlight ) {
      // if the item is highlighted make the label green.
      tft.setTextColor(_color(ILI9341_GREEN));
      tft.setCursor(elem.x,elem.y); //location of label
      tft.print(val.name);
    }
    else {
      tft.setTextColor(_color(elem.label_color));
      tft.setCursor(elem.x,elem.y); //location of label
      tft.print(val.name);
    }
    
    /*
    Serial.print(val.name);
    Serial.print("(");
    Serial.print(x);
    Serial.print(".");
    Serial.print(y);
    Serial.print("x");
    Serial.print(w);
    Serial.print(".");
    Serial.print(elem.value_fsize*8);
    Serial.print("=");
    Serial.print(valstr);
    Serial.print(")");
    Serial.print("\n");
    delay(1000);
    */
  }
}

void GUI::setup(){
  /* 
  I setup the display with the fixed labels and such
  */
  tft=Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
  SPI.begin();
  tft.begin();
  tft.setRotation(2);
  tft.fillScreen(_color(ILI9341_BLACK));
  Serial.println("Start....");
  for(unsigned i=0;i<nItems;i++) {
    const GUI_item &item=items[i];
    const GUI_value &val=item.val;
    const GUI_elem &elem=item.elem;
    if(val.name==0) continue;
    tft.setTextSize(elem.label_fsize);
    tft.setTextColor(_color(elem.label_color));
    tft.setCursor(elem.x,elem.y);
    tft.print(val.name);    
    *val.val = val.dval;
    /*
    Serial.print(val.name);
    Serial.print("(");
    Serial.print(elem.x);
    Serial.print(".");
    Serial.print(elem.y);
    Serial.print(")");
    Serial.print("\n");
    delay(1000);
    */
  }
}

uint16_t GUI::_color(uint16_t color) {
  if(!invert_colors) return color;
  if(color==ILI9341_MAROON) return ILI9341_PINK ;
  if(color==ILI9341_DARKGREEN) return ILI9341_GREEN;    
  if(color==ILI9341_DARKCYAN) return ILI9341_CYAN;
  if(color==ILI9341_WHITE) return ILI9341_BLACK;
  // Dont we need the opposite functions?
  return color;
}


Adafruit_ILI9341 GUI::tft =Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST); 
