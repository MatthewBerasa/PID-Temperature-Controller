#include "driver/gpio.h"
#include "fan.h"

#define FAN_PIN GPIO_NUM_27

TaskHandle_t toggleFanHandler;
enum fanStatus currFanStatus = OFF;

void initializeFan(){
    gpio_reset_pin(FAN_PIN);
    gpio_set_direction(FAN_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(FAN_PIN, 0);
}

void toggleFan(void *pvParameters){
    while(1){
        if(ulTaskNotifyTake(pdTRUE, portMAX_DELAY)){
            currFanStatus = (currFanStatus == OFF) ? ON : OFF;
            gpio_set_level(FAN_PIN, currFanStatus);
        }
    }    

}






