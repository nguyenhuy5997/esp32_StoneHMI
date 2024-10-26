///* UART Echo Example
//
//   This example code is in the Public Domain (or CC0 licensed, at your option.)
//
//   Unless required by applicable law or agreed to in writing, this
//   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
//   CONDITIONS OF ANY KIND, either express or implied.
//*/
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
QueueHandle_t  Stone_CMD_buf_handle;
QueueHandle_t Timer_queue;
gptimer_handle_t gptimer = NULL;
uint8_t mode = 1;
uint32_t  record = 300;
uint32_t run_time = 0;
uint16_t remaining_time = 0;
uint32_t mode_to_time = 0;
//#define DS3221

typedef struct {
    uint64_t event_count;
} example_queue_element_t;
void check_status_afer_seset(){
    get_date("digit_clock1");
	vTaskDelay(pdMS_TO_TICKS(100));
	get_displayed_window();

}
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
	recive_group * STONER_recv = NULL;
	while(1){
		if( xQueueReceive(Stone_CMD_buf_handle, &STONER_recv, (TickType_t)100)){
//			printf("STONER_recv->widget: %s\r\n", STONER_recv->widget);
//			printf("@@@@ %d\r\n", strcmp((const char*)STONER_recv->widget, "W01"));
			if(strcmp((const char*)STONER_recv->widget, "FREQ1") == 0) {
				mode = (uint8_t)STONER_recv->float_value;
			}else if(strcmp((const char*)STONER_recv->widget, "P01") == 0) {
    			printf("Turn on relay\r\n");
    			gpio_set_level(RELAY_1, 0);
    			gpio_set_level(RELAY_2, 0);
				if(mode == 1) record = 5*60;
				else if(mode == 2) {
					mode_to_time = record = 10*60;
				}
				else if (mode == 3) {
					mode_to_time = record = 15*60;
				}
				else if(mode == 4) {
					mode_to_time = record = 20*60;
				}
				else if(mode == 5) {
					mode_to_time = record = 30*60;
				}
				else {
					mode_to_time = record = 5*60;
				}
				run_time = 0;
				gptimer_start(gptimer);
				char t[3];
				itoa(mode,t,10);
				set_text("label", "FREQ2", t);
    		} else if (strcmp((const char*)STONER_recv->widget, "P02") == 0) {
    			printf("Turn off relay\r\n");
    			gpio_set_level(RELAY_1, 1);
    			gpio_set_level(RELAY_2, 1);
    			gptimer_stop(gptimer);
    			save_history(mode, run_time);
    		} else if (strcmp((const char*)STONER_recv->widget, "B42") == 0 || strcmp((const char*)STONER_recv->widget, "B01") == 0) {
    			push_history();
    		} else if (strcmp((const char*)STONER_recv->widget, "user_time_edit") == 0) {
    			if(set_time_epoch_string((char*)STONER_recv->text) == ESP_OK){
    				set_text("label","label_time_validation", "Set time success");
    			} else set_text("label","label_time_validation", "Set time fail, check format");
    		} else if (strcmp((const char*)STONER_recv->widget, "digit_clock1") == 0) {
    			set_time_epoch_string((char*)STONER_recv->text);
    		} else if (strcmp((const char*)STONER_recv->widget, "W01") == 0 || strcmp((const char*)STONER_recv->widget, "W02") == 0) {
    			if(strcmp((const char*)STONER_recv->widget, "W01") == 0) {
    			    get_value("label", "FREQ1");
    			} else {
    				get_text("label", "label_timer_count_down");
    			}
    		} else if (strcmp((const char*)STONER_recv->widget, "label_timer_count_down") == 0) {
    			uint8_t minutes, second;
    			if (sscanf((const char*)STONER_recv->text, "%hhu:%hhu", &minutes, &second) == 2) {
    				record = (minutes * 60) + second;
    				printf("ran_time: %ld\r\n", record);
    				get_value("label", "FREQ2");
    			}
    		} else if (strcmp((const char*)STONER_recv->widget,"FREQ2") == 0) {
    			if((uint8_t)STONER_recv->float_value == 1){
    				run_time = 5*60 - record;
    				mode_to_time = 5*60;
    			} else if((uint8_t)STONER_recv->float_value == 2) {
    				run_time = 10*60 - record;
    				mode_to_time = 10*60;
    			}else if((uint8_t)STONER_recv->float_value == 3) {
    				run_time = 15*60 - record;
    				mode_to_time = 15*60;
    			}else if((uint8_t)STONER_recv->float_value == 4) {
    				run_time = 20*60 - record;
    				mode_to_time = 20*60;
    			} else if((uint8_t)STONER_recv->float_value == 5) {
    				run_time = 30*60 - record;
    				mode_to_time = 30*60;
    			} else {
    				run_time = 5*60 - record;
    				mode_to_time = 5*60;
    			}
    			gptimer_start(gptimer);
    		}
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
    			save_history(mode, mode_to_time);
    			back_win();
			}
			sprintf(cd_timmer_buf, "%lu:%02lu", record/60, record%60);
			printf("Runtime = %lu, record time = %lu, timer_cd = %s\r\n", run_time, record, cd_timmer_buf);
			set_text("label","label_timer_count_down", cd_timmer_buf);
		 }else {

		 }
		 vTaskDelay(pdMS_TO_TICKS(100));
	}
}
void app_main(void)
{
    gpio_reset_pin(RELAY_1);
    gpio_set_direction(RELAY_1, GPIO_MODE_OUTPUT);
    gpio_reset_pin(RELAY_2);
    gpio_set_direction(RELAY_2, GPIO_MODE_OUTPUT);
	init_SPIFFS();
#ifdef DS3221
	set_time_epoch_ds3221();
#endif
	Stone_CMD_buf_handle = xQueueCreate(5, sizeof(recive_group *));
	Timer_queue = xQueueCreate(10, sizeof(example_queue_element_t));
    xTaskCreate(uart_task, "uart_task", ECHO_TASK_STACK_SIZE, NULL, 10, NULL);
	xTaskCreate(stone_cmd, "stone_cmd", ECHO_TASK_STACK_SIZE*8, NULL, 10, NULL);
	xTaskCreate(timer_handle, "timer", ECHO_TASK_STACK_SIZE*8, NULL, 10, NULL);
	check_status_afer_seset();
}

