#include <cstdio>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/uart.h"
#include <Arduino.h>

static const char *TAG = "main";

extern "C" void app_main(void)
{
    initArduino();

    uart_set_baudrate(UART_NUM_0, 115200);

    printf("Hello from ESP32!\n");

    int count = 0;
    while (1) {
        ESP_LOGI(TAG, "Loop count A: %d", count++);
        vTaskDelay(pdMS_TO_TICKS(1000));   // wait 1 second
    }
}
