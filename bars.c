#include <stdio.h>
#include <stdint.h>
#include "fancyTerminal.h"

void init()
{
  clearTerminal();
}

void exitTerminal()
{
  resetColors();
}

int main () 
{
  init();
  drawVerticalLine(1,10,5,'+');
  drawVerticalLine(2,10,7,'-');
  setCursorXY(1,11);
  exitTerminal();
}