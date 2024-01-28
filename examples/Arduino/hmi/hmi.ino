/*
  An example of a simple human-machine interface on Arduino using the fancy terminal!
  Expects:
  - analog inputs connected to A0 and A1
  - Push-button connected to D6
  - LED connected to D4
  Tested on Seeed Studio's Grove Beginner Kit for Arduino
  Author: Fabio Pereira
  Date: January, 27nd, 2024
  Visit my Youtube channel: https://www.youtube.com/@non_maskable_interrupt
  And my blog: https://embeddedsystems.io/  
*/

#include "fancyTerminal.h"

const int8_t screenSizeX = 60;
const int8_t screenSizeY = 13;
const termColor frameColor = CL_WHITE;
const int screenUpdateIntervalms = 100;
const int analogSamplingIntervalms = 50;
const int analogChannels[] = {
    A0, A2
};
const int8_t numAnalogChannels = sizeof(analogChannels) / sizeof(int);
const int maxAnalogVoltage = 5000;  // Use 5000 for 5V boards and 3300 for 3.3V boards!
const int maxAnalogReading = 1023;  // Use 1023 for 10-bit ADC or 4095 for 12-bit ADC
const int keyPin = 6;
const int LEDPin = 4;

typedef struct {
    int current;
    int minimum;
    int maximum;
    int x;
    int ycurrent;
    int ymin;
    int ymax;
} svoltage;
svoltage voltages[numAnalogChannels];
int analogSamplingTimerms;
int screenUpdateTimerms;
bool LEDstate = false;

bool checkTimeout(int * timer, int timeout) {
    int now = millis();
    if ((now - *timer) >= timeout ) return true; else return false;
}

void resetAnalogReadings() {
    for (int x = 0; x < numAnalogChannels; x++) {
        voltages[x].minimum = voltages[x].current;
        voltages[x].maximum = voltages[x].current;
    }
}

void drawScreen() {
    // Main frame
    setFGColor(frameColor);
    drawRect(1,1,screenSizeX,screenSizeY,'*');
    // Title
    const char str[] = "Arduino HMI";
    // Centralize the title
    setCursorXY((screenSizeX-strlen(str))>>1,1);
    terminalOutput(str);
    // Analog frame
    drawRect(35,2,screenSizeX-2,10,'*');
    drawHorizontalLine(35,6,screenSizeX-2,'*');
    setCursorXY(44,2);
    terminalOutput("Analog");
    setCursorXY(37,4);
    terminalOutput("Channel A0");
    setCursorXY(37,8);
    terminalOutput("Channel A1"); 
    // Key frame   
    drawRect(3,2,15,6,'*');
    setCursorXY(7,3);
    terminalOutput("Key");    
    // LED frame
    drawRect(3,6,15,10,'*');
    setCursorXY(7,7);
    terminalOutput("LED");
    // Help text
    const char str2[] = "Press L for toggling LED, R for reset measurements";
    const char str3[] = ", X to redraw screen";
    setFGColor(CL_CYAN);
    setCursorXY((screenSizeX-strlen(str2))>>1,11);
    terminalOutput(str2);
    setCursorXY((screenSizeX-strlen(str3))>>1,12);
    terminalOutput(str3);
}

void processInput() {
    termInputResult key = readTerminalInput();
    switch (key.key) {
        case KEY_STD:
            switch (key.stdKey) {
                case 'l' : LEDstate = !LEDstate; break;
                case 'r' : resetAnalogReadings(); break;
                case 'x' : drawScreen(); break;
            }
            break;
    }  
}

void terminalInit() {
    initTerminalInput();
    clearTerminalScreen();
    setCursorInvisible();
}

void exitTerminal(void) {
    setCursorXY(1,screenSizeY+1);
    deInitTerminalInput();
    resetTerminalColors();
    setCursorVisible();
}

void doHardwareInterface() {
    if (checkTimeout(&analogSamplingTimerms, analogSamplingIntervalms)) {
        for (int x = 0; x < numAnalogChannels; x++) {
            int reading = analogRead(analogChannels[x]);
            // Let's use fixed point math and avoid floating point! 5000 = 5.00 Volts
            voltages[x].current = (((long)reading * maxAnalogVoltage) / maxAnalogReading) / 10;
            if (voltages[x].current < voltages[x].minimum) voltages[x].minimum = voltages[x].current;
            if (voltages[x].current > voltages[x].maximum) voltages[x].maximum = voltages[x].current;
        }
        analogSamplingTimerms = millis();
    }
    digitalWrite(LEDPin, LEDstate);
}

void printFractionalValue(int value) {
    int integer = value / 100;
    terminalOutput(integer);
    terminalOutput('.');
    int fractional = value - integer*100;
    if (fractional < 10 ) terminalOutput('0');
    terminalOutput(fractional);
}

void doScreenUpdate() {
    if (checkTimeout(&screenUpdateTimerms, screenUpdateIntervalms)) {
        // Analog data
        setFGColor(CL_RED);
        setCursorXY(50,3);
        printFractionalValue("  ");
        setCursorXY(50,3);
        printFractionalValue(voltages[0].maximum);
        setCursorXY(50,7);
        printFractionalValue("  ");
        setCursorXY(50,7);
        printFractionalValue(voltages[1].maximum);
        setFGColor(CL_BLUE);
        setCursorXY(50,4);
        printFractionalValue("  ");
        setCursorXY(50,4);
        printFractionalValue(voltages[0].current);       
        setCursorXY(50,8);
        printFractionalValue("  ");
        setCursorXY(50,8);
        printFractionalValue(voltages[1].current);
        setFGColor(CL_GREEN);
        setCursorXY(50,5);
        printFractionalValue("  ");
        setCursorXY(50,5);
        printFractionalValue(voltages[0].minimum);         
        setCursorXY(50,9);
        printFractionalValue("  ");
        setCursorXY(50,9);
        printFractionalValue(voltages[1].minimum);
        // Digital I/O
        setFGColor(CL_GREEN);
        setCursorXY(6,5);
        if (digitalRead(keyPin)) terminalOutput("HIGH"); else terminalOutput(" LOW");
        setCursorXY(7,9);
        if (LEDstate) terminalOutput(" ON"); else terminalOutput("OFF");
        screenUpdateTimerms = millis();
    }    
}

void setup() {
    Serial.begin(115200);
    pinMode(keyPin,INPUT_PULLUP);
    pinMode(LEDPin, OUTPUT);
    digitalWrite(LEDPin, LOW);
    terminalInit();
    drawScreen();
    analogSamplingTimerms = 0;
    screenUpdateTimerms = 0;
}

void loop () {
    while (1) {
        processInput();
        doHardwareInterface();
        doScreenUpdate();
    }
    exitTerminal();
}
