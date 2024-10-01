#ifndef _STONE_CONFIG_H__
#define _STONE_CONFIG_H__

#include "limits.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"
#include "freertos/FreeRTOS.h"
#include "freertos/ringbuf.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "DS3231/DS3231.h"
#define ECHO_TEST_TXD (CONFIG_EXAMPLE_UART_TXD)
#define ECHO_TEST_RXD (CONFIG_EXAMPLE_UART_RXD)
#define ECHO_TEST_RTS (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)

#define ECHO_UART_PORT_NUM      (CONFIG_EXAMPLE_UART_PORT_NUM)
#define ECHO_UART_BAUD_RATE     (CONFIG_EXAMPLE_UART_BAUD_RATE)
#define ECHO_TASK_STACK_SIZE    (CONFIG_EXAMPLE_TASK_STACK_SIZE)
//Print instructions to parse data for Arduino observation******************************************
#define Instruction_parsing        1    //The default open

//Print data, used for Arduino debugging, will occupy memory after enabled********************************
#define print_recive_ALL           0    //Off by default

#define print_recive_sys           1
#define print_recive_button        1
#define print_recive_switch        1
#define print_recive_check         1
#define print_recive_radio         1
#define print_recive_slider        1
#define print_recive_progress      1
#define print_recive_label         1
#define print_recive_edit          1
#define print_recive_selector      1
#define print_recive_image_value   1

#endif
