/*
 * xdb305.h
 *
 *  Created on: Nov 10, 2024
 *      Author: chutc
 */

#ifndef MAIN_XDB305_XDB305_H_
#define MAIN_XDB305_XDB305_H_
#include "esp_log.h"


esp_err_t read_sensor_one_shot(float *Cal_TData, float *Cal_PData);
esp_err_t i2c_master_init(void);
#endif /* MAIN_XDB305_XDB305_H_ */
