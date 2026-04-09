idf_component_register(SRCS "fan.c" "temp_sensor.c" "lcd_display.c" "keypad.c" "main.c"
                       PRIV_REQUIRES spi_flash
                       INCLUDE_DIRS ""
                       REQUIRES esp_driver_gpio
                       REQUIRES esp_driver_gptimer
                       REQUIRES esp_driver_uart
                       )
