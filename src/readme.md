# The Software

This directory includes the demo program that I show in the
[One Wire 3D Printed ws2812 Switch Array Demo](https://www.youtube.com/watch?v=qGlxH-YuUHk)
YouTube Video.

If you happen to have a Teensy 3.2 and an 8x32 LED array, then you
*might* be interested in trying to rebuild my exact demo.

**Proabably Not!!** :-)

Really, probably all you really want to know is **how it works** and
**how can I make it work for me** ... :-)

So, although I put a ton of effort into the demo program, sigh, to
implement the color picker and a full blown (although integer)
desktop calculator application :-) all that is actually interesting
in this part of the project is the very small **ws2812SwitchArray class**
that is merely a very thin wrapper around *Paul Stoffregen's*
**WS2812Serial** library.

**IMPORTANT NOTE:**  In the YouTube video I say that I'm using the FastLed
library.  I am not.

I am using **Paul Stoffregen's non-blocking WS2812Serial** library !!

Please support the Teensy and Paul and their website at https://www.pjrc.com!!


## My ws2812SwitchArray class

Here's pretty much the whole H file:

```
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
```

You call **init()** from your sketch's *setup()* method.

The **setPixel()** and **getDrawingMemory()** methods are just pass
throughs to the the "base class" (global static) leds stuff.
These are used to set the color bytes into arrays in memory.

Each time through your *loop()* method, at some point, you call
**showLeds()** to send the pixels to the array.

Then next time through the loop (at the top) you call **buttonPressed()**
to see if a button was presed during the previous call to showLeds().

*That's pretty much it.*

You may need to modify the number of rows and number of columns to
match your array or strip.   Other details, like the PINS used are
in the CPP file, which is also shown here in nearly it's entirety:

```
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
```

In order to calibrate this the first time, I did some measurements to
come up with values for *MICROS_TO_0TH_LED* and *MICROS_PER_LED* by including
the **#if 0** section of code in **buttonIRQ()**

Once more, the code goes like this:

(1) at some point **showLeds()** is called, and it sets both the elapsedMicros "show_time"
and one-based integer variable "button_pressed" to zero.

(2) Then (if a button is pressed) **the IRQ is triggered** and it figures out
which button was pressed and sets that into a button_pressed.

(3) **The client calls buttonPressed()** from their loop() method, at the top
of the next time through their loop, before they make the next showLeds()
call.


There's a bit of debouncing and higher level logic in the
**onLoop()** method in [demo_ui.cpp](demo_ui.cpp), but apart from
that, this is basically how it works.

And finally, please note a couple of other things.

* **This is only a demo**.  It is not intended to be a general purpose library
* The *technique* (but not necesarily the *CODE*) here should work with other CPUs and Libraries
* As an alternative to Interrupts and counting microseconds, for libraries using synchronous
  output or which disable interrupts, you'll have to get down to the low level output method
  and check for a high signal on the input line just after it sends out the rising edge(s).
  At that point, since you are sending out pixels! you should already KNOW what pixel you
  are talking to!
