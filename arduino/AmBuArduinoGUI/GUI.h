/*
I create a gui parmameter than knows where it is on the screen
and can be sent parameters to update it as well as adjust the 
values.
*/

#ifndef _GUI_H_
#define _GUI_H_



#include "Arduino.h"
#include "Adafruit_ILI9341.h"
#include <stdint.h>

using GUI_pos  = struct {
  
  uint16_t xpos;
  uint16_t ypos;
  uint16_t fsize;
  uint16_t color;
  uint16_t valsize;
}

using GUI_value = struct  {
  float *value;
  float default;
  float min;
  float max;
  float dx;
  const char *valfmt;
}

using  GUI_item = struct {
  GUI_value val;
  GUI_pos pos;
  const char* label;
  float old_value;
}


class GUI
{
    public:
        GUI(bool invert=false);
	
        void addItem(const char* label,const GUI_pos &pos ,const &GUI_value);
      
	constexpr uint32_t SPI_DEFAULT_FREQ=20000000;
	constexpr uint8_t TFT_DC=9;
	constexpr uint8_t TFT_CS= -1;
	constexpr uint8_t TFT_MOSI= 11;
	constexpr uint8_t TFT_MISO= -1;
	constexpr uint8_t TFT_RST= 10;
	constexpr uint8_t TFT_CLK= 13;	
	constexpr uint8_t TFT_FONTH_1 = 8*1;
	constexpr uint8_t TFT_FONTH_2=  8*2;
	constexpr uint8_t TFT_FONTH_3 = 8*3;
	constexpr uint8_t TFT_FONTW = 6;
	constexpr uint8_t TFT_WIDTH= 240;
	constexpr uint8_t TFT_HEIGHT= 320 ; 
	constexpr uint8_t TFT_THIRD = TFT_WIDTH/3;
	constexpr uint8_t TFT_HALF = TFT_WIDTH/2;
	
 private:
	// hold 16 GUI items
	GUI_item items[16];
	uint16_t count;
	static Adafruit_ILI9341 tft;
	bool invert_colors;
};

#endif
