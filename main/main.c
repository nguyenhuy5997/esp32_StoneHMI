/* UART Echo Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "stone_parser/stone.h"
#include "history/spiffs.h"
#include "DS3231/DS3231.h"
#define RELAY_1 32
#define RELAY_2 33
RingbufHandle_t Stone_CMD_buf_handle;
QueueHandle_t Timer_queue;
gptimer_handle_t gptimer = NULL;
uint8_t mode = 1;
uint32_t  record = 300;
uint32_t run_time = 0;
typedef struct {
    uint64_t event_count;
} example_queue_element_t;

static bool IRAM_ATTR timer_on_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_awoken = pdFALSE;
    QueueHandle_t queue = (QueueHandle_t)user_data;
    example_queue_element_t ele = {
        .event_count = edata->count_value
    };
    xQueueSendFromISR(queue, &ele, &high_task_awoken);
    return (high_task_awoken == pdTRUE);
}

static void stone_cmd(){
	recive_group* STONER_recv = NULL;
	size_t item_size = 0;
	while(1){
		STONER_recv = (recive_group*) xRingbufferReceiveFromISR(Stone_CMD_buf_handle, &item_size);
		if(item_size > 0 && STONER_recv!= NULL){
			if(strcmp((const char*)STONER_recv->widget, "FREQ1") == 0) {
				mode = (uint8_t)STONER_recv->float_value;
			}else if(strcmp((const char*)STONER_recv->widget, "P01") == 0) {
    			printf("Turn on relay\r\n");
    			gpio_set_level(RELAY_1, 0);
    			gpio_set_level(RELAY_2, 0);
				if(mode == 1) record = 5*60;
				else if(mode == 2) record = 10*60;
				else if(mode == 3) record = 15*60;
				else if(mode == 4) record = 20*60;
				else if(mode == 5) record = 30*60;
				else record = 5*60;
				run_time = 0;
				gptimer_start(gptimer);
    		} else if (strcmp((const char*)STONER_recv->widget, "P02") == 0) {
    			printf("Turn off relay\r\n");
    			gpio_set_level(RELAY_1, 1);
    			gpio_set_level(RELAY_2, 1);
    			gptimer_stop(gptimer);
    			save_history(mode, run_time);
    		} else if (strcmp((const char*)STONER_recv->widget, "B42") == 0 || strcmp((const char*)STONER_recv->widget, "B01") == 0) {
    			push_history();
    		}

			vRingbufferReturnItem(Stone_CMD_buf_handle, (void*) STONER_recv);
		}
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}
static void timer_handle(){
	example_queue_element_t ele;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1MHz, 1 tick=1us
    };
    gptimer_new_timer(&timer_config, &gptimer);
    gptimer_event_callbacks_t cbs = {
		.on_alarm = timer_on_alarm_cb,
    };
    gptimer_register_event_callbacks(gptimer, &cbs, Timer_queue);
    gptimer_enable(gptimer);
    gptimer_alarm_config_t alarm_config1 = {
		.reload_count = 0,
		.alarm_count = 1000000, // period = 1s
		.flags.auto_reload_on_alarm = true,
    };
    gptimer_set_alarm_action(gptimer, &alarm_config1);
    char cd_timmer_buf[50];
	while(1) {
		 if (xQueueReceive(Timer_queue, &ele, pdMS_TO_TICKS(2000))) {
			record--;
			run_time++;
			if(record == 0) {
    			printf("Turn off relay\r\n");
    			gpio_set_level(RELAY_1, 1);
    			gpio_set_level(RELAY_2, 1);
    			gptimer_stop(gptimer);
    			save_history(mode, run_time);
    			back_win();
			}
			sprintf(cd_timmer_buf, "%lu:%02lu", record/60, record%60);
			printf("Runtime = %lu, record time = %lu, timer_cd = %s\r\n", run_time, record, cd_timmer_buf);
			set_text("label","G02_copy1", cd_timmer_buf);
		 }else {

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
	Timer_queue = xQueueCreate(10, sizeof(example_queue_element_t));
    xTaskCreate(uart_task, "uart_task", ECHO_TASK_STACK_SIZE, NULL, 10, NULL);
	xTaskCreate(stone_cmd, "stone_cmd", ECHO_TASK_STACK_SIZE*8, NULL, 10, NULL);
	xTaskCreate(timer_handle, "timer", ECHO_TASK_STACK_SIZE*8, NULL, 10, NULL);
}
