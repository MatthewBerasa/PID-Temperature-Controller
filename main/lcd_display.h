#pragma once 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "1602_driver.h"


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
    SemaphoreHandle_t xMutex;
};

extern TaskHandle_t updateDisplayHandler;

void updateLCDDisplay(void* pvParameters);
void initializeLCDDisplay();