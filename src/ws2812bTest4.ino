// Based on Teensy WS2812Serial BasicTest Example
//
// Test ws2812b switch array
//
// Attach the DIN pin(s) of ws2812b's thru a switch to
// a common bus to SWITCH_PIN, and when the corresponding
// pixel gets a signal (rising edge), if the button is pressed,
// it will generate an interrupt.  From that interrupt we can
// determine which button, if any, was pressed.
//
// I use a 10K pulldown resister on the SWITCH_PIN.
//
// Otherwise, it uses Paul's well documented and well written
// WS2812Serial.h library to write the pixels and a hodge-podge
// of other stuff to present a flashy UI.
//
// I made a 3D printed button that snaps on top of ws2812's on
// these ws3812 arrays.  Please see Hackaday for the STl files.
//
// The main problem with this approach is that (a) it cannot detect
// multiple button presses, and (b) if there ARE multiple buttons pressed
// simultaneously, the DIN pins will be connnected together and the LED's
// will not work correctly.
//
// (b) can be solved by merely putting a diode between the switch
// and the bus line so that the signal only goes one way.
//
// As I'm planning to make a foot pedal midi controller, I think
// that (a) can be mostly mitigated by dividing the array into
// two parts (left and right).  So, the right foot could, for example,
// press a "shift" key, and the left foot could select a functionn.
//
// Perhaps someone can think of a way to get around detecting multiple
// simultaneous keypresses.


#include "demo_ui.h"
#include "myDebug.h"

demoUI ui;


void setup()
{
    Serial.begin(115200);
    delay(1000);
    display(0,"started ...",0);
    ui.init();
}


void loop()
{
    ui.onLoop();
}   
