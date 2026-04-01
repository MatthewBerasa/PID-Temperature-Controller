#pragma once
#include <stdbool.h>
#include "esp_attr.h"

void initializeKeypad();
void determineButton(void* pvParameters);
void IRAM_ATTR handleButtonPress(void* args);
bool validateInput(int inputLength, char buttonPressed);