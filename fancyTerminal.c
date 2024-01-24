#include <stdio.h>
#include <stdint.h>
#ifdef __gnu_linux__
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#endif
#include "fancyTerminal.h"

#ifdef __gnu_linux__
struct termios original_term, new_term;
#endif

void clearTerminalScreen(void) {
    terminalOutput("\x1B[2J");
}

void setCursorInvisible(void) {
    terminalOutput("\x1B[?25l");
}

void setCursorVisible(void) {
    terminalOutput("\x1B[?25h");
}

void setFGColor(termColor color) {
    terminalOutput("\x1B[3%um",color);
}

void setBGColor(termColor color) {
    terminalOutput("\x1B[4%um",color);
}

void resetTerminalColors() {
    terminalOutput("\x1B[0m");
}

void setCursorXY(uint8_t x, uint8_t y) {
    terminalOutput("\x1B[%hu;%huH",y,x);
}

void printCharXY(uint8_t x, uint8_t y, char ch) {
    setCursorXY(x,y);
    terminalOutput("%c",ch);
    terminalFlush();
}

void drawHorizontalLine(int x, int y, int width, char ch) {
    for (int tx = x; tx < (x+width); tx++) printCharXY(tx,y,ch);
}

void drawVerticalLine(int x, int y, int height, char ch) {
    // The line moves towards the origin
    for (int ty = y; ty > (y-height); ty--) printCharXY(x,ty,ch);
}

void initTerminalInput(void) {
#ifdef __gnu_linux__
    // Set terminal to non-canonical mode
    tcgetattr(STDIN_FILENO, &original_term);
    new_term = original_term;
    new_term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_term);
    // Set file descriptor to non-blocking
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
#else

#endif    
}

void deInitTerminalInput(void) {
#ifdef __gnu_linux__
    // Restore the terminal to its previous configuration
    tcsetattr(STDIN_FILENO, TCSANOW, &original_term);
    // Exit the application
    exit(0);
#endif
}

termInputResult readTerminalInput(void) {
    termInputResult result;
    result.key = KEY_NONE;
#ifdef __gnu_linux__
    char str[4];
    ssize_t bytesRead = read(STDIN_FILENO, &str, 4);
    if (bytesRead >= 1) {
        if (str[0] == '\x1b') {
            if (str[1] == '[') {
                switch (str[2]) {
                case 'A' : result.key = KEY_UP; break;
                case 'B' : result.key = KEY_DOWN; break;
                case 'C' : result.key = KEY_RIGHT; break;
                case 'D' : result.key = KEY_LEFT; break;
                case 'F' : result.key = KEY_END; break;
                case 'G' : result.key = KEY_KEYPAD5; break;
                case 'H' : result.key = KEY_HOME; break;
                case '1' :
                    switch (str[3]) {
                    case 'P' : result.key = KEY_F1; break;
                    case 'Q' : result.key = KEY_F2; break;
                    case 'R' : result.key = KEY_F3; break;
                    case 'S' : result.key = KEY_F4; break;
                    }
                }
            }      
        } else {
            result.key = KEY_STD;
            result.stdKey = str[0];
        }
    }
#else
    
#endif
    return result;
}