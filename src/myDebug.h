#ifndef MYDEBUGH
#define MYDEBUGH

// Client initializes the serial port themselves

#include "Arduino.h"


#define WITH_INDENTS        1
#define WITH_DISPLAY        1
#define WITH_WARNINGS       1
#define WITH_ERRORS         1

#define USE_PROGMEM         1
#define USE_MEMORY_CHECK    1


#if WITH_DISPLAY
    // you can turn these on or off individually
    // but they default to following WITH_DISPLAY
    #define WITH_DISPLAY_BYTES       1
    #define WITH_DISPLAY_BYTES_EP    1
    #define WITH_DISPLAY_BYTES_LONG  1
#else
    #define WITH_DISPLAY_BYTES       0
    #define WITH_DISPLAY_BYTES_EP    0
    #define WITH_DISPLAY_BYTES_LONG  0
#endif
    
// #include <SoftwareSerial.h> and change below if you want
// Change below on Teensy to Serial1 if you want

#ifdef CORE_TEENSY
    #define dbgSerial     Serial
    #undef USE_MEMORY_CHECK
#else
    #define dbgSerial     Serial
#endif



//---------------------------------------------------
// turn major chunks of code of or on
//---------------------------------------------------

#if WITH_INDENTS
    #define proc_entry()            proc_level++
    #define proc_leave()            proc_level--
    extern int proc_level;
#else
    #define proc_entry()
    #define proc_leave()
#endif

#if WITH_DISPLAY
    #if USE_PROGMEM
        #define display(l,f,...)        display_fxn(l,PSTR(f),__VA_ARGS__)
    #else
        #define display(l,f,...)        display_fxn(l,f,__VA_ARGS__)
    #endif
    extern void display_fxn(int level, const char *format, ...);
    extern void clearDisplay();
#else
    #define display(l,f,...)
    #define clearDisplay()
#endif


#if WITH_WARNINGS
    #if USE_PROGMEM
        #define warning(l,f,...)        warning_fxn(l,PSTR(f),__VA_ARGS__)
    #else
        #define warning(l,f,...)        warning_fxn(l,f,__VA_ARGS__)
    #endif
    extern void warning_fxn(int level, const char *format, ...);
#else
    #define warning(l,f,...)
#endif


#if WITH_ERRORS
    #if USE_PROGMEM
        #define my_error(f,...)        error_fxn(PSTR(f),__VA_ARGS__)
    #else
        #define my_error(f,...)        error_fxn(f,__VA_ARGS__)
    #endif
    extern void error_fxn(const char *format, ...);
#else
    #define my_error(f,...)
#endif



#if WITH_DISPLAY_BYTES    
    extern void display_bytes(int level, const char *label, uint8_t *buf, int len);
    #ifdef CORE_TEENSY
        // display_bytes_ep() only available on teensy
        extern void display_bytes_ep(int level, uint8_t ep, const char *label, uint8_t *buf, int len);
    #endif
#else
    #ifdef CORE_TEENSY
        // and we enforce it by not defining a null method otherwise
        #define display_bytes_ep(l,a,b,x,z)
    #endif
    #define display_bytes(l,a,b,z)
#endif



    
#if WITH_DISPLAY_BYTES_LONG    
    extern void display_bytes_long(int level, uint16_t addr, uint8_t *buf, int len);
#else
    #define display_bytes_long(l,a,b,z)
#endif


//---------------------------------------------------
// these are always defined 
//---------------------------------------------------

extern int debug_level;
extern int warning_level;
extern uint8_t myButtonPressed(uint8_t pin, uint8_t *state);


//----------------------------------------
// memory checking
//----------------------------------------

#if USE_MEMORY_CHECK
    extern "C" {
        
        #define MEMORY_LIMIT_WARNING 256
    
        void checkMem();
            // call this at the top of a routine and if there is less than
            // MEMORY_LIMIT_WARNING of bytes of free memory, it will print
            // a warning to the dbgSerial port.  Note that this is called
            // in all debug routines, so merely calling display() in your
            // routine *should* roughly check the memory situation
        
        void dbgMem();
            // call this and it will print out the amount of free memory
            // to the dbgSerial port    
    }
#else
    #define checkMem()
    #define dbgMem()
#endif


#endif  // !MYDEBUGH