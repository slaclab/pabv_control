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

static inline constexpr uint8_t pPEEP=0;
static inline constexpr uint8_t pPIP= 1;
static inline constexpr uint8_t pVol=  2;
static inline constexpr uint8_t pRR= 3;
static inline constexpr uint8_t pIH= 4;
static inline constexpr uint8_t pTH= 5;
static inline constexpr uint8_t pVmax= 6;
static inline constexpr uint8_t pPmin= 7;
static inline constexpr uint8_t pPmax= 8;
static inline constexpr uint8_t nParam = 9;


using GUI_elem  = struct  _GUI_elem {
  uint16_t x;
  uint16_t y;
  uint16_t label_color;
  uint16_t label_fsize;
  uint16_t value_color;
  uint16_t value_fsize;
  bool highlight;
  bool selected;
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
	void change_value(uint8_t p ,int8_t c);
	// hold 16 GUI items
	static constexpr uint8_t nItems=16;
	GUI_item items[nItems];

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


	uint16_t _color(uint16_t color);
	
	static Adafruit_ILI9341 tft;
	bool invert_colors;
	GUI_text labels;
};

#endif
