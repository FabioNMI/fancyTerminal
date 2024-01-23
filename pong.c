/*
  A simple Pong game demonstrating the fancy terminal library!
  Author: Fabio Pereira
  Author: Fabio Pereira
  Date: January, 22nd, 2024
  Visit my Youtube channel: https://www.youtube.com/@non_maskable_interrupt
  Or my blog: https://embeddedsystems.io/  
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "fancyTerminal.h"

const int8_t courtSizeX = 40;
const int8_t courtSizeY = 15;
const int8_t paddleSize = 2;
const uint32_t ballUpdateIntervalms = 150;

typedef struct
{
    int8_t posX;
    int8_t posY;
    int8_t dirX;
    int8_t dirY;
} sBall;
typedef struct 
{
    int8_t paddlePos[2];
    sBall ball;
    uint8_t scorePlayer[2];
    uint32_t nextBallUpdatems;
    bool start;
    bool playing;
    bool endGame;    
} sPongGame;

void processInput(sPongGame * game)
{
  int8_t oldPaddlePos[2];
  oldPaddlePos[0] = game->paddlePos[0];
  oldPaddlePos[1] = game->paddlePos[1];

  termInputResult key = readTerminalInput();
  switch (key.key) {
    case KEY_STD:
      switch (key.stdKey) {
        case 'q' : game->endGame = true; break;
        case 'w' : game->paddlePos[1]--; break;
        case 's' : game->paddlePos[1]++; break;
        case ' ' : game->start = true; game->playing = true; break;
      }
      break;
    case KEY_UP : game->paddlePos[0]--; break;
    case KEY_DOWN : game->paddlePos[0]++; break;
  }

  for (int x = 0; x <= 1; x++)
  {
    if (oldPaddlePos[x] != game->paddlePos[x])
    {
      if (game->paddlePos[x] < paddleSize + 1) game->paddlePos[x] = paddleSize + 1;
      if (game->paddlePos[x] > courtSizeY-1) game->paddlePos[x] = courtSizeY-1;
    }
  }   
}

void exitTerminal(void)
{
    resetTerminalColors();
    setCursorXY(1,courtSizeY+1);
    deInitTerminal();
}

// Platform independent code
 
void drawCourt()
{
    setFGColor(CL_BLUE);
    drawHorizontalLine(1,1,courtSizeX,'-');
    for (int ty = 2; ty < courtSizeY; ty++)
    {
        printCharXY(1,ty,'|');
        printCharXY(courtSizeX,ty,'|');
    }
    drawHorizontalLine(1,courtSizeY,courtSizeX,'-');
    setFGColor(CL_CYAN);
    for (int ty = 2; ty < courtSizeY; ty++)
    {
        printCharXY(courtSizeX >> 1,ty,':');
    }
}

void drawPaddle(int px, int py, char ch)
{
    printCharXY(px,py,ch);
    printCharXY(px,py+1,ch);
}

void moveBallToCenter(sPongGame * game)
{
    game->ball.posX = courtSizeX >> 1;
    game->ball.posY = courtSizeY >> 1;
}

void blinkStart(sPongGame * game)
{
    static bool previousStart;
    if (!game->start)
    {
        uint32_t currentTime = millis();
        if (currentTime >= game->nextBallUpdatems)
        {
            setCursorXY(10,3);
            setFGColor(CL_YELLOW);
            if (!game->playing) OUTPUT("Press Space to Start!"); else 
            {
                drawHorizontalLine(10,3,22,' ');
                setFGColor(CL_CYAN);
                printCharXY(courtSizeX >> 1,3,':');                
            }
            FLUSH(stdout);
            game->playing = !game->playing;
            game->nextBallUpdatems = currentTime + 500;
        }        
    } else
    {
        // if we just started the game, make sure we erase the text
        if (!previousStart)
        {
            drawHorizontalLine(10,3,22,' ');
            // redraw the net
            setFGColor(CL_CYAN);
            printCharXY(courtSizeX >> 1,3,':');
            // let's also put the ball at the center of the court!
            moveBallToCenter(game);
        }
    }
    previousStart = game->start;
}

void drawPaddles(sPongGame * game)
{
    static int previous0 = -1;
    static int previous1 = -1;
    if (game->paddlePos[0] != previous0)
    {
        // we only need to update the paddle if it moved
        // First we erase the previous paddle
        if (previous1 != -1) drawVerticalLine(courtSizeX-1, previous0, paddleSize, ' ');
        // Now we draw the new one
        setFGColor(CL_RED);
        drawVerticalLine(courtSizeX-1, game->paddlePos[0], paddleSize, '[');  
    }
    if (game->paddlePos[1] != previous1)
    {
        // we only need to update the paddle if it moved
        // First we erase the previous paddle
        if (previous1 != -1) drawVerticalLine(2, previous1, paddleSize, ' ');
        // Now we draw the new one
        setFGColor(CL_GREEN);
        drawVerticalLine(2, game->paddlePos[1], paddleSize, ']');
    }    
    previous0 = game->paddlePos[0];
    previous1 = game->paddlePos[1];
    // Blink Press Space to Start until the players start
    blinkStart(game);
}

void init()
{
    initTerminalInput();
    clearTerminalScreen();
    drawCourt();
}

void checkBallCollision(sPongGame * game)
{
    if (game->ball.posX == 2) {
        // check if the ball hit the paddle or not
        if (game->ball.posY >= (game->paddlePos[1] - paddleSize) && game->ball.posY <= game->paddlePos[1]) {
            // we had a collision with the paddle, reverse X axis
            game->ball.dirX = -game->ball.dirX;
            game->ball.posX += game->ball.dirX;
            // randomize Y direction
            game->ball.dirY = (random() % 3) - 1;
        } else {
            // player missed the ball, the other player scored
            game->scorePlayer[1]++;
            moveBallToCenter(game);
            game->playing = false; 
        }
    } else if (game->ball.posX == courtSizeX-1) {
        // check if the ball hit the paddle or not
        if (game->ball.posY >= (game->paddlePos[0] - paddleSize) && game->ball.posY <= game->paddlePos[0]) {
            // we had a collision with the paddle, reverse X axis
            game->ball.dirX = -game->ball.dirX;
            game->ball.posX += game->ball.dirX;
            // randomize Y direction
            game->ball.dirY = (random() % 3) - 1;            
        } else {
            // player missed the ball, the other player scored
            game->scorePlayer[0]++;
            moveBallToCenter(game);
            game->playing = false; 
        }               
    }
    if (game->ball.posY == 1 || game->ball.posY == courtSizeY) {
        // the ball hit a wall
        game->ball.dirY = -game->ball.dirY;
        game->ball.posY += game->ball.dirY;
    }
}

void processBall(sPongGame * game) {
    static sBall previousBall;
    if (game->start && game->playing)
    {
        uint32_t currentTime = millis();
        if (currentTime >= game->nextBallUpdatems)
        {
            game->ball.posX += game->ball.dirX;
            game->ball.posY += game->ball.dirY;
            checkBallCollision(game);
            // update ball on screen
            if (previousBall.posX != game->ball.posX || previousBall.posY != game->ball.posY)    
            {
                // The ball moved, compute new ball position
                // First erase previous ball position
                char ch = ' ';
                if (previousBall.posX == courtSizeX >> 1) 
                {
                    // if the ball was on the net, redraw the net
                    setFGColor(CL_CYAN);
                    ch = ':';
                }
                printCharXY(previousBall.posX, previousBall.posY, ch);
                // now draw the ball
                setFGColor(CL_YELLOW);
                printCharXY(game->ball.posX, game->ball.posY, 'o');
            }
            game->nextBallUpdatems = currentTime + ballUpdateIntervalms;
            previousBall.posX = game->ball.posX;
            previousBall.posY = game->ball.posY;
        }
    } else
    {
        // Randomize ball direction
        do
        {
            game->ball.dirX = (random() % 3) - 1;
            game->ball.dirY = (random() % 3) - 1;
        } while (game->ball.dirX == 0);
        //setCursorXY(2,2);
        //printf("%d  ",game->ball.dirX);
    }
}

int main () 
{
    sPongGame game;
    memset(&game, 0, sizeof(sPongGame));
    game.paddlePos[0] = ((courtSizeY - paddleSize) >> 1) + (paddleSize);
    game.paddlePos[1] = ((courtSizeY - paddleSize) >> 1) + (paddleSize);
    game.nextBallUpdatems = 0;
    init();
    while (1) 
    {
        drawPaddles(&game);
        processInput(&game);
        processBall(&game);
        if (game.endGame)
        {
            break;
        } 
    }
    exitTerminal();
}