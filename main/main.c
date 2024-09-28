/* UART Echo Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "stone_parser/stone.h"
#include "history/spiffs.h"
#define RELAY_1 21
#define RELAY_2 22
RingbufHandle_t Stone_CMD_buf_handle;

static void stone_cmd(){
	recive_group* STONER_recv = NULL;
	size_t item_size = 0;
	while(1){
		STONER_recv = (recive_group*) xRingbufferReceiveFromISR(Stone_CMD_buf_handle, &item_size);
		if(item_size > 0 && STONER_recv!= NULL){
    		if(strcmp((const char*)STONER_recv->widget, "P01") == 0) {
    			printf("Turn on relay\r\n");
    			gpio_set_level(RELAY_1, 0);
    			gpio_set_level(RELAY_2, 0);
    			save_history();
    		} else if (strcmp((const char*)STONER_recv->widget, "P02") == 0) {
    			printf("Turn off relay\r\n");
    			gpio_set_level(RELAY_1, 1);
    			gpio_set_level(RELAY_2, 1);
    		} else if (strcmp((const char*)STONER_recv->widget, "SAV") == 0 || strcmp((const char*)STONER_recv->widget, "B01") == 0) {
    			push_history();
    		}

			vRingbufferReturnItem(Stone_CMD_buf_handle, (void*) STONER_recv);
		}
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

void app_main(void)
{
	init_SPIFFS();
	set_time_epoch();
    gpio_reset_pin(RELAY_1);
    gpio_set_direction(RELAY_1, GPIO_MODE_OUTPUT);
    gpio_reset_pin(RELAY_2);
    gpio_set_direction(RELAY_2, GPIO_MODE_OUTPUT);
	Stone_CMD_buf_handle = xRingbufferCreate(1024, RINGBUF_TYPE_NOSPLIT);
	xTaskCreate(stone_cmd, "stone_cmd", ECHO_TASK_STACK_SIZE*8, NULL, 10, NULL);
    xTaskCreate(uart_task, "uart_task", ECHO_TASK_STACK_SIZE, NULL, 10, NULL);
}
