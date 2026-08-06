# Temperature Controller with ESP32
A real-time temperature monitoring and control system built on the ESP33 using the ESP-IDF Framework.

## Overview
The system continuously samples temperature from a DHT11 sensor. FreeRTOS tasks, mutuexes, and inter-task notifications are used to manage the sensor and display concurrently. 
Whenever a temperature change is detected, a notification signal is sent to the LCD task, which updates the display in real time. 

A keypad allows the user to input a target temperature. An automated fan activates whenever the current temperature exceeds the user-defined target.

## Hardware
- ESP32
- DHT11 Temperature Sensor
- 1602A LCD
- 4x4 Keypad Matrix
- Fan Motor
- Custom PCB


![Schematic](https://i.imgur.com/hAuj73F.png)


![PCB](https://i.imgur.com/P3Ti2m1.png)





    

