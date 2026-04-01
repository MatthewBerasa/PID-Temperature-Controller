#include "temp_sensor.h"
#include "keypad.h"
#include "lcd_display.h"
#include "freertos/FreeRTOS.h"

struct displayInfo globalTemperatureInformation;

void app_main(){
    //Initialize Peripherals
    initializeLCDDisplay();
    initializeKeypad();
    
    xTaskCreatePinnedToCore(updateLCDDisplay, "updateLCDDisplay", 4096, &globalTemperatureInformation, 2, &updateDisplayHandler, 1);
    xTaskCreatePinnedToCore(measureTemperature, "measureTemperature", 4096, &globalTemperatureInformation, 1, NULL, 0);
    

}










