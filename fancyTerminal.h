/*
    A simple library for handling ANSI terminal input/output on embedded systems 
    and console applications!
    Author: Fabio Pereira
    Date: January, 22nd, 2024
    Visit my Youtube channel: https://www.youtube.com/@non_maskable_interrupt
    And my blog: https://embeddedsystems.io/
*/

#ifndef __FANCYTERMINAL
#define __FANCYTERMINAL

typedef enum {
    CL_BLACK,
    CL_RED,
    CL_GREEN,
    CL_YELLOW,
    CL_BLUE,
    CL_MAGENTA,
    CL_CYAN,
    CL_WHITE,
    CL_DEFAULT
} termColor;

typedef enum {
    KEY_NONE,   // no key pressed
    KEY_STD,    // the value returned is a standard keyboard key
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_END,
    KEY_KEYPAD5,
    KEY_HOME,
    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4
} termKey;

typedef struct {
    termKey key;
    char stdKey;
} termInputResult;

void clearTerminalScreen(void);
void setCursorInvisible(void);
void setCursorVisible(void);
void setFGColor(termColor color);
void setBGColor(termColor color);
void resetTerminalColors(void);
void setCursorXY(uint8_t x, uint8_t y);
void printCharXY(uint8_t x, uint8_t y, char ch);
void drawHorizontalLine(int x, int y, int width, char ch);
void drawVerticalLine(int x, int y, int height, char ch);
void initTerminalInput(void);
void deInitTerminalInput(void);
termInputResult readTerminalInput(void);

#ifdef __gnu_linux__
// We are using variadic macro parameters
#define terminalOutput(...) printf(__VA_ARGS__)
#define terminalFlush() fflush(stdout)
#else
#define terminalOutput(...) Serial.print(__VA_ARGS__)
#define terminalFlush() 
#endif

#endif