/*
 * esp_idf_lib_helpers.h
 *
 *  Created on: Sep 30, 2024
 *      Author: chutc
 */

#ifndef MAIN_DS3231_ESP_IDF_LIB_HELPERS_H_
#define MAIN_DS3231_ESP_IDF_LIB_HELPERS_H_

#include <freertos/FreeRTOS.h>
#include <esp_idf_version.h>

#if !defined(ESP_IDF_VERSION) || !defined(ESP_IDF_VERSION_VAL)
#error Unknown ESP-IDF/ESP8266 RTOS SDK version
#endif

/* Minimal supported version for ESP32, ESP32S2 */
#define HELPER_ESP32_MIN_VER    ESP_IDF_VERSION_VAL(3, 3, 5)
/* Minimal supported version for ESP8266 */
#define HELPER_ESP8266_MIN_VER  ESP_IDF_VERSION_VAL(3, 3, 0)

/* HELPER_TARGET_IS_ESP32
 * 1 when the target is esp32
 */
#if defined(CONFIG_IDF_TARGET_ESP32) \
        || defined(CONFIG_IDF_TARGET_ESP32S2) \
        || defined(CONFIG_IDF_TARGET_ESP32S3) \
        || defined(CONFIG_IDF_TARGET_ESP32C2) \
        || defined(CONFIG_IDF_TARGET_ESP32C3) \
        || defined(CONFIG_IDF_TARGET_ESP32C6) \
        || defined(CONFIG_IDF_TARGET_ESP32H2)
#define HELPER_TARGET_IS_ESP32     (1)
#define HELPER_TARGET_IS_ESP8266   (0)

/* HELPER_TARGET_IS_ESP8266
 * 1 when the target is esp8266
 */
#elif defined(CONFIG_IDF_TARGET_ESP8266)
#define HELPER_TARGET_IS_ESP32     (0)
#define HELPER_TARGET_IS_ESP8266   (1)
#else
#error BUG: cannot determine the target
#endif

#if HELPER_TARGET_IS_ESP32 && ESP_IDF_VERSION < HELPER_ESP32_MIN_VER
#error Unsupported ESP-IDF version. Please update!
#endif

#if HELPER_TARGET_IS_ESP8266 && ESP_IDF_VERSION < HELPER_ESP8266_MIN_VER
#error Unsupported ESP8266 RTOS SDK version. Please update!
#endif

/* show the actual values for debugging */
#if DEBUG
#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "="  VALUE(var)
#pragma message(VAR_NAME_VALUE(CONFIG_IDF_TARGET_ESP32C3))
#pragma message(VAR_NAME_VALUE(CONFIG_IDF_TARGET_ESP32H2))
#pragma message(VAR_NAME_VALUE(CONFIG_IDF_TARGET_ESP32S2))
#pragma message(VAR_NAME_VALUE(CONFIG_IDF_TARGET_ESP32))
#pragma message(VAR_NAME_VALUE(CONFIG_IDF_TARGET_ESP8266))
#pragma message(VAR_NAME_VALUE(ESP_IDF_VERSION_MAJOR))
#endif
#include <freertos/FreeRTOS.h>
#include <esp_idf_version.h>

#if !defined(ESP_IDF_VERSION) || !defined(ESP_IDF_VERSION_VAL)
#error Unknown ESP-IDF/ESP8266 RTOS SDK version
#endif

/* Minimal supported version for ESP32, ESP32S2 */
#define HELPER_ESP32_MIN_VER    ESP_IDF_VERSION_VAL(3, 3, 5)
/* Minimal supported version for ESP8266 */
#define HELPER_ESP8266_MIN_VER  ESP_IDF_VERSION_VAL(3, 3, 0)

/* HELPER_TARGET_IS_ESP32
 * 1 when the target is esp32
 */
#if defined(CONFIG_IDF_TARGET_ESP32) \
        || defined(CONFIG_IDF_TARGET_ESP32S2) \
        || defined(CONFIG_IDF_TARGET_ESP32S3) \
        || defined(CONFIG_IDF_TARGET_ESP32C2) \
        || defined(CONFIG_IDF_TARGET_ESP32C3) \
        || defined(CONFIG_IDF_TARGET_ESP32C6) \
        || defined(CONFIG_IDF_TARGET_ESP32H2)
#define HELPER_TARGET_IS_ESP32     (1)
#define HELPER_TARGET_IS_ESP8266   (0)

/* HELPER_TARGET_IS_ESP8266
 * 1 when the target is esp8266
 */
#elif defined(CONFIG_IDF_TARGET_ESP8266)
#define HELPER_TARGET_IS_ESP32     (0)
#define HELPER_TARGET_IS_ESP8266   (1)
#else
#error BUG: cannot determine the target
#endif

#if HELPER_TARGET_IS_ESP32 && ESP_IDF_VERSION < HELPER_ESP32_MIN_VER
#error Unsupported ESP-IDF version. Please update!
#endif

#if HELPER_TARGET_IS_ESP8266 && ESP_IDF_VERSION < HELPER_ESP8266_MIN_VER
#error Unsupported ESP8266 RTOS SDK version. Please update!
#endif

/* show the actual values for debugging */
#if DEBUG
#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "="  VALUE(var)
#pragma message(VAR_NAME_VALUE(CONFIG_IDF_TARGET_ESP32C3))
#pragma message(VAR_NAME_VALUE(CONFIG_IDF_TARGET_ESP32H2))
#pragma message(VAR_NAME_VALUE(CONFIG_IDF_TARGET_ESP32S2))
#pragma message(VAR_NAME_VALUE(CONFIG_IDF_TARGET_ESP32))
#pragma message(VAR_NAME_VALUE(CONFIG_IDF_TARGET_ESP8266))
#pragma message(VAR_NAME_VALUE(ESP_IDF_VERSION_MAJOR))
#endif



#endif /* MAIN_DS3231_ESP_IDF_LIB_HELPERS_H_ */
