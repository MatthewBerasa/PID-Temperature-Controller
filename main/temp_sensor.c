#include "freertos/FreeRTOS.h"
#include "dht.h"
#include "esp_log.h"
#include "temp_sensor.h"
#include "lcd_display.h"
#include "fan.h"
#include <math.h>

#define DHT_SENSOR DHT_TYPE_DHT11
#define DHT_PIN GPIO_NUM_21


void convertCelsiusToFahrenheit(float* temperature){
    *temperature =  (*temperature * (9.0/5)) + 32;
}

/*
Take temperature and humidity sample every 2 seconds
*/
void measureTemperature(void* pvParameters){
    struct displayInfo* info = (struct displayInfo*)pvParameters;
    float humidity = 0.0f;
    float currTemperature = 0.0f;

    info->targetTemperature = 73.0f;
    info->temperature = 0.0f;
    while(1){
        ESP_ERROR_CHECK(dht_read_float_data(DHT_SENSOR, DHT_PIN, &humidity, &currTemperature));
        convertCelsiusToFahrenheit(&currTemperature);

        xSemaphoreTake(info->xMutex, portMAX_DELAY);
        //Toggle Fan
        if(currFanStatus == OFF && currTemperature > info->targetTemperature){
            xTaskNotifyGive(toggleFanHandler);
        }
        else if(currFanStatus == ON && currTemperature < info->targetTemperature){
            xTaskNotifyGive(toggleFanHandler);
        }

        //Update Screen
        if(info->mode == NORMAL_MODE && fabs(currTemperature - info->temperature) >= .1){
            info->temperature = currTemperature;
            xTaskNotifyGive(updateDisplayHandler);
        }   
        xSemaphoreGive(info->xMutex);
        

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
} 