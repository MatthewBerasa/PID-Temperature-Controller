#include "1602_driver.h"
#include "temp_sensor.h"
#include "lcd_display.h"
#include <math.h>
#include <stdbool.h>

TaskHandle_t updateDisplayHandler;

void initializeLCDDisplay(){
    lcd_err_t result = LCD_OK;
    lcdInit();

    result = lcdClear();
    lcdAssert(result);

   
}

void updateLCDDisplay(void* pvParameters){
    struct displayInfo* info = (struct displayInfo*)pvParameters;
    lcd_err_t res = LCD_OK;
    while(1){
        if(ulTaskNotifyTake(pdTRUE, portMAX_DELAY)){
            res = lcdClear();
            lcdAssert(res);
            sprintf(info->temperatureString, "Temp: %.1f F", info->temperature);
            lcdSetText(info->temperatureString, 0, 0);
            
            if(info->mode == NORMAL_MODE){
                sprintf(info->targetString, "Target: %.1f F", info->targetTemperature);
                lcdSetText(info->targetString, 0, 1);
            }
            else{
                if(info->inputLength == 0)
                    lcdSetText("Target: ", 0, 1);
                else{
                    sprintf(info->targetString, "Target: %s", info->inputBuffer);
                    lcdSetText(info->targetString, 0, 1);
                }    
            }
        }
    }
}
