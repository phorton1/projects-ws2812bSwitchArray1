

#include "ws2812SwitchArray.h"
#include <WS2812Serial.h>

#define LED_PIN         1       // write to DIN of first pixel
#define SWITCH_PIN      7       // sense if a button is pressed

// Usable LED pins:
//   Teensy LC:   1, 4, 5, 24
//   Teensy 3.2:  1, 5, 8, 10, 31   (overclock to 120 MHz for pin 8)
//   Teensy 3.5:  1, 5, 8, 10, 26, 32, 33, 48
//   Teensy 3.6:  1, 5, 8, 10, 26, 32, 33



#define MICROS_PER_LED      30      // microseconds per pixel before it sees rising edge
#define MICROS_TO_0TH_LED   475     // microseconds until the first led sees rising edge


byte drawingMemory[NUM_LEDS*3];         //  3 bytes per LED
DMAMEM byte displayMemory[NUM_LEDS*12]; // 12 bytes per LED
WS2812Serial leds(NUM_LEDS, displayMemory, drawingMemory, LED_PIN, WS2812_GRB);

elapsedMicros ws2812SwitchArray::show_time = 0;       // microseconds since showLeds started
int ws2812SwitchArray::button_pressed = 0;            // one based button pressed


ws2812SwitchArray::ws2812SwitchArray() 
{}


void ws2812SwitchArray::init()
{
    pinMode(SWITCH_PIN,INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(SWITCH_PIN), buttonIRQ, RISING);
    leds.begin();
}

byte *ws2812SwitchArray::getDrawingMemory()
{
    return drawingMemory;
}

void ws2812SwitchArray::showLeds()
{
    button_pressed = 0;
    show_time = 0;
    leds.show();
}


void ws2812SwitchArray::setPixel(int pixel_num, int color)
    // convenience method
{
    leds.setPixel(pixel_num, color);
}


void ws2812SwitchArray::setPixel(int row, int col, int color)
    // handles interleaving of my led array
{
    if (col % 2) row = NUM_ROWS - 1 - row;
    leds.setPixel(col * NUM_ROWS + row,color);
}


int ws2812SwitchArray::buttonPressed()
{
    return button_pressed;
}


void ws2812SwitchArray::buttonIRQ()
{
    if (!button_pressed)
    {
        int the_time = show_time;
        
        #if 0
            static int counter = 0;
            counter++;
            Serial.print(counter);
            Serial.print(" - IRQ encountered micros=");
            Serial.println(the_time);
        #endif

        button_pressed = ((the_time-MICROS_TO_0TH_LED) / MICROS_PER_LED) + 1;
    }
}

