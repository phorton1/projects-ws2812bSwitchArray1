//------------------------------------------
// button and expression pedal routines
//------------------------------------------

#include "myDebug.h"

int debug_level = 0;
int warning_level = 0;

// string is "\033[%d;" with following
// ansi color codes fore back
//  
//     Black 	        30 	40 
//     Red 	            31 	41 
//     Green 	        32 	42 
//     Yellow 	        33 	43   brown
//     Blue 	        34 	44 
//     Magenta 	        35 	45 
//     Cyan 	        36 	46 
//     White 	        37 	47 
//     Bright Black 	90 	100
//     Bright Red 	    91 	101
//     Bright Green 	92 	102
//     Bright Yellow 	93 	103
//     Bright Blue 	    94 	104
//     Bright Magenta 	95 	105
//     Bright Cyan 	    96 	106
//     Bright White 	97  107

#define COLOR_CONST_NONE     0
#define COLOR_CONST_WARNING  1
#define COLOR_CONST_ERROR    2

#define DISPLAY_BUFFER_WARNING   10
    // will get a warning before we use a format that is
    // this close to the full buffer size.


#ifdef CORE_TEENSY
    #define DISPLAY_BUFFER_SIZE     255
    #define PLATFORM_COLOR_STRING   "\033[92m"      // bright green    
    #define WARNING_COLOR_STRING    "\033[93m"       // yellow   
    #define ERROR_COLOR_STRING      "\033[91m"       // red   
#else
    #define DISPLAY_BUFFER_SIZE     80
    #define PLATFORM_COLOR_STRING   "\033[96m"       // bright cyan
    #define WARNING_COLOR_STRING    "\033[33m"       // brown
    #define ERROR_COLOR_STRING      "\033[95m"       // magenta
#endif


#if WITH_DISPLAY || WITH_WARNINGS || WITH_ERRORS
    char display_buffer1[DISPLAY_BUFFER_SIZE];
    #if USE_PROGMEM
        char display_buffer2[DISPLAY_BUFFER_SIZE];
    #endif
#endif


#if WITH_INDENTS
    int proc_level = 0;
    void indent()
    {
        for (int i=0; i<proc_level; i++)
            dbgSerial.print("    ");
    }
#endif


#if WITH_DISPLAY
    void clearDisplay()
    {
        dbgSerial.print("\033[2J");
        dbgSerial.print("\033[3J");
    }

    void display_fxn(int level, const char *format, ...)
    {
        checkMem();
        
        if (level > debug_level)
            return;
        
        va_list var;
        va_start(var, format);
        
        #if USE_PROGMEM
            if (strlen_P(format) >= DISPLAY_BUFFER_SIZE)
            {
                dbgSerial.println(F("error - display progmem buffer overflow"));
                return;
            }
            strcpy_P(display_buffer2,format);
            vsprintf(display_buffer1,display_buffer2,var);
        #else
            if (strlen(format) >= DISPLAY_BUFFER_SIZE)
            {
                dbgSerial.println(F("error - display buffer overflow"));
                return;
            }
            vsprintf(display_buffer1,format,var);
        #endif

        dbgSerial.print(PLATFORM_COLOR_STRING);
        #if WITH_INDENTS
            indent();
        #endif
        dbgSerial.println(display_buffer1);
    }
#endif


#if WITH_WARNINGS
    void warning_fxn(int level, const char *format, ...)
    {
        checkMem();

        if (level > warning_level)
            return;
        
        va_list var;
        va_start(var, format);

        #if USE_PROGMEM
            if (strlen_P(format) >= DISPLAY_BUFFER_SIZE)
            {
                dbgSerial.println(F("error - warning progmem buffer overflow"));
                return;
            }
            strcpy_P(display_buffer2,format);
            vsprintf(display_buffer1,display_buffer2,var);
        #else
            if (strlen(format) >= DISPLAY_BUFFER_SIZE)
            {
                dbgSerial.println(F("error - warning buffer overflow"));
                return;
            }
            vsprintf(display_buffer1,format,var);
        #endif

        dbgSerial.print(WARNING_COLOR_STRING);
        #if WITH_INDENTS
            indent();
        #endif
        dbgSerial.print("WARNING - ");
        dbgSerial.println(display_buffer1);
    }
#endif



#if WITH_ERRORS
    void error_fxn(const char *format, ...)
    {
        checkMem();

        va_list var;
        va_start(var, format);
        
        #if USE_PROGMEM
            if (strlen_P(format) >= DISPLAY_BUFFER_SIZE)
            {
                dbgSerial.println(F("error - error progmem buffer overflow"));
                return;
            }
            strcpy_P(display_buffer2,format);
            vsprintf(display_buffer1,display_buffer2,var);
        #else
            if (strlen(format) >= DISPLAY_BUFFER_SIZE)
            {
                dbgSerial.println(F("error - error buffer overflow"));
                return;
            }
            vsprintf(display_buffer1,format,var);
        #endif

        dbgSerial.print(ERROR_COLOR_STRING);
        dbgSerial.print("ERROR - ");
        dbgSerial.println(display_buffer1);
    }
#endif



    // defined even if MY_DEBUG is turned off

uint8_t myButtonPressed(uint8_t pin, uint8_t *state)
    // available if !MY_DEBUG
{
    checkMem();
    
    if (digitalRead(pin))
    {
        if (*state)
        {
            *state = 0;
        }
    }
    else if (!*state)
    {
        *state = 1;
        return 1;
    }
    return 0;
}





#if OLD_STUFF

    //-------------------------
    // hex display routines
    //-------------------------
    // use 4 buffers so they can be
    // called from my display macros
    
    #define NUM_HEX_BUFS  4
    char hex_buf[NUM_HEX_BUFS][10];
    int next_hex_buf = 0;
    
    char *hex2(uint8_t v)
    {
        int bnum = next_hex_buf++;
        sprintf(hex_buf[bnum],"0x%02x",v);
        if (next_hex_buf>=NUM_HEX_BUFS) next_hex_buf=0;
        return hex_buf[bnum];
    }
    
    char *hex4(uint16_t v)
    {
        int bnum = next_hex_buf++;
        sprintf(hex_buf[bnum],"0x%04x",v);
        if (next_hex_buf>=NUM_HEX_BUFS) next_hex_buf=0;
        return hex_buf[bnum];
    }
    
    char *hex8(uint32_t v)
    {
        int bnum = next_hex_buf++;
        sprintf(hex_buf[bnum],"0x%08lx",v);
        if (next_hex_buf>=NUM_HEX_BUFS) next_hex_buf=0;
        return hex_buf[bnum];
    }
    
    
    //-------------------------
    // rest of API
    //-------------------------
    
    void init_my_debug()
    {
        dbgSerial.begin(MY_BAUD_RATE);
        checkMem();
    }
    
    void clearPutty()
    {
        // dbgSerial.print("\033[1;1]f");
        // dbgSerial.print("\033[1J");
        dbgSerial.print("\033[2J");
        dbgSerial.print("\033[3J");
        
    }
#endif


#if WITH_DISPLAY_BYTES

    #ifdef CORE_TEENSY
    
        // optimized version of display_bytes (speed vs memory)
        
        #define MAX_DISPLAY_BYTES    2048
        char disp_bytes_buf[MAX_DISPLAY_BYTES];

        char *indent_buf(char *obuf)
        {
            if (proc_level < 0)
            {
                dbgSerial.println("WARNING: MISSING (unbalanced) proc_entry!!");
                proc_level = 0;
            }
            int i = proc_level * 4;
            while (i--) { *obuf++ = ' '; }
            return obuf;
        }

        void display_bytes(int level, const char *label, uint8_t *buf, int len)
        {
            if (level > debug_level) return;
            display_bytes_ep(level,0,label,buf,len);
        }

                
        void display_bytes_ep(int level, uint8_t ep, const char *label, uint8_t *buf, int len)
        {
            if (level > debug_level) return;
            char *obuf = disp_bytes_buf;
            obuf = indent_buf(obuf);
            
            uint8_t space_len = 4;
            if (ep)
            {
                *obuf++ = '0' + ep;
                space_len = 5;
            }
            while (*label)
            {
                *obuf++ = *label++;
            }
            *obuf++ = ' ';
            
            if (!len)
            {
                strcpy(obuf," (0 bytes!!)");
                dbgSerial.println(disp_bytes_buf);
                return;
            }

            // dbgSerial.println("its not just the call to dbgSerial.println");
            // return;
            
            int bnum = 0;
            while (bnum < len && bnum < MAX_DISPLAY_BYTES-8)
            {
                if (bnum % 16 == 0)
                {
                    if (bnum)
                    {
                        *obuf++ = 10;   // "\r";
                        *obuf++ = 13;   // "\n";
                        obuf = indent_buf(obuf);
                        for (int i=0; i<space_len; i++)
                        {
                            *obuf++ = ' ';
                        }
                    }
                }
                
                #if 1
                    uint8_t byte = buf[bnum++];
                    uint8_t nibble = byte >> 4;
                    byte &= 0x0f;
                    
                    *obuf++ = nibble > 9 ? ('a' + nibble-10) : ('0' + nibble);
                    *obuf++ = byte > 9 ? ('a' + byte-10) : ('0' + byte);
                    *obuf++ = ' ';
                    
                #else
                    sprintf(obuf,"%02x ",buf[bnum++]);
                    obuf += 3;
                #endif
            }
            *obuf++ = 0;
            dbgSerial.println(disp_bytes_buf);
        }

    #else      // display_bytes() on arduino
    
        void display_bytes(int level, const char *label, uint8_t *buf, int len)
        {
            checkMem();    
            if (level > debug_level) return;
            if (!len)
            {
                indent();
                dbgSerial.print(label);
                dbgSerial.println(" (0 bytes!!)");
                return;
            }
            
            char tbuf[6];
            int bnum = 0;
            while (bnum < len)
            {
                if (bnum % 16 == 0)
                {
                    if (!bnum)
                    {
                        indent();
                        dbgSerial.print(label);
                        dbgSerial.print(" ");
                    }
                    else
                    {
                        dbgSerial.println();
                        indent();
                        dbgSerial.print("    ");
                    }
                }
                sprintf(tbuf,"%02x ",buf[bnum++]);
                dbgSerial.print(tbuf);
            }
            dbgSerial.println();
        }
        
    #endif      // arduino version of display_bytes
#endif // WITH_DISPLAY_BYTES


#if WITH_DISPLAY_BYTES_LONG

    void display_bytes_long(int level, uint16_t addr, uint8_t *buf, int len)
    {
        checkMem();    
        if (level > debug_level) return;
        if (!len)
        {
            indent();
            dbgSerial.println("0x000000 (0 bytes!!)");
            return;
        }
        
        char tbuf[8];
        char char_buf[17];
        memset(char_buf,0,17);
        
        int bnum = 0;
        while (bnum < len)
        {
            if (bnum % 16 == 0)
            {
                if (bnum)
                {
                    dbgSerial.print("    ");
                    dbgSerial.println(char_buf);
                    memset(char_buf,0,17);
                }
                indent();
                sprintf(tbuf,"0x%04x:",addr + bnum);
                dbgSerial.print(tbuf);
            }

            uint8_t c = buf[bnum];
            sprintf(tbuf,"%02x ",c);
            dbgSerial.print(tbuf);
            char_buf[bnum % 16] = (c >= 32) && (c < 128) ? ((char) c) : '.';
            bnum++;
        }
        if (bnum)
        {
            dbgSerial.print("    ");
            dbgSerial.println(char_buf);
        }
    }
    
#endif  // WITH_DISPLAY_BYTES_LONG


    
//------------------------------------------------------
// memory debugging
//------------------------------------------------------

#if USE_MEMORY_CHECK
    extern "C" {
    
        // code copied from Arduino Playground

        extern unsigned int __heap_start;
        extern void *__brkval;
        
        
        // The free list structure as maintained by the
        // avr-libc memory allocation routines.
        
        struct __freelist {
            size_t sz;
            struct __freelist *nx;
        };
        
        // The head of the free list structure
        
        extern struct __freelist *__flp;
        
        
        // Calculates the size of the free list 
        int freeListSize()
        {
            struct __freelist* current;
            int total = 0;
            for (current = __flp; current; current = current->nx)
            {
                total += 2;
                    // Add two bytes for the memory block's header  
                total += (int) current->sz;
            }
            return total;
        }
        
        int freeMemory()
        {
            int free_memory;
            if ((int)__brkval == 0)
            {
                free_memory = ((int)&free_memory) - ((int)&__heap_start);
            }
            else
            {
                free_memory = ((int)&free_memory) - ((int)__brkval);
                free_memory += freeListSize();
            }
            return free_memory;
        }
                
        // my code
        
        void checkMem()
        {
            int free_mem = freeMemory();
            if (free_mem < MEMORY_LIMIT_WARNING)
            {
                dbgSerial.print("MEMORY WARNING!! free=");
                dbgSerial.println(free_mem);
            }
        }
        
        void dbgMem()
        {
            dbgSerial.print("DEBUG MEMORY = ");
            dbgSerial.println(freeMemory());
        }
        
    }   // extern "C"
    
#endif // USE_MEMORY_CHECK
