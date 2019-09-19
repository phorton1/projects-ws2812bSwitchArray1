#ifndef __mode__h
#define __mode__h

#include "demo_ui.h"
#include "myDebug.h"

#define BLACK               0
#define RED                 0x040000
#define DARK_RED            0x010000
#define BRIGHT_RED          0x080000
#define GREEN               0x000400
#define DARK_GREEN          0x000100
#define BRIGHT_GREEN        0x000800
#define BLUE                0x000004
#define DARK_BLUE           0x000001
#define BRIGHT_BLUE         0x000008
#define YELLOW              0x020400
#define BRIGHT_YELLOW       0x040800
#define PINK                0x040002
#define BRIGHT_PINK         0x080004
#define ORANGE              0x040200
#define BRIGHT_ORANGE       0x080400
#define WHITE               0x020202
#define BRIGHT_WHITE        0x070707
#define HIGHLIGHT_WHITE     0x303030

#define BUTTON_ONOFF_X      0
#define BUTTON_ONOFF_Y      0

#define BUTTON_MODE_X       1
#define BUTTON_MODE_Y       0

#define DEBUG_MODE  1



class demoMode : public ws2812SwitchArray
    // This is the base class for UI modes.
    //
    // The LED array is broken into three parts:
    // a display area, a keypad, and a frame
    // around the keypad.
    //
    // A mode manages the display area by calling
    // animateText() at various times.
    //
    // A mode explicitly determines the layout,
    // display, and functionality of the keypad.
    // It accepts button presses and maintains
    // state and can change to other modes.
    //
    // By default, upon a mode change, the default
    // display background color, as well as the keypad
    // and keypad frame colors are set to black, so the
    // simplest mode, MODE_OFF, merely blinks the ON/OFF
    // button, and if pressed, calls MODE_STARTING.
    //
    // The demoUI contains the onLoop method that first
    // calls the mode's virtual handleButton() method on any
    // new key presses allowing the mode to change state.
    //
    // The demoUI then calls the show() method on the
    // mode allowing it to change the animation and keypad
    // display characteristics.
    //
    // The demoUI then processes the keypad and display
    // (animations), and finally, if there is a button pressed,
    // and the mode is not MODE_OFF, it highlights any button
    // that might be pressed.
{
public:
    
    demoMode(demoUI *pui, int num) :
        pUI(pui)
    {
        mode_num = num;
        frame_counter = 0;
    }
    ~demoMode()  {}
    
   virtual void onModeSelected()
        // on any mode change stop any animations
    {
        frame_counter = 0;
        pUI->animateText(0);
        
    };
        
    virtual bool handleButton(int x, int y)
        // if the button press falls within the keypad,
        // the demoUI maps the coordinates into the keypad
        // and calls this method.  Base class handles
        // OFF and mode changes;
    {
        // on off button
        
        if (x == BUTTON_ONOFF_X &&
            y == BUTTON_ONOFF_Y)
        {
            pUI->setMode(MODE_STOPPING);
            return true;
        }
        else if (x == BUTTON_MODE_X &&
                 y == BUTTON_MODE_Y)
        {
            if (mode_num == MODE_COLORPICKER)
                pUI->setMode(MODE_CALCINTRO);
            else if (mode_num == MODE_CALCINTRO)
                pUI->setMode(MODE_CALCULATOR);
            else
                pUI->setMode(MODE_COLORPICKER);
            return true;
        }
        return false;
    }        
        
        
    virtual void onFrame()     { frame_counter++; }
        // called before default painting
    virtual void onDraw()      {}
        // called after default painting
    
    virtual int getDefaultBackgroundColor()  { return BLACK; }       
    virtual int getDefaultForegroundColor()  { return WHITE; }
    virtual int getKeypadColor(int x, int y) { return BRIGHT_WHITE; }
    virtual int getKeypadFrameColor()        { return DARK_BLUE; }
        // called by the DemoUI to draw the display background,
        // and set the default animated text colors, it gets
        // the color of the keypad frame and each key by calling
        // these methods.  After that it processes the animations,
        // and finally it highlights any pressed key.

protected:
    
    demoUI *pUI;
    int frame_counter;
    int mode_num;
    
};






class modeOff : public demoMode
{
public:

    modeOff(demoUI *pui, int num) :
        demoMode(pui, num) {}
    ~modeOff() {}
    
  
    virtual bool handleButton(int x, int y)
    {
        if (x == BUTTON_ONOFF_X &&
            y == BUTTON_ONOFF_Y)
        {
            pUI->setMode(MODE_STARTING);
            return true;
        }
        return false;
    }
    
    virtual int getKeypadFrameColor()
    {
        return BLACK;
    }
    
    virtual int getKeypadColor(int x, int y)
    {
        if (x == BUTTON_ONOFF_X &&
            y == BUTTON_ONOFF_Y &&
            frame_counter % 150 < 5)    // turn on green for 5 out of every 30 frames
        {
            return BRIGHT_GREEN;
        }
        return BLACK;
    }
};





class modeStarting : public demoMode
{
public:
    
    modeStarting(demoUI *pui, int num) :
        demoMode(pui,num) {}
    ~modeStarting() {}
    
    
    virtual void onModeSelected()
    {
        frame_counter = 0;
        pUI->animateText("  ONE WIRE(r,,<)3D Printed(o,,<)WS2812(p,,<)SWITCH ARRAY DEMO(g,-,[)DEMO(r,-,3)DEMO(b,-,3)DEMO(g,-,3)DEMO(r,-,3)DEMO(b,-,3)DEMO(g,-,3)DEMO(r,-,3)DEMO(W,-,35)");
    }
    
    virtual void onFrame()
    {
        // After the startup animation has run it's course,
        // switch to color picker mode
        
        frame_counter++;
        if (frame_counter == 2100)
            pUI->setMode(MODE_COLORPICKER);
    }
};




class modeStopping : public demoMode
{
public:
    
    modeStopping(demoUI *pui, int num) :
        demoMode(pui,num) {}
    ~modeStopping() {}
    
    virtual bool handleButton(int x, int y)
    {
        // go directly to OFF mode if any presses during shutdown
        pUI->setMode(MODE_OFF);
        return false;
    }
    
    virtual void onModeSelected()
    {
        frame_counter = 0;
        pUI->animateText("BYE!!(g,,[)BYE!!(g,,20)(,,5) and(o,,20)(,,5)THANKS for Watching!!(p,,<)(,,30)");
    }
    
    virtual void onFrame()
    {
        frame_counter++;

        // After the startup animation has run it's course,
        // go to OFF mode
        
        if (!pUI->animationActive())
            pUI->setMode(MODE_OFF);
    }
    
    
};







class modeColorPicker : public demoMode
{
public:
    
    modeColorPicker(demoUI *pui, int num) :
        demoMode(pui,num) {}
    ~modeColorPicker() {}
    

    virtual bool handleButton(int x, int y)
    {
        if (y)
        {
            color_picked = pickColor(x,y,false);   
            pUI->animateText(0);
            num_colors_picked++;
            return true;
        }
        
        return demoMode::handleButton(x,y);
    }
    
    virtual void onModeSelected()
    {
        frame_counter = 0;
        num_colors_picked = 0;
        color_picked = WHITE;
        pUI->animateText(0);
    }

    virtual void onFrame()
    {
        // After the animation has run it's course, switch to
        // color picker mode
        
        frame_counter++;
        if (!pUI->animationActive())
        {
            if (num_colors_picked > 2)
                pUI->animateText("Try the mode button(,,<)(,,30)");
            else
                pUI->animateText("Pick a color(,,[)color(,,30)(,,30)");
        }
    }
    
    virtual int getKeypadColor(int x, int y)
    {
        if (y)
        {
            return pickColor(x,y,true);
        }
        if (num_colors_picked > 2 &&
            x == BUTTON_MODE_X &&
            y == BUTTON_MODE_Y &&
            frame_counter % 20 < 6)    // flash the MODE button
        {
            return HIGHLIGHT_WHITE;
        }
        return WHITE;
    }
    
    
    virtual int getDefaultForegroundColor()
    {
        return color_picked;
    }
    
private:
    
    int color_picked = WHITE;
    int num_colors_picked;
    
    int pickColor(float x, float y, bool for_button)
    {
        float rd = sqrt( (x-0.5)*(x-0.5) + (y-1)*(y-1) );
        float gd = sqrt( (x+0.5)*(x+0.5) + (5-y)*(5-y) );
        float bd = sqrt( (3-x)*(3-x) + abs(3-y)*abs(3-y) );        
        int r = 4 - rd;
        int g = 4 - gd;
        int b = 5 - bd;

        if (!x) b = 0;
        
        if (r < 0) r = 0;
        if (g < 0) g = 0;
        if (b < 0) b = 0;

        if (for_button)
        {
            r *= 2;
            g *= 2;
            b *= 2;
        }
        return (r << 16) + (g << 8) + b;
    }
    
};



class modeCalcIntro : public demoMode
{
public:
    
    modeCalcIntro(demoUI *pui, int num) :
        demoMode(pui,num) {}
    ~modeCalcIntro() {}
    

    virtual int getKeypadColor(int x, int y)
    {
        if (frame_counter > 720 && frame_counter < 850)
        {
            if (frame_counter % 7 < 3)
                return BRIGHT_WHITE;
        }
        return 0;
    }
    
    
    virtual bool handleButton(int x, int y)
    {
        if (!demoMode::handleButton(x,y))
            pUI->setMode(MODE_CALCULATOR);
        return true;
    }
    
    virtual void onModeSelected()
    {
        frame_counter = 0;
        pUI->animateText("Calculator(g,,<)");
    }

    virtual void onDraw()
    {
        if (frame_counter < 720)
        {
            int digit = frame_counter / 80;
            pUI->drawChar(PAD_SX-1,PAD_SY,'0'+(9-digit),BRIGHT_RED,true,false);
        }
        if (frame_counter > 900)
            pUI->setMode(MODE_CALCULATOR);
    }
    
};





class modeCalculator : public demoMode
{
public:
    
    #define FUNCTION_ADD   10
    #define FUNCTION_SUB   11
    #define FUNCTION_MUL   12
    #define FUNCTION_DIV   13
    #define FUNCTION_EQ    14
    #define FUNCTION_CLEAR 15
    
    int value;
    int fxn;
    int accum;
    bool num_started;
    
    
    modeCalculator(demoUI *pui, int num) :
        demoMode(pui,num)
    {}
    
    ~modeCalculator() {}

    virtual void onModeSelected()
    {
        frame_counter = 0;
        pUI->animateText(0);
        doOp(FUNCTION_CLEAR);
    }

    virtual void onDraw()
    {
        if (fxn)
            pUI->drawChar(-1,1,opChar(fxn),GREEN,true);
        
        int use_value = fxn == FUNCTION_EQ && !num_started ?
            accum : value;
        
        char buf[100];
        sprintf(buf,"%d",use_value);

        int off = 0;            
        int len = strlen(buf);
        if (len > 5)
        {
            off = len - 5;
            len = 5;
        }
        int at = 5 - len;
        
        pUI->drawText(1+at*5,1,&buf[off],GREEN,true);
    }
    
    
    virtual int getKeypadColor(int x, int y)
    {
        // draw the numeric keys in blue
        // and function keys in various colors
        
        int key = mapKey(x,y);
        if (key == -1)
            return WHITE;
        if (key < FUNCTION_ADD)
            return GREEN;
        if (key == FUNCTION_EQ)
            return YELLOW;
        if (key == FUNCTION_CLEAR)
            return PINK;
        return (key & 1) ? BLUE : RED;
    }        
        

    int mapKey(int x, int y)
        // return -1 or 0..9 for a numeric key
        // or one of the function keys
    {
        if (y==5 && x==0)
            return FUNCTION_CLEAR;
        if (y && x==3) 
            return y+9;
        if (x>2)
            return -1;
        if (y==1)
            return x+1;
        if (y==2)
            return x+4;
        if (y==3)
            return x+7;
        if (y==4 && x==1)
            return 0;
        return -1;
    }    

    char opChar(int op)
    {
        return
            op == FUNCTION_ADD ? '+' :
            op == FUNCTION_SUB ? '-' :
            op == FUNCTION_MUL ? '*' :
            op == FUNCTION_DIV ? '/' :
            op == FUNCTION_EQ  ? '=' :
            op == FUNCTION_CLEAR ? 'C' :
            ' ';
    }
    
    void doOp(int do_fxn)
    {
        display(DEBUG_MODE,"start    do_fxn(%c) fxn=%c value=%d accum=%d num_started=%d",
            opChar(do_fxn),
            opChar(fxn),
            value,
            accum,
            num_started);
        
        if (do_fxn == FUNCTION_CLEAR)
        {
            value = 0;
            fxn = 0;
            accum = 0;
            num_started = false;
        }
        else
        {
            if (!fxn)
            {
                accum = value;
                value = 0;
            }
            else if (num_started)
            {
                if (fxn == FUNCTION_ADD)
                    accum += value;
                else if (fxn == FUNCTION_SUB)
                    accum -= value;
                else if (fxn == FUNCTION_MUL)
                    accum *= value;
                else if (fxn == FUNCTION_DIV && value)
                    accum /= value;
                    
                value = 0;
                num_started = 0;
            }
            
            fxn = do_fxn;
        }

        display(DEBUG_MODE,"end      do_fxn(%c) fxn=%c value=%d accum=%d num_started=%d",
            opChar(do_fxn),
            opChar(fxn),
            value,
            accum,
            num_started);
        
    }
    

    virtual bool handleButton(int x, int y)
    {
        if (!demoMode::handleButton(x,y))
        {
            int key = mapKey(x,y);
            if (key >= FUNCTION_ADD)
            {
                doOp(key);
            }
            else if (key >= 0)
            {
                if (!num_started && fxn == FUNCTION_EQ)
                {
                    fxn = 0;
                    accum = 0;
                }
                value = (10 * value) + key;
                num_started = true;
                display(DEBUG_MODE,"    key(%d) value=%d",key,value);
            }
        }
        return true;
    }
    

        
};






#endif

