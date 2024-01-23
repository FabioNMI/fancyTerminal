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

void clearTerminal();
void setFGColor(termColor color);
void setBGColor(termColor color);
void resetColors();
void setCursorXY(uint8_t x, uint8_t y);
void printCharXY(uint8_t x, uint8_t y, char ch);
void drawHorizontalLine(int x, int y, int width, char ch);
void drawVerticalLine(int x, int y, int height, char ch);

#ifdef __gnu_linux__
// We are using variadic macro parameters
#define OUTPUT(...) printf(__VA_ARGS__)
#define FLUSH(x) fflush(x)
#else
#define OUTPUT(...) Serial.print(__VA_ARGS__)
#define FLUSH(x) 
#endif

#endif