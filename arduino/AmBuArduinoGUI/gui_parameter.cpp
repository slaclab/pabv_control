#include "Arduino.h"
#include "gui_parameter.h"

gui_parameter::gui_parameter(Adafruit_ILI9341 *tft, uint16_t xpos, uint16_t ypos, uint16_t fsize, 
                    uint16_t valsize, float* parms, const char* pName, const char* valfmt) 
{
    value_ = parms[0];
    val_change_ = parms[1];
    val_min_ = parms[2];
    val_max_ = parms[3];
    val_fmt_ = valfmt;
    label_c_ = pName.length();
    label_w_ = label_c_ * fsize * 6;
    label_h_ = fsize*8;
    hspace_ = 5;
}

void gui_parameter::setup(){

}

void gui_parameter::draw(){

}