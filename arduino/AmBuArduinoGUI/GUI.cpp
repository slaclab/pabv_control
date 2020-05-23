#include "Arduino.h"
#include "GUI.h"
#include <string.h>
void GUI:GUI {bool invert} : count(0),invert_colors(invert);

return &GUI GUI::addItem(const char* label,const GUI_pos &pos ,const &GUI_value) {
  GUI_item &item=items[count];
  item.val=val;
  item.pos=pos;
  item.label=lebel;;
  count++;
  return *this;
};

void setup::update() {
  

}

void setup(){
  /* 
  I setup the display with the fixed labels and such
  */
  if (invert_colors == false)  tft.fillScreen(ILI9341_WHITE);
  else tft.fillScreen(ILI9341_BLACK); // inverted
  for(unsigned i=0;i<count;i++) {
    tft.fillRect(0,30,80,TFT_FONTH_3,ILI9341_BLACK);
    tft.setCursor(1, 30);  
    tft.print(randp_peep,1);

  }
}

uint16_t invertColor(uint16_t color) {
  if(!in

}


static Adafruit_ILI9341 GUI::tft =Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST); 
