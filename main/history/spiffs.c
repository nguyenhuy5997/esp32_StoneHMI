/*
 * spiffs.c
 *
 *  Created on: Sep 28, 2024
 *      Author: chutc
 */

#include "spiffs.h"
#include "stone_parser/stone.h"
#define TAG  "SPIFFS"

esp_err_t init_SPIFFS(){
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = true
    };

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ret;
    }

    ESP_LOGI(TAG, "Performing SPIFFS_check().");
    ret = esp_spiffs_check(conf.partition_label);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPIFFS_check() failed (%s)", esp_err_to_name(ret));
        return ret;
    } else {
        ESP_LOGI(TAG, "SPIFFS_check() successful");
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s). Formatting...", esp_err_to_name(ret));
        esp_spiffs_format(conf.partition_label);
        return ret;
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

#
    // Check consistency of reported partiton size info.
    if (used > total) {
        ESP_LOGW(TAG, "Number of used bytes cannot be larger than total. Performing SPIFFS_check().");
        ret = esp_spiffs_check(conf.partition_label);
        // Could be also used to mend broken files, to clean unreferenced pages, etc.
        // More info at https://github.com/pellepl/spiffs/wiki/FAQ#powerlosses-contd-when-should-i-run-spiffs_check
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "SPIFFS_check() failed (%s)", esp_err_to_name(ret));
            return ret;
        } else {
            ESP_LOGI(TAG, "SPIFFS_check() successful");
        }
    }
    //Check file is exist, if not create then
    FILE *file;

    // Kiểm tra xem file có tồn tại không
    file = fopen("/spiffs/log.txt", "r");
    if (file) {
        fclose(file);
    } else {
        // Nếu file không tồn tại, tạo file
        file = fopen("/spiffs/log.txt", "w");
        if (file) {
            fclose(file);
        } else {
            printf("Can not create file %s\n", "/spiffs/log.txt");
        }
    }
    return ret;
}

void save_history(uint8_t mode, uint16_t time ){
	char buffer_time[30];
	char buffer_machine_run[30];
	uint16_t period = 1;
	if(mode == 1) period = 5;
	else if(mode == 2) period = 10;
	else if(mode == 3) period = 15;
	else if(mode == 4) period = 20;
	else if(mode == 5) period = 30;
	else period = 1;
	struct timeval tv;
	time_t curtime;
    ESP_LOGI(TAG, "Opening file");
    gettimeofday(&tv, NULL);
    curtime=tv.tv_sec;
    strftime(buffer_time,30,"%m-%d-%Y %T,",localtime(&curtime));
    sprintf(buffer_machine_run, "%d,%d,%d", period, mode, time);
    strcat(buffer_time, buffer_machine_run);
    insert_text_at_first_line("/spiffs/log.txt", buffer_time);
    ESP_LOGI(TAG, "File written");
}
void push_history(){
#define INDEX     7
#define DATE      22
#define TIME_MODE 5
#define MODE	  4
#define TIME_RUN  7
#define LOG_MAX	  200
    ESP_LOGI(TAG, "Reading file");
    FILE* f = fopen("/spiffs/log.txt", "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }
    char line[30];
    char str1[20], str2[3], str3[2], str4[4];
    char string_label[18];
    char array_res_1[INDEX*LOG_MAX+3], array_res_2[DATE*LOG_MAX+3], array_res_3[TIME_MODE*LOG_MAX+3], array_res_4[MODE*LOG_MAX+3], array_res_5[TIME_RUN*LOG_MAX+3];
    uint32_t line_count = 0;
    if(f!=NULL){
    	memset(array_res_1, 0, sizeof(array_res_1));
    	memset(array_res_2, 0, sizeof(array_res_2));
    	memset(array_res_3, 0, sizeof(array_res_3));
    	memset(array_res_4, 0, sizeof(array_res_4));
    	memset(array_res_5, 0, sizeof(array_res_5));
    	sprintf(array_res_1+strlen(array_res_1), "[");
    	sprintf(array_res_2+strlen(array_res_2), "[");
    	sprintf(array_res_3+strlen(array_res_3), "[");
    	sprintf(array_res_4+strlen(array_res_4), "[");
    	sprintf(array_res_5+strlen(array_res_5), "[");
    	while (fgets(line, sizeof(line), f)) {
			printf("%s", line);
			sscanf(line, "%[^,],%[^,],%[^,],%s", str1, str2, str3, str4);
			sprintf(array_res_1+strlen(array_res_1), "\"%ld\",", ++line_count);
			sprintf(array_res_2+strlen(array_res_2), "\"%s\",", str1);
			sprintf(array_res_3+strlen(array_res_3), "\"%s\",", str2);
			sprintf(array_res_4+strlen(array_res_4), "\"%s\",", str3);
			sprintf(array_res_5+strlen(array_res_5), "\"%s\",", str4);
			if(line_count >= LOG_MAX) break;
		}
    	sprintf(array_res_1+strlen(array_res_1)-1, "]");
    	sprintf(array_res_2+strlen(array_res_2)-1, "]");
    	sprintf(array_res_3+strlen(array_res_3)-1, "]");
    	sprintf(array_res_4+strlen(array_res_4)-1, "]");
    	sprintf(array_res_5+strlen(array_res_5)-1, "]");
//    	printf("%s\r\n", array_res_1);
//    	printf("%s\r\n", array_res_2);
//    	printf("%s\r\n", array_res_3);
//    	printf("%s\r\n", array_res_4);
//    	printf("%s\r\n", array_res_5);
    	memset(string_label, 0, sizeof(string_label));
    	sprintf(string_label, "lable1_copy1_%d", LOG_MAX);
    	set_text("label", string_label, array_res_1, 1);
    	vTaskDelay(pdMS_TO_TICKS(50));

    	memset(string_label, 0, sizeof(string_label));
    	sprintf(string_label, "lable2_copy1_%d", LOG_MAX);
    	set_text("label", string_label, array_res_2, 1);
    	vTaskDelay(pdMS_TO_TICKS(50));

    	memset(string_label, 0, sizeof(string_label));
    	sprintf(string_label, "lable3_copy1_%d", LOG_MAX);
    	set_text("label", string_label, array_res_3, 1);
    	vTaskDelay(pdMS_TO_TICKS(50));

    	memset(string_label, 0, sizeof(string_label));
    	sprintf(string_label, "lable4_copy1_%d", LOG_MAX);
    	set_text("label", string_label, array_res_4, 1);
    	vTaskDelay(pdMS_TO_TICKS(50));

    	memset(string_label, 0, sizeof(string_label));
    	sprintf(string_label, "lable5_copy1_%d", LOG_MAX);
    	set_text("label", string_label, array_res_5, 1);

		fclose(f);
	    ESP_LOGI(TAG, "Closing file");
    }
}
esp_err_t time_parser(char * input_string, struct tm *timeStruct) {
	if (strptime((const char*)input_string, "%Y-%m-%d %H:%M:%S", timeStruct) != NULL) {
		// Output the contents of the tm structure for validation
		printf("Hour: %d\n", timeStruct->tm_hour);
		printf("Minute: %d\n", timeStruct->tm_min);
		printf("Second: %d\n", timeStruct->tm_sec);
		printf("Year: %d\n", timeStruct->tm_year + 1900);// tm_year is years since 1900
		printf("Month: %d\n", timeStruct->tm_mon + 1);// tm_mon is 0-based
		printf("Day: %d\n", timeStruct->tm_mday);
		return ESP_OK;
	} else {
		return ESP_FAIL;
		printf("Failed to parse date-time string.\n");
	}
	return ESP_OK;
}
void set_time_epoch_ds3221(){
	struct timeval now;
	struct tm time;
	i2c_dev_t dev;
	ESP_ERROR_CHECK(i2cdev_init());
	memset(&dev, 0, sizeof(i2c_dev_t));
	ESP_ERROR_CHECK(ds3231_init_desc(&dev, 0, 21, 22));
	if (ds3231_get_time(&dev, &time) != ESP_OK)
	{
		printf("Could not get time\n");
		return;
	}
	now.tv_sec=mktime(&time);
	now.tv_usec=0;
	struct timezone utc = {0,0};
    int retrc = settimeofday(&now, &utc);
    if(retrc == 0){
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now.tv_sec));
        printf("settimeofday() successful: %s\r\n", buffer);
    }
}
esp_err_t set_time_epoch_string(char *input_string){
	struct timeval now;
	struct tm time;
	esp_err_t res;
	res = time_parser(input_string, &time);
	if(res == ESP_FAIL) return ESP_FAIL;
	now.tv_sec=mktime(&time);
	now.tv_usec=0;
	struct timezone utc = {0,0};
    int retrc = settimeofday(&now, &utc);
    if(retrc == 0){
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now.tv_sec));
        printf("settimeofday() successful: %s\r\n", buffer);
        set_date("digit_clock1",buffer);
    } else return ESP_FAIL;
    return ESP_OK;
}
void insert_text_at_first_line(const char *filename, const char *new_text) {

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Can not open file");
        return;
    }
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *content = malloc(file_size + 1);
    if (content == NULL) {
        perror("No memory");
        fclose(file);
        return;
    }
    fread(content, 1, file_size, file);
    content[file_size] = '\0';
    fclose(file);
    file = fopen(filename, "w");
    if (file == NULL) {
        perror("Can not open file for writing");
        free(content);
        return;
    }
    fprintf(file, "%s\n%s", new_text, content);
    fclose(file);
    free(content);
}
