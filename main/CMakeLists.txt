idf_component_register(SRCS "main.c"
							"stone_parser/stone_receive_group.c"
							"stone_parser/stone_receive.c"
							"stone_parser/stone_transport.c"
							"history/spiffs.c"
							"DS3231/DS3231.c"
							"DS3231/i2cdev.c"
							"XDB305/xdb305.c"
                    INCLUDE_DIRS ".")
