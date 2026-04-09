#pragma once
#include "freertos/FreeRTOS.h"

enum fanStatus{
    OFF = 0,
    ON = 1,
};

extern enum fanStatus currFanStatus;
extern TaskHandle_t toggleFanHandler;


void initializeFan();
void toggleFan(void *pvParameters);

