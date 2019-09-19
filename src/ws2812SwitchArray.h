#ifndef __ws2812SwitchArray_h__
#define __ws2812SwitchArray_h__

#include <Arduino.h>
#include <WS2812Serial.h>


#define NUM_ROWS  8
#define NUM_COLS  32
#define NUM_LEDS (NUM_ROWS * NUM_COLS)


class ws2812SwitchArray
{
public:
    
    ws2812SwitchArray();  
    ~ws2812SwitchArray()  {}
    
    void init();
    void showLeds();
    void setPixel(int pixel_num, int color);
    void setPixel(int row, int col, int color);
    
    byte *getDrawingMemory();
    
    int buttonPressed();
        // called at the top of the loop, before the next
        // call to showLeds() and after the previous one,
        // returns the one based button that was pressed,
        // if any.
        
protected:

    static int button_pressed;                     // button pressed 
    static elapsedMicros show_time;                // microseconds since showLeds started
    static void buttonIRQ();

};

#endif

