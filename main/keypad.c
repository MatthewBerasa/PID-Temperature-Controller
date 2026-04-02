#include "driver/gpio.h"
#include "keypad.h"
#include "freertos/FreeRTOS.h"
#include "lcd_display.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define COL_PIN_1 GPIO_NUM_32
#define COL_PIN_2 GPIO_NUM_33 
#define COL_PIN_3 GPIO_NUM_25 
#define COL_PIN_4 GPIO_NUM_26 
#define ROW_PIN_1 GPIO_NUM_36
#define ROW_PIN_2 GPIO_NUM_39
#define ROW_PIN_3 GPIO_NUM_34
#define ROW_PIN_4 GPIO_NUM_35
#define COL_SIZE 4
#define ROW_SIZE 4

struct buttonInformation{
    int RowLocation;
    int ColumnLocation; 
};

const int colPins[] = {COL_PIN_1, COL_PIN_2, COL_PIN_3, COL_PIN_4};
const int rowPins[] = {ROW_PIN_1, ROW_PIN_2, ROW_PIN_3, ROW_PIN_4};

const char keypadMatrix[4][4] = {{'1', '2', '3', 'A'},
                                 {'4', '5', '6', 'B'},
                                 {'7', '8', '9', 'C'},
                                 {'*', '0', '#', 'D'},};

QueueHandle_t pressedButtonRowQueue;
QueueHandle_t buttonInformationQueue;

void initializeKeypad(){
    pressedButtonRowQueue = xQueueCreate(3, sizeof(int));
    buttonInformationQueue = xQueueCreate(3, sizeof(struct buttonInformation));

    //Initialize Columns
    for(int i = 0; i < COL_SIZE; i++){
        gpio_reset_pin(colPins[i]);
        gpio_set_direction(colPins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(colPins[i], 0);
    }

    //Initialize Rows
    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1 | ESP_INTR_FLAG_IRAM);
    for(int i = 0; i < ROW_SIZE; i++){
        gpio_reset_pin(rowPins[i]);
        gpio_set_direction(rowPins[i], GPIO_MODE_INPUT);

        gpio_set_intr_type(rowPins[i], GPIO_INTR_NEGEDGE);
        gpio_isr_handler_add(rowPins[i], handleButtonPress, (void*) i);
    }
}

/*
Task that determines which exact button was pressed.
Unblocked by ISR sending row location
Figures out the exact button through columns
*/
void determineButtonLocation(void* pvParameters){
    struct buttonInformation button;
    while(1){
        if(xQueueReceive(pressedButtonRowQueue, &button.RowLocation, portMAX_DELAY)){
            for(int i = 0; i < COL_SIZE; i++){
                gpio_set_level(colPins[i], 1);
                if(gpio_get_level(rowPins[button.RowLocation]) == 1){
                    button.ColumnLocation = i;
                }
                gpio_set_level(colPins[i], 0);
            }
            xQueueSend(buttonInformationQueue, &button, 0);
            vTaskDelay(pdMS_TO_TICKS(250));
            gpio_intr_enable(rowPins[button.RowLocation]);
        }
    }
}

void evaluateButtonPressed(void* pvParameters){
    struct displayInfo* info = (struct displayInfo*)pvParameters;
    struct buttonInformation buttonLocation;
    while(1){
        if(xQueueReceive(buttonInformationQueue, &buttonLocation, portMAX_DELAY)){
            char buttonPressed = keypadMatrix[buttonLocation.RowLocation][buttonLocation.ColumnLocation];
            

            xSemaphoreTake(info->xMutex, portMAX_DELAY);
            // '#' Pressed - Enter Input Mode
            if(info->mode == NORMAL_MODE && buttonPressed == '#'){
                info->mode = INPUT_MODE;
                info->inputLength = 0;
                info->inputBuffer[0] = '\0';
                info->validInput = true;
                xTaskNotifyGive(updateDisplayHandler);
            }
            else if(info->mode == INPUT_MODE){
                if(!validateInput(info->inputLength, buttonPressed)){
                    info->validInput = false;
                }
                else{
                    switch(buttonPressed){  
                        case '*': { //Valid Decimal Pressed
                            info->inputBuffer[info->inputLength] = '.';
                            info->inputLength++;
                            info->inputBuffer[info->inputLength] = '\0';
                            break;
                        }
                        case 'A':{ //'A' - Enter Key
                            float targetValue = (float)atof(info->inputBuffer);
                            info->mode = NORMAL_MODE;
                                                
                            //Check for Valid Temperature Range
                            if(targetValue >= 32.0 && targetValue <= 100.0)
                                info->targetTemperature = targetValue;
                            else
                                info->validInput = false;
                            break;
                        }
                        case 'B': { //'B' - Backspace
                            if(info->inputLength != 0){
                                info->inputLength--;
                                info->inputBuffer[info->inputLength] = '\0';
                            }       
                            break;
                        }
                        case 'C': { // 'C' - Clear
                            info->mode = NORMAL_MODE;
                            break;
                        }
                        default: {  // Valid Digit Pressed
                            info->inputBuffer[info->inputLength] = buttonPressed;
                            info->inputLength++;
                            info->inputBuffer[info->inputLength] = '\0';
                        }
                    };
                }
                xTaskNotifyGive(updateDisplayHandler);
            }
        }
        xSemaphoreGive(info->xMutex);
    }
}

/*
Make Sure Input is correct format
%d%d.%d
Ex. 73.9
*/
bool validateInput(int inputLength, char buttonPressed){
    if(buttonPressed == 'B' || buttonPressed == 'C')
        return true;
    else if(inputLength >= 4 && buttonPressed != 'A' && buttonPressed != 'C') //Error 1 Exceed Length
        return false;
    else if(inputLength == 2 && buttonPressed != '*') //Error 2 Decimal Missing
        return false;
    else if(buttonPressed == 'D' || buttonPressed == '#') //Error 3 Invalid Character
        return false;
    else if((inputLength == 0 || inputLength == 1 || inputLength == 3) && (buttonPressed == '*')) //Error 4 Misplaced Decimal
        return false;
    else if(buttonPressed == 'A' && inputLength < 4)  // Error 5 Invalid Entry 
        return false;
    
    return true;
}


/*
ISR Handling 4x4 Keypad Button Press
Checks which row the button pressed belongs to then sends to Queue
*/
void IRAM_ATTR handleButtonPress(void* args){
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
    int rowPressed = (int)args;
    gpio_intr_disable(rowPins[rowPressed]);
    xQueueSendFromISR(pressedButtonRowQueue, &rowPressed, &pxHigherPriorityTaskWoken);
    
    if(pxHigherPriorityTaskWoken == pdTRUE){
        portYIELD_FROM_ISR();
    }
}