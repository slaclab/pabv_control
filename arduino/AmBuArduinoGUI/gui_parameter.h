/*
I create a gui parmameter than knows where it is on the screen
and can be sent parameters to update it as well as adjust the 
values.
*/

#ifndef gui_parameter_h
#define gui_parameter_h

#include "Arduino.h"
#include "Adafruit_ILI9341.h"

class gui_parameter
{
    public:
        gui_parameter(Adafruit_ILI9341 *tft, uint16_t xpos, uint16_t ypos, uint8_t fsize, 
                      float* parms, const char* pName, const char* numfmt);
        void highlight();
        void increment_val();
        void decriment_val();
        void unhighlight();
        void update(float v);
        void set_space(uint16_t s);
        float get_value();
        uint8_t get_value_fmt_len();

    private:
        float value_;
        float val_change_;
        float val_min_;
        float val_max_;
        char val_fmt_[10];
        char val_str_[5];
        char label_[5];
        // Text pixel widths/heights 
        uint8_t label_c_;
        uint8_t value_c_;
        uint16_t label_w_;
        uint16_t label_h_;
        uint16_t valtext_w_;
        uint16_t valtext_h_;
        uint16_t hspace_;

};

#endif