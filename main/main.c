#include "temp_sensor.h"
#include "keypad.h"
#include "lcd_display.h"
#include "freertos/FreeRTOS.h"

struct displayInfo globalTemperatureInformation;

void app_main(){
    //Initialize Peripherals
    initializeLCDDisplay();
    initializeKeypad();

    globalTemperatureInformation.xMutex = xSemaphoreCreateMutex();


    xTaskCreatePinnedToCore(updateLCDDisplay, "updateLCDDisplay", 4096, &globalTemperatureInformation, 2, &updateDisplayHandler, 1);
    xTaskCreatePinnedToCore(measureTemperature, "measureTemperature", 4096, &globalTemperatureInformation, 1, NULL, 0);
    xTaskCreatePinnedToCore(determineButtonLocation, "determineButtonLocation", 4096, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(evaluateButtonPressed, "evaluateButtonPressed", 4096, &globalTemperatureInformation, 1, NULL, 1);
}










