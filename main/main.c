/* GPIO Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_timer.h"

#define GPIO_INPUT_IO_0     10
#define GPIO_INPUT_PIN_SEL  (1ULL<<GPIO_INPUT_IO_0)

#define ESP_INTR_FLAG_DEFAULT 0
#define LED_NUM     100

uint32_t led_data[LED_NUM] = {0};
uint32_t trigger = 0;
uint16_t led_index = 0;
int64_t timestamp = 0;
uint8_t captured_flag = 0;
int dat = 0;

//static QueueHandle_t gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    //captured_flag = 1;
    dat = gpio_get_level(GPIO_INPUT_IO_0);
    led_data[led_index] = (led_data[led_index] << 1) + dat;
    trigger++;
}
/*
static void gpio_task_example(void* arg)
{
    while(1) 
    {
        if(captured_flag == 1) {
            dat = gpio_get_level(GPIO_INPUT_IO_0);
            led_data[led_index] = (led_data[led_index] << 1) + dat;
            captured_flag = 0;
            trigger++;
        }
    }
}
*/

void app_main(void)
{
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};

    //interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    //bit mask of the pins, use GPIO10 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);

    //create a queue to handle gpio event from isr
    //gpio_evt_queue = xQueueCreate(0, sizeof(uint32_t));

    //start gpio task
    //xTaskCreatePinnedToCore(gpio_task_example, "gpio_task_example", 2048, NULL, 0, NULL, 1);
    //xTaskCreatePinnedToCore(generate_ledc_task, "generate_ledc_task", 2048, NULL, 0, NULL, 1);


    printf("Minimum free heap size: %"PRIu32" bytes\n", esp_get_minimum_free_heap_size());

    while(1) 
    {
        led_index = trigger/24;
        if (led_index == LED_NUM){
            led_index = 0;
            trigger = 0;
            for (int i = 0; i < LED_NUM; i++){
                printf("LED%"PRIu16", val: 0x%"PRIX32"\n", i, led_data[i]);
                led_data[i] = 0;
            }
            
        }
    }
}
