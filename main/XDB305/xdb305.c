/*
 * xdb305.c
 *
 *  Created on: Nov 10, 2024
 *      Author: chutc
 */

#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "string.h"
#include "xdb305.h"
static const char *TAG = "i2c";

#define I2C_MASTER_SCL_IO           19      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           18      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0       /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          40000  /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0       /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0       /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       2000

#define SENSOR_ADDR                 0x7F        /*!< Slave address of the MPU9250 sensor */

/**
 * @brief Read a sequence of bytes from a MPU9250 sensor registers
 */
static esp_err_t sensor_register_read(uint8_t reg_addr, uint8_t *data, size_t len)
{
    return i2c_master_write_read_device(I2C_MASTER_NUM, SENSOR_ADDR, &reg_addr, 1, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

/**
 * @brief Write a byte to a MPU9250 sensor register
 */
static esp_err_t sensor_register_write_byte(uint8_t reg_addr, uint8_t data)
{
    int ret;
    uint8_t write_buf[2] = {reg_addr, data};
    ret = i2c_master_write_to_device(I2C_MASTER_NUM, SENSOR_ADDR, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    return ret;
}

/**
 * @brief i2c master initialization
 */
esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_DISABLE,
        .scl_pullup_en = GPIO_PULLUP_DISABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}
/**
 * @brief read temperature and pressure value
 */
esp_err_t read_sensor_one_shot(float *Cal_TData, float *Cal_PData){
	float FULLSCALE = 300; // Maximum pressure is 300Bar
	uint8_t REG_Data[5]; // Array to store register value
	uint8_t SCO = 0;
	esp_err_t ret;
	memset(REG_Data, 0, sizeof(REG_Data));
	/* Send command 0x0A to register 0x30 */
	ret = sensor_register_write_byte(0x30, 0x0A);
	if(ret != ESP_OK) return ret;
	/* Read the value of register 0x30 and wait till SCO bit (bit 3) is 0 */
	ret = sensor_register_read(0x30, &SCO, 1);
	if(ret != ESP_OK) return ret;
	while (((SCO >> 2) & 0x1) == 1){
		ret = sensor_register_read(0x30, &SCO, 1);
		if(ret != ESP_OK) return ret;
	}
	/* Read REG_0x06, REG_0x07, REG_0x08, REG_0x09, REG_0x0A and store in REG_Data array*/
	ret = sensor_register_read(0x06, REG_Data, 5);
	if(ret != ESP_OK) return ret;
	/* Calculate real temperature */
	*(Cal_TData) = REG_Data[3] * 256 + REG_Data[4];
	if (*(Cal_TData) > 32768)
		*(Cal_TData) = (*(Cal_TData) - 65536) / 256;
	else
		*(Cal_TData) = *(Cal_TData) / 256;
	/* Calculate real temperature */
	*(Cal_PData) = REG_Data[0] * 65536 + REG_Data[1] * 256 + REG_Data[2];
	if (*(Cal_PData) > 8388608)
		*(Cal_PData) = ((*(Cal_PData) - 16777216) / 8388608) * FULLSCALE;
	else
		*(Cal_PData) = (*(Cal_PData) / 8388608) * FULLSCALE;
	return ret;
}


