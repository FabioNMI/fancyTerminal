#include <stdio.h>
#include <stdint.h>
#include "../../../src/fancyTerminal.h"

void init()
{
  clearTerminalScreen();
}

void exitTerminal()
{
  resetTerminalColors();
}

int main () 
{
  init();
  drawVerticalLine(1,10,5,'+');
  drawVerticalLine(2,10,7,'-');
  setCursorXY(1,11);
  exitTerminal();
}