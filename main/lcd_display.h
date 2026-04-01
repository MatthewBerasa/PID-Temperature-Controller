#pragma once 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

enum displayMode{
    NORMAL_MODE = 0,
    INPUT_MODE = 1,
};

struct displayInfo{
    float temperature;
    float targetTemperature;
    enum displayMode mode;
    char inputBuffer[8];
    int inputLength;
    bool validInput;
    char temperatureString[18];
    char targetString[18];
};

extern TaskHandle_t updateDisplayHandler;

void updateLCDDisplay(void* pvParameters);
void initializeLCDDisplay();