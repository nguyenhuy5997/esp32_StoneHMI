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
    return ret;
}

void save_history(){
	char buffer[30];
	struct timeval tv;
	time_t curtime;
	gettimeofday(&tv, NULL);
	curtime=tv.tv_sec;
    ESP_LOGI(TAG, "Opening file");
    gettimeofday(&tv, NULL);
    curtime=tv.tv_sec;
    strftime(buffer,30,"%m-%d-%Y %T,15,1,120",localtime(&curtime));
    insert_text_at_first_line("/spiffs/log.txt", buffer);
    ESP_LOGI(TAG, "File written");
}
void push_history(){
    ESP_LOGI(TAG, "Reading file");
    FILE* f = fopen("/spiffs/log.txt", "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }
    char line[64];
    char str1[30], str2[10], str3[10], str4[10];
    uint8_t line_count = 0;
    char line_count_str[2];
    if(f!=NULL){
    	while (fgets(line, sizeof(line), f)) {
			printf("%s", line);
			line_count++;
			if(line_count == 1) {
				itoa(line_count, line_count_str, 10);
				sscanf(line, "%[^,],%[^,],%[^,],%s", str1, str2, str3, str4);
				set_text("label","label3_copy1", line_count_str);
				set_text("label","label4_copy1_copy1", str1);
				set_text("label","label4_copy1", str2);
				set_text("label","label4_copy2_copy1", str3);
				set_text("label","label4_copy3_copy1", str4);
			} else if (line_count == 2) {
				itoa(line_count, line_count_str, 10);
				sscanf(line, "%[^,],%[^,],%[^,],%s", str1, str2, str3, str4);
				set_text("label","label3_copy1_copy1", line_count_str);
				set_text("label","label4_copy1_copy1_copy1", str1);
				set_text("label","label4_copy5_copy1", str2);
				set_text("label","label4_copy2_copy1_copy1", str3);
				set_text("label","label4_copy3_copy1_copy1", str4);
			} else if (line_count == 3) {
				itoa(line_count, line_count_str, 10);
				sscanf(line, "%[^,],%[^,],%[^,],%s", str1, str2, str3, str4);
				set_text("label","label3_copy2_copy1", line_count_str);
				set_text("label","label4_copy1_copy2_copy1", str1);
				set_text("label","label4_copy6_copy1", str2);
				set_text("label","label4_copy2_copy2_copy1", str3);
				set_text("label","label4_copy3_copy2_copy1", str4);
			} else if (line_count == 4) {
				itoa(line_count, line_count_str, 10);
				sscanf(line, "%[^,],%[^,],%[^,],%s", str1, str2, str3, str4);
				set_text("label","label3_copy3_copy1", line_count_str);
				set_text("label","label4_copy1_copy3_copy1", str1);
				set_text("label","label4_copy7_copy1", str2);
				set_text("label","label4_copy2_copy3_copy1", str3);
				set_text("label","label4_copy3_copy3_copy1", str4);
			} else if (line_count == 5) {
				itoa(line_count, line_count_str, 10);
				sscanf(line, "%[^,],%[^,],%[^,],%s", str1, str2, str3, str4);
				set_text("label","label3_copy4_copy1", line_count_str);
				set_text("label","label4_copy1_copy4_copy1", str1);
				set_text("label","label4_copy8_copy1", str2);
				set_text("label","label4_copy2_copy4_copy1", str3);
				set_text("label","label4_copy3_copy4_copy1", str4);
				break;
			}

		}
		fclose(f);
    }
}
void set_time_epoch(){
	struct timeval now;
	now.tv_sec=1679385737;
	now.tv_usec=0;
	struct timezone utc = {0,0};
    int retrc = settimeofday(&now, &utc);
    if(retrc == 0){
        printf("settimeofday() successful.\n");
    }
}
void insert_text_at_first_line(const char *filename, const char *new_text) {
    // Mở file để đọc
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Can not open file");
        return;
    }
    // Đọc toàn bộ nội dung file
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
    content[file_size] = '\0'; // Kết thúc chuỗi
    fclose(file);
    // Mở lại file để ghi
    file = fopen(filename, "w");
    if (file == NULL) {
        perror("Can not open file for writing");
        free(content);
        return;
    }
    // Ghi text mới vào đầu và nội dung cũ vào file
    fprintf(file, "%s\n%s", new_text, content);
    fclose(file);
    free(content);
}
