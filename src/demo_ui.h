#ifndef __demo_ui__h
#define __demo_ui__h

#include "ws2812SwitchArray.h"

#define MAX_ANIM_PHRASE  99


#define MODE_OFF            0
#define MODE_STARTING       1
#define MODE_COLORPICKER    2
#define MODE_CALCINTRO      3
#define MODE_CALCULATOR     4
#define MODE_STOPPING       5

#define NUM_MODES           6


#define PAD_SX    27
#define PAD_EX    30
#define PAD_SY    1
#define PAD_EY    6

#define PAD_XDIM  (PAD_EX-PAD_SX+1)
#define PAD_YDIM  (PAD_EY-PAD_SY+1)



class demoMode;
    // forward
    
    

class Rect
{
public:
    
    Rect(int x0,int y0,int x1,int y1)
    {
        sx = x0;
        sy = y0;
        ex = x1;
        ey = y1;
    }
    
    int sx;
    int sy;
    int ex;
    int ey;
    
    bool contains(int x, int y)
    {
        if (x>=sx && x<=ex && y>=sy && y<=ey)
            return true;
        return false;
    }
    
    int width()  { return ex-sx+1; }
    int height()  { return ey-sy+1; }

};




class demoUI : public ws2812SwitchArray
    // this class absorbs button presses, maintains
    // state, and updates a frame buffer in the onLoop() method,
    // to display a rudimentary example User Interface using
    // my ws2812b switch array hack on an 8x32 LED array.
    //
    // The ws2812b handling is specifically excluded from this
    // class so that the the main INO sketch contains the demo
    // logic for running the switch array.
{
public:
    
    demoUI();
    ~demoUI()  {}
    
    void init();
    
    void onLoop();
        // called directly from INO loop()

    // called by modes
    
    void setMode(int the_mode);
    void animateText(const char *text);
    bool animationActive() { return anim_raw_text; }

        // basic drawing methods
    
    void drawRect(Rect &r, int  color);
    void fillRect(Rect &r, int  color);
    void drawText(int x, int y, const char *string, int color, bool small_font=false, bool clip_display=true);
    void drawChar(int x, int y, const char c, int color,  bool small_font=false, bool clip_display=true);

    
protected:
 
    Rect rect_full;
    Rect rect_display;
    Rect rect_keypad;
    Rect rect_keypad_frame;
    
    elapsedMillis refresh_time;
    
    // buttons

    int last_button;
    int cur_button;
    int cur_row;
    int cur_col;
    elapsedMillis debounce;

    void handleButton();
    
    // animated text
    //    
    // The string is broken into "chunks" that define an overall animation.
    // Each chunk consists of a section of text, followed by parens containing
    // parameters.
    //
    // Some examples:
    //
    //      "TEST(g,-,[)TEST(g,-,3)TEST(b,r,)TEST(g,-,3)(-,-,6)BLAH(r,-,10)"
    //
    //          scroll the text "TEST" in green on a black backround
    //              in from the right until it gets to 0, then
    //
    //              show it in green/black for 3 frames, then
    //              show it in blue/red for one frame, and then
    //              show it for 3 more frames in green, then
    //
    //          clear the screen for 6 frames, then
    //          show the word "BLAH" in red/black for 10 frames
    //
    //  The parameters are  "TEXT(fg_color,bg_color,command_chars):
    //  where colors can be
    //
    //         rgbyopw = red,green,blue,yellow,orange,pink,white
    //         RGBYOPW = bright versions of same
    //         dr dg db = dark red, dark blue, or dark green 
    //    
    // and command chars can be
    //
    //         < scroll in from right until last char disappears off left
    //         > scroll in from left until last char disappears off right
    //         [ scroll in from right until last word is at 0,0
    //         ] scroll in from left until first char is at 0,0
    //         n = an integer number of frames
    //
    // Note that the animation "frame rate" is based on the constant
    // SCROLL_RATE number of refresh-frames.  In other words, (a)
    // SCROLL_RATE is smaller for faster animations, and (b) every
    // SROLL_RATE "refresh frames" we bump the animation on of it's
    // frames.
   
    
    const char *anim_raw_text;      // unparsed animation string to interpret
    const char *anim_next_piece;    // next piece to parse
    
    char anim_text[MAX_ANIM_PHRASE+1];   // the text for this parsed chunk

    int anim_fg_color;                   // color of text (default = white)
    int anim_bg_color;                   // color of text (default = black)
    int anim_steps;                      // number of (animation) frames remaining for this parsed step
    int anim_count;                      // counter for "refresh" to "animation" frame conversion
    
    const char *anim_last_word;          // pointer to last word for [ scrolling
    int anim_scroll_pos;                 // where to write the text
    int anim_scroll_dir;                 // in which direction, if any, to scroll

    void parseNextAnimPiece();
    void processAnimatedText();
    
    // modes
    
    demoMode *pMode;
    demoMode *modes[NUM_MODES];
    
};


#endif

