
#include "demo_ui.h"
#include "demo_mode.h"
#include "myDebug.h"

#define DEBUG_DEMO   1
#define DEBUG_ANIM   1      // set to 0 to debug (compares to debug_level)

#define REFRESH_RATE      100      // frames per second
#define SCROLL_RATE         5      // frames per pixel


#define START_MODE   MODE_OFF


demoUI::demoUI() :
    rect_full(0,0,NUM_COLS-1,NUM_ROWS-1),
    rect_display(0,0,PAD_SX-2,NUM_ROWS-1),
    rect_keypad(PAD_SX, PAD_SY, PAD_EX, PAD_EY),
    rect_keypad_frame(PAD_SX-1, PAD_SY-1, PAD_EX+1, PAD_EY+1)
{
    refresh_time = 0;

    last_button = 0;
    cur_button = 0;
    cur_row = -1;
    cur_col = -1;
    
    animateText(0);
    
    modes[MODE_OFF] = new modeOff(this,MODE_OFF);
    modes[MODE_STARTING] = new modeStarting(this,MODE_STARTING);
    modes[MODE_COLORPICKER] = new modeColorPicker(this,MODE_COLORPICKER);
    modes[MODE_CALCINTRO] = new modeCalcIntro(this,MODE_CALCINTRO);
    modes[MODE_CALCULATOR] = new modeCalculator(this,MODE_CALCULATOR);
    modes[MODE_STOPPING] = new modeStopping(this,MODE_STOPPING);

    setMode(START_MODE);
}



void demoUI::init()
{
    ws2812SwitchArray::init();
    debounce = 0;
}



void demoUI::onLoop()
{
    if (refresh_time < (1000 / REFRESH_RATE))
        return;

    // debounce buttons to 100ms ..
    // determine if a button was pressed
    // and if it was within the keypad,
    // pass it to the mode for handling.
    
    if (debounce > 100)
    {
        int button = buttonPressed();
        if (button != last_button)
        {
            debounce = 0;
            last_button = button;
            display(DEBUG_DEMO,"button_pressed=%d",button);
            
            if (button)
            {
                cur_button = button;
                cur_col = (button-1) / NUM_ROWS;
                cur_row = (button-1) % NUM_ROWS;
                if (cur_col & 1) cur_row = (NUM_ROWS-1)-cur_row;
            }
            else    // handle the button on the key up
            {
                if (cur_col >= PAD_SX &&
                    cur_col <= PAD_EX &&
                    cur_row >= PAD_SY &&
                    cur_row <= PAD_EY)
                {
                   pMode->handleButton(cur_col-PAD_SX,cur_row-PAD_SY);
                }
                cur_button = 0;
                cur_row = -1;
                cur_col  = -1;
            }
        }
    }   

    // increment the modes frame counter
    
    pMode->onFrame();
    
    // draw the display background, keypad, and frame

    fillRect(rect_display,pMode->getDefaultBackgroundColor());
    drawRect(rect_keypad_frame,pMode->getKeypadFrameColor());
    for (int x=0; x<PAD_XDIM; x++)
    {
        for (int y=0; y<PAD_YDIM; y++)
        {
            setPixel(
                y + PAD_SY,
                x + PAD_SX,
               pMode->getKeypadColor(x,y));
        }
    }
    
    pMode->onDraw();
    
    //-------------------------------------------
    // general effects get written over anything
    // drawn directly in showState()
    //-------------------------------------------
    // (1) process text animation
    
    processAnimatedText();
    
    // (3) highlight the current button being pressed, if any
    // if it's a pad button, clip to the keyboard
    // otherwise, clip to the display area
    
    if (cur_row >= 0)
    {
        Rect button_area(cur_col-1,cur_row-1,cur_col+1,cur_row+1);
        Rect &clip = rect_keypad.contains(cur_col,cur_row) ?
            rect_keypad :
            rect_display;
            
        if (clip.contains(cur_col-1,cur_row-1))
            setPixel(cur_row-1,cur_col-1,RED);
        if (clip.contains(cur_col,cur_row-1))
            setPixel(cur_row-1,cur_col,RED);
        if (clip.contains(cur_col+1,cur_row-1))
            setPixel(cur_row-1,cur_col+1,RED);
            
        if (clip.contains(cur_col-1,cur_row))
            setPixel(cur_row,cur_col-1,RED);
        setPixel(cur_row,cur_col,0xFFFFFF);
        if (clip.contains(cur_col+1,cur_row))
            setPixel(cur_row,cur_col+1,RED);

        if (clip.contains(cur_col-1,cur_row+1))
            setPixel(cur_row+1,cur_col-1,RED);
        if (clip.contains(cur_col,cur_row+1))
            setPixel(cur_row+1,cur_col,RED);
        if (clip.contains(cur_col+1,cur_row+1))
            setPixel(cur_row+1,cur_col+1,RED);
    }
    
    // REFRESH - show the led array (which also sets
    // up for the button IRQ in ws2812SwitchArray

    showLeds();
    refresh_time = 0;

}   // onLoop()
    
    


//--------------------------------------------
// basic drawing methods
//--------------------------------------------

#define FONT_HEIGHT  8
#define FONT_WIDTH   5


void demoUI::drawRect(Rect &r, int  color)
{
    for (int x=r.sx; x<=r.ex; x++)
    {
        setPixel(r.sy,x,color);
        setPixel(r.ey,x,color);
    }
    for (int y=r.sy+1; y<=r.ey-1; y++)
    {
        setPixel(y,r.sx,color);
        setPixel(y,r.ex,color);
    }
}


void demoUI::fillRect(Rect &r, int  color)
{
    for (int y=r.sy; y<=r.ey; y++)
    {
        for (int x=r.sx; x<=r.ex; x++)
        {
            setPixel(y,x,color);
        }
    }
}



void demoUI::drawText(int x, int y, const char *string, int color,   bool small_font /* = false */, bool clip_display /* = true */)
{
    int len = strlen(string);
    for (int i=0; i<len; i++)
    {
        drawChar(x+i*(FONT_WIDTH + (small_font?0:1)),y,string[i],color,small_font,clip_display);
    }
}


void demoUI::drawChar(int x, int y, const char c, int color,  bool small_font /* = false */, bool clip_display /* = true */)
{
    extern unsigned char font5x8[256][8];
    extern unsigned char font5x5[256][8];
    Rect &clip = clip_display ? rect_display : rect_full;
    for	(int row=0; row<FONT_HEIGHT; row++)
    {
        unsigned char b = small_font ?
            font5x5[(unsigned char)c][row] :
            font5x8[(unsigned char)c][row];
        
        for (int col=0; col<FONT_WIDTH; col++)
        {
            // clip to display rectangle
            if (clip.contains(col+x,row+y) &&
                (b & (1 << col)))
            {
                setPixel(row+y,col+x,color);
            }
        }
    }
}


//-----------------------------------------------
// effect routines
//-----------------------------------------------



void demoUI::animateText(const char *text)
{
    anim_raw_text = text;
    anim_next_piece = text;
    if (anim_next_piece)
    {
        display(DEBUG_ANIM,"animateText(%s)",text);
        parseNextAnimPiece();
    }
}


void demoUI::parseNextAnimPiece()
{
    if (!*anim_next_piece)
    {
        anim_raw_text = 0;
        anim_next_piece = 0;
        return;
    }

    display(DEBUG_ANIM,"parseNextAnimPiece(%s)",anim_next_piece);
    
    // defaults for a step
    
    anim_fg_color = pMode->getDefaultForegroundColor();       // color of text (default = white)
    anim_bg_color = pMode->getDefaultBackgroundColor();       // color of background (default = global display color background)
    anim_steps = 1;                         // number of (animation) frames remaining for this parsed step
    anim_count = 0;                         // counter for "refresh" to "animation" frame conversion
    anim_last_word = anim_raw_text;         // pointer to last word for [ scrolling
    anim_scroll_pos = 0;                    // where to write the text
    anim_scroll_dir = 0;                    // in which direction, if any, to scroll
    
    memset(anim_text,0,MAX_ANIM_PHRASE+1);
    
    // parse the raw text
    
    int len = 0;
    while (len<MAX_ANIM_PHRASE  &&
           anim_next_piece[len] &&
           anim_next_piece[len] != '(')
    {
        anim_text[len] = anim_next_piece[len];
        len++;
    }
    if (anim_next_piece[len] != '(')
    {
        my_error("EXPECTED '(' at chr(%d) in piece(%s) in animation(%s)",len,anim_next_piece,anim_raw_text);
        anim_raw_text = 0;
        anim_next_piece = 0;
        return;
    }
    anim_text[len++] = 0;
    display(DEBUG_ANIM,"anim_text='%s'",anim_text);
    
    // parse the arguments
    
    int arg_num = 0;
    int num_steps = 0;
    bool scroll_stopper = false;
    bool is_dark = false;
    while (anim_next_piece[len] &&
           anim_next_piece[len] != ')')
    {
        char c = anim_next_piece[len];
        if (c == ',')
        {
            arg_num++;
            if (arg_num > 2)
            {
                my_error("Too many arguments ',' at chr(%d) in piece(%s) in animation(%s)",len,anim_next_piece,anim_raw_text);
                anim_raw_text = 0;
                anim_next_piece = 0;
                return;
            }
        }
        else if (arg_num < 2)
        {
            int color = arg_num ? BLACK : WHITE;
            if (c == 'd')
                is_dark = true;
            else
            {
                switch (c)
                {
                    case '-' : color = BLACK; break;
                    case 'r' : color = is_dark ? DARK_RED : RED; break;
                    case 'g' : color = is_dark ? DARK_GREEN : GREEN; break;
                    case 'b' : color = is_dark ? DARK_BLUE : BLUE; break;
                    case 'y' : color = YELLOW; break;
                    case 'o' : color = ORANGE; break;
                    case 'p' : color = PINK; break;
                    case 'w' : color = WHITE; break;
                    case 'R' : color = BRIGHT_RED; break;
                    case 'G' : color = BRIGHT_GREEN; break;
                    case 'B' : color = BRIGHT_BLUE; break;
                    case 'Y' : color = BRIGHT_YELLOW; break;
                    case 'O' : color = BRIGHT_ORANGE; break;
                    case 'P' : color = BRIGHT_PINK; break;
                    case 'W' : color = BRIGHT_WHITE; break;
                    default : 
                        my_error("unknown color character '%c' at chr(%d) in piece(%s) in animation(%s)",c,len,anim_next_piece,anim_raw_text);
                        anim_raw_text = 0;
                        anim_next_piece = 0;
                        return;
                }
                if (arg_num)
                {
                    anim_bg_color = color;
                    display(DEBUG_ANIM,"bg_color=%d",color);
                }
                else
                {
                    display(DEBUG_ANIM,"fg_color=%d",color);
                    anim_fg_color = color;
                }
            }
        }
        else if (arg_num == 2)
        {
            if (c >= '0' && c <= '9')
            {
                num_steps = 10 * num_steps + (c - '0');
                display(DEBUG_ANIM,"num_steps bumped to %d",num_steps);
            }
            else if (c=='<' || c=='[')
            {
                display(DEBUG_ANIM,"scroll_left - %c",c);
                anim_scroll_dir = -1;
                scroll_stopper = c == '[';
            }
            else if (c=='>' || c==']')
            {
                display(DEBUG_ANIM,"scroll_right - %c",c);
                anim_scroll_dir = 1;
                scroll_stopper = c == ']';
            }
            else
            {
                my_error("unknown command character '%c' at chr(%d) in piece(%s) in animation(%s)",c,len,anim_next_piece,anim_raw_text);
                anim_raw_text = 0;
                anim_next_piece = 0;
                return;
            }
        }
        len++;
    }
    if (anim_next_piece[len] != ')')
    {
        my_error("EXPECTED ')' at chr(%d) in piece(%s) in animation(%s)",len,anim_next_piece,anim_raw_text);
        anim_raw_text = 0;
        anim_next_piece = 0;
        return;
    }
    
    // based on the commands, if any, setup the actual duration
    // by default its the number of animation frames they specified
    
    anim_steps = num_steps;
    display(DEBUG_ANIM,"finished parsing next piece at len=%d",len);
    anim_next_piece = &anim_next_piece[len+1];
    
    if (anim_scroll_dir)
    {
        // length of text in pixels
        // is amount to scroll text so that it will end up at 0,0
        
        num_steps = strlen(anim_text) * (FONT_WIDTH+1);
        anim_steps = num_steps;

        // full scroll has to also move the whole screen
        
        if (!scroll_stopper)
        {
            anim_steps += rect_display.width();
        }
        
        // srolling to a stop in from right requires that also scroll up to the last word
        
        else if (anim_scroll_dir < 0)
        {
            int last_word_len = 0;
            int text_len = strlen(anim_text);
            int at = text_len - 1;
            while (at >= 0 && anim_text[at] != ' ')
            {
                at--;
                last_word_len++;
            }
            anim_steps += rect_display.width() - (last_word_len * (FONT_WIDTH+1));
        }
        
        // starting scroll position for full text off screen

        anim_scroll_pos = (anim_scroll_dir > 0) ?
            -num_steps-1 : rect_display.width();
        
        display(DEBUG_ANIM,"starting anim dir=%d  stopper=%d  steps=%d  scroll_pos=%d",anim_scroll_dir,scroll_stopper,anim_steps,anim_scroll_pos);
    }
}




void demoUI::processAnimatedText()
{
    if (!anim_raw_text)
        return;
    if (!anim_steps)
    {
        display(DEBUG_ANIM,"NEXT anim step",0);
        parseNextAnimPiece();
        if (!anim_raw_text)
        {
            display(DEBUG_ANIM,"ANIMATION FINISHED2",0);
            return;
        }
    }
    
    display(DEBUG_ANIM,"doing anim step(%d) text=%s  dir=%d scroll_pos=%d",anim_steps,anim_text,anim_scroll_dir,anim_scroll_pos);
    
    // set the background and draw the text
    
    fillRect(rect_display,anim_bg_color);
    drawText(anim_scroll_pos,0,anim_text,anim_fg_color);
    
    // increment the count
    // and if it's up to the SCROLL_RATE, decrement the number of remaining steps
    
    anim_count++;
    if (anim_count == SCROLL_RATE)
    {
        anim_count = 0;
        anim_steps--;
        anim_scroll_pos += anim_scroll_dir;
    }
}


//--------------------------------------------
// modes
//--------------------------------------------

void demoUI::setMode(int the_mode)
{
    display(0,"setMode(%d)",the_mode);
    pMode = modes[the_mode];
    pMode->onModeSelected();
}

