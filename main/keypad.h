#pragma once
#include <stdbool.h>
#include "esp_attr.h"

void initializeKeypad();
void determineButtonLocation(void* pvParameters);
void IRAM_ATTR handleButtonPress(void* args);
bool validateInput(int inputLength, char buttonPressed);
void evaluateButtonPressed(void* pvParameters);