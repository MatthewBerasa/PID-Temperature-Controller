
#include "dht.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "1602_driver.h"
#include <stdio.h>
#include <math.h>

#define DHT_SENSOR DHT_TYPE_DHT11
#define DHT_PIN 27

QueueHandle_t tempQueue;

void convertToFahrenheit(float* temperature);
void lcdDisplay(void* pvParameters);
void measureTemperature(void* pvParameters);

void app_main(){
    tempQueue = xQueueCreate(1, sizeof(float));
    xTaskCreate(lcdDisplay, "lcdDisplayTask", 2048, NULL, 1, NULL);
    xTaskCreate(measureTemperature, "measureTempature", 2048, NULL, 1, NULL);
}


void convertToFahrenheit(float* temperature){
    *temperature =  (*temperature * (9.0/5)) + 32;
}

void lcdDisplay(void* pvParameters){
    lcd_err_t result = LCD_OK;
    lcdInit();

    result = lcdClear();
    lcdAssert(result);

    float lastTemperature = 0.0f;
    float currTemperature = 0.0f;
    char textBuffer[18];

    while(1){
        if(xQueueReceive(tempQueue, &currTemperature, portMAX_DELAY)){
            if(fabs(lastTemperature - currTemperature) > .1){
                sprintf(textBuffer, "Temp: %.1f F", currTemperature);
                result = lcdClear();
                lcdAssert(result);
                result = lcdSetText(textBuffer, 0, 0);
                lcdAssert(result);
            }
            lastTemperature = currTemperature;
        }
    }
}


void measureTemperature(void* pvParameters){
    float humidity = 0.0f;
    float temperature = 0.0f;

    while(1){
        ESP_ERROR_CHECK(dht_read_float_data(DHT_SENSOR, DHT_PIN, &humidity, &temperature));
        convertToFahrenheit(&temperature);
        xQueueSend(tempQueue, &temperature, 0);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}







