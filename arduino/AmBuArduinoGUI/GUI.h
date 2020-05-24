/*
I create a gui parmameter than knows where it is on the screen
and can be sent parameters to update it as well as adjust the 
values. 
Values can be updated, but labels are assumed to never change.
Value updates include the number shown and the color to highlight
the ability to change the set value.

*/

#ifndef _GUI_H_
#define _GUI_H_



#include "Arduino.h"
#include "Adafruit_ILI9341.h"
#include <stdint.h>

using GUI_elem  = struct  _GUI_elem {
  uint16_t x;
  uint16_t y;
  uint16_t label_color;
  uint16_t label_fsize;
  uint16_t value_color;
  uint16_t value_fsize;
};

using GUI_value = struct  _GUI_value  {
  const char *name;
  uint8_t id;
  float *val;
  float dval;
  float dx;
  float min;
  float max;
  const char *fmt;
};

using  GUI_item = struct  _GUI_item   {
  GUI_value val;
  GUI_elem elem;
  float old_value;
};

using GUI_text = struct  _GUI_text   {
  const char* val;
  GUI_elem elem;
  char old[64];
};


class GUI
{
 public:
  	GUI(bool invert=false);	
	void addItem(const GUI_value &gui_val,const GUI_elem &elem);
	void addText(const char *value,const GUI_elem &elem) {}
	void setup();
	void update();

	static constexpr uint32_t SPI_DEFAULT_FREQ=20000000;
	static constexpr uint8_t TFT_DC=10;
	static constexpr uint8_t TFT_CS= -1;
	static constexpr uint8_t TFT_MOSI= 11;
	static constexpr uint8_t TFT_MISO= -1;
	static constexpr uint8_t TFT_RST= 9;
	static constexpr uint8_t TFT_CLK= 13;	
	static constexpr uint8_t TFT_FONTH_1 = 8*1;
	static constexpr uint8_t TFT_FONTH_2=  8*2;
	static constexpr uint8_t TFT_FONTH_3 = 8*3;
	static constexpr uint8_t TFT_FONTW = 6;
	static constexpr uint8_t TFT_WIDTH= 240;
	static constexpr uint8_t TFT_HEIGHT= 320 ; 
	static constexpr uint8_t TFT_THIRD = TFT_WIDTH/3;
	static constexpr uint8_t TFT_HALF = TFT_WIDTH/2;
	static constexpr uint8_t LABEL_VALUE_YSPACE = 7;
	
	
 private:
	static constexpr uint8_t nItems=16;

	uint16_t _color(uint16_t color);
	// hold 16 GUI items
	GUI_item items[nItems];
	static Adafruit_ILI9341 tft;
	bool invert_colors;
	GUI_text labels;
};

#endif
