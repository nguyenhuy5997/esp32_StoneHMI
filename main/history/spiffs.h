/*
 * spiffs.hh
 *
 *  Created on: Sep 28, 2024
 *      Author: chutc
 */

#ifndef MAIN_HISTORY_SPIFFS_H_
#define MAIN_HISTORY_SPIFFS_H_
#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "driver/timer.h"
#include "time.h"
#include <sys/time.h>
esp_err_t init_SPIFFS();
void insert_text_at_first_line(const char *filename, const char *new_text);
void set_time_epoch();
void push_history();
void save_history(uint8_t mode, uint16_t time);
#endif /* MAIN_HISTORY_SPIFFS_H_ */
