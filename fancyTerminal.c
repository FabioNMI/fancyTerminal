#include <stdio.h>
#include <stdint.h>
#include "fancyTerminal.h"

void clearTerminal()
{
  OUTPUT("\x1B[2J");
  OUTPUT("\x1B[?25l");
}

void setFGColor(termColor color)
{
  OUTPUT("\x1B[3%um",color);
}

void setBGColor(termColor color)
{
  OUTPUT("\x1B[4%um",color);
}

void resetColors()
{
  OUTPUT("\x1B[0m");
}

void setCursorXY(uint8_t x, uint8_t y)
{
  OUTPUT("\x1B[%hu;%huH",y,x);
}

void printCharXY(uint8_t x, uint8_t y, char ch)
{
  setCursorXY(x,y);
  OUTPUT("%c",ch);
  FLUSH(stdout);
}

void drawHorizontalLine(int x, int y, int width, char ch)
{
  for (int tx = x; tx < (x+width); tx++) printCharXY(tx,y,ch);
}

void drawVerticalLine(int x, int y, int height, char ch)
{
  // The line moves towards the origin
  for (int ty = y; ty > (y-height); ty--) printCharXY(x,ty,ch);
}