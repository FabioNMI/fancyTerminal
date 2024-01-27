/*
  A simple Pong game demonstrating the fancy terminal library!
  Author: Fabio Pereira
  Date: January, 22nd, 2024
  Visit my Youtube channel: https://www.youtube.com/@non_maskable_interrupt
  And my blog: https://embeddedsystems.io/  
*/

#include "fancyTerminal.h"

const int8_t courtSizeX = 40;
const int8_t courtSizeY = 15;
const int8_t paddleSize = 2;
const uint32_t ballUpdateIntervalms = 150;
const uint8_t maxScore = 15;
const termColor courtColor = CL_BLUE;
const termColor netColor = CL_CYAN;
const termColor player1Color = CL_GREEN;
const termColor player2Color = CL_RED;
const termColor ballColor = CL_YELLOW;
const char ballChar = 'o';
const char player1Char = ']';
const char player2Char = '[';
const char horizontalCourtChar = '-';
const char verticalCourtChar = '|';
const char netChar = ':';

typedef struct {
    int8_t posX;
    int8_t posY;
    int8_t dirX;
    int8_t dirY;
} sBall;
typedef struct {
    int8_t paddlePos[2];
    sBall ball;
    uint8_t scorePlayer[2];
    uint32_t nextBallUpdatems;
    bool start;
    bool playing;
    bool endGame;    
} sPongGame;

void processInput(sPongGame * game) {
    int8_t oldPaddlePos[2];
    oldPaddlePos[0] = game->paddlePos[0];
    oldPaddlePos[1] = game->paddlePos[1];
    termInputResult key = readTerminalInput();
    switch (key.key) {
        case KEY_STD:
            switch (key.stdKey) {
            case 'q' : game->endGame = true; break;
            case 'w' : game->paddlePos[0]--; break;
            case 's' : game->paddlePos[0]++; break;
            case ' ' : game->start = true; game->playing = true; break;
            }
            break;
        case KEY_UP : game->paddlePos[1]--; break;
        case KEY_DOWN : game->paddlePos[1]++; break;
    }

    for (int x = 0; x <= 1; x++) {
        if (oldPaddlePos[x] != game->paddlePos[x]) {
            if (game->paddlePos[x] < paddleSize + 1) game->paddlePos[x] = paddleSize + 1;
            if (game->paddlePos[x] > courtSizeY-1) game->paddlePos[x] = courtSizeY-1;
        }
    }   
}

void terminalInit() {
    initTerminalInput();
    clearTerminalScreen();
    setCursorInvisible();
}

void exitTerminal(void) {
    setCursorXY(1,courtSizeY+1);
    deInitTerminalInput();
    resetTerminalColors();
    setCursorVisible();
}

void drawCourt() {
    setFGColor(courtColor);
    drawHorizontalLine(1,1,courtSizeX,horizontalCourtChar);
    for (int ty = 2; ty < courtSizeY; ty++) {
        printCharXY(1,ty,verticalCourtChar);
        printCharXY(courtSizeX,ty,verticalCourtChar);
    }
    drawHorizontalLine(1,courtSizeY,courtSizeX,horizontalCourtChar);
    setFGColor(netColor);
    for (int ty = 2; ty < courtSizeY; ty++) {
        printCharXY(courtSizeX >> 1,ty,netChar);
    }
}

void drawPaddle(int px, int py, char ch) {
    printCharXY(px,py,ch);
    printCharXY(px,py+1,ch);
}

void moveBallToCenter(sPongGame * game) {
    game->ball.posX = courtSizeX >> 1;
    game->ball.posY = courtSizeY >> 1;
}

void updateScores(sPongGame * game) {
    // print player score
    setCursorXY(courtSizeX>>2,1);
    setFGColor(CL_MAGENTA);
    Serial.print(game->scorePlayer[0]);
    setCursorXY((courtSizeX>>2)*3,1);
    Serial.print(game->scorePlayer[1]);
    setCursorXY((courtSizeX>>1)-7,(courtSizeY>>1)-1);
    if (game->scorePlayer[0] == maxScore) {
        terminalOutput("Player 1 wins!");
        game->endGame = true;
    } else if (game->scorePlayer[1] == maxScore) {
        terminalOutput("Player 2 wins!");
        game->endGame = true;
    }
}

void eraseOpeningText(bool all) {
    setFGColor(netColor);
    drawHorizontalLine((courtSizeX >> 1)-11, 5, 22, ' ');
    if (all) {
        drawHorizontalLine((courtSizeX >> 1)-15, 3, 31, ' ');
        
        printCharXY(courtSizeX >> 1,3,netChar);
    }
    // redraw the net
    printCharXY(courtSizeX >> 1,5,netChar);    
}

void blinkStart(sPongGame * game) {
    static bool previousStart;
    if (!game->start) {
        uint32_t currentTime = millis();
        if (currentTime >= game->nextBallUpdatems) {
            setCursorXY((courtSizeX >> 1)-15,3);
            setFGColor(CL_YELLOW);
            terminalOutput("Console Pong by Fabio Pereira!");
            if (!game->playing) {
                setCursorXY((courtSizeX >> 1)-11,5);
                setFGColor(CL_YELLOW);
                terminalOutput("Press Space to Start!");
            } else {
                eraseOpeningText(game->start);
            }
            // reuse playing variable for controlling blinking (while start is false)
            game->playing = !game->playing;
            // reuse ball update timer for blinking the text
            game->nextBallUpdatems = currentTime + 500;
        }        
    } else {
        // if we just started the game, make sure we erase the text
        if (!previousStart) {
            eraseOpeningText(game->start);
            // let's also put the ball at the center of the court!
            moveBallToCenter(game);
            updateScores(game);
        }
    }
    previousStart = game->start;
}

void drawPaddles(sPongGame * game) {
    static int previous0 = -1;
    static int previous1 = -1;
    if (game->paddlePos[0] != previous0) {
        // we only need to update the paddle if it moved
        // first we erase the previous paddle
        if (previous1 != -1) drawVerticalLine(2, previous0, paddleSize, ' ');
        // now we draw the new one
        setFGColor(CL_RED);
        drawVerticalLine(2, game->paddlePos[0], paddleSize, player1Char);  
    }
    if (game->paddlePos[1] != previous1) {
        // we only need to update the paddle if it moved
        // first we erase the previous paddle
        if (previous1 != -1) drawVerticalLine(courtSizeX-1, previous1, paddleSize, ' ');
        // now we draw the new one
        setFGColor(CL_GREEN);
        drawVerticalLine(courtSizeX-1, game->paddlePos[1], paddleSize, player2Char);
    }    
    previous0 = game->paddlePos[0];
    previous1 = game->paddlePos[1];
    // blink Press Space to Start until the players start
    blinkStart(game);
}

void checkBallCollision(sPongGame * game) {
    if (game->ball.posX == 2) {
        // check if the ball hit the paddle of player 1 or not
        if (game->ball.posY >= (game->paddlePos[0] - paddleSize) && game->ball.posY <= game->paddlePos[0]) {
            // we had a collision with the paddle, reverse X axis
            game->ball.dirX = -game->ball.dirX;
            game->ball.posX += game->ball.dirX;
            // randomize Y direction
            game->ball.dirY = (random() % 3) - 1;
        } else {
            // player missed the ball, player 2 scored
            game->scorePlayer[1]++;
            moveBallToCenter(game);
            updateScores(game);
            game->playing = false; 
        }
    } else if (game->ball.posX == courtSizeX-1) {
        // check if the ball hit the paddle or not
        if (game->ball.posY >= (game->paddlePos[1] - paddleSize) && game->ball.posY <= game->paddlePos[1]) {
            // we had a collision with the paddle, reverse X axis
            game->ball.dirX = -game->ball.dirX;
            game->ball.posX += game->ball.dirX;
            // randomize Y direction
            game->ball.dirY = (random() % 3) - 1;            
        } else {
            // player missed the ball, the other player scored
            game->scorePlayer[0]++;
            moveBallToCenter(game);
            updateScores(game);
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
    if (game->start && game->playing) {
        uint32_t currentTime = millis();
        if (currentTime >= game->nextBallUpdatems) {
            game->ball.posX += game->ball.dirX;
            game->ball.posY += game->ball.dirY;
            checkBallCollision(game);
            // update ball on screen
            if (previousBall.posX != game->ball.posX || previousBall.posY != game->ball.posY) {
                // The ball moved, compute new ball position
                // First erase previous ball position
                char ch = ' ';
                if (previousBall.posX == courtSizeX >> 1) {
                    // if the ball was on the net, redraw the net
                    setFGColor(netColor);
                    ch = netChar;
                }
                printCharXY(previousBall.posX, previousBall.posY, ch);
                // now draw the ball
                setFGColor(ballColor);
                printCharXY(game->ball.posX, game->ball.posY, ballChar);
            }
            game->nextBallUpdatems = currentTime + ballUpdateIntervalms;
            previousBall.posX = game->ball.posX;
            previousBall.posY = game->ball.posY;
        }
    } else {
        // Randomize ball direction
        do {
            game->ball.dirX = (random() % 3) - 1;
            game->ball.dirY = (random() % 3) - 1;
        } while (game->ball.dirX == 0);
    }
}

void setup() {
    Serial.begin(115200);
    terminalInit();
    drawCourt();
}

void loop () {
    sPongGame game;
    memset(&game, 0, sizeof(sPongGame));
    game.paddlePos[0] = ((courtSizeY - paddleSize) >> 1) + (paddleSize);
    game.paddlePos[1] = ((courtSizeY - paddleSize) >> 1) + (paddleSize);
    while (1) {
        drawPaddles(&game);
        processInput(&game);
        processBall(&game);
        if (game.endGame) break;
    }
    exitTerminal();
}
