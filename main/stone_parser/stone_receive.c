#include "stone.h"


unsigned char STONE_RX_BUF[RX_LEN];
unsigned short STONE_RX_CNT=0;
const unsigned char frame_head[3]={'S','T','<'};//Frame header flag data
const unsigned char frame_end[3]={'>','E','T'};//End of frame flag data
unsigned char receive_over_flage = 0;

void uart_task(void *arg)
{
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = ECHO_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;

    ESP_ERROR_CHECK(uart_driver_install(ECHO_UART_PORT_NUM, RX_LEN * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(ECHO_UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(ECHO_UART_PORT_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS));
    uint8_t *data = (uint8_t *) malloc(RX_LEN);
    while (1) {
        // Read data from the UART
        int len = uart_read_bytes(ECHO_UART_PORT_NUM, data, (RX_LEN - 1), 20 / portTICK_PERIOD_MS);
        if (len) {
        	data[len] = '\0';
        	for(int i = 0; i < len; i++){
//        		printf( "%02x ", data[i]);
        	}
//        	printf("\r\n");
            for(int i = 0; i < len; i++){
            	STONE_RX_BUF[STONE_RX_CNT++] = data[i];
            	if(data[i]==0x3e && data[i+1]==0x45 && data[i+2]==0x54)
            	{
            		STONE_RX_BUF[STONE_RX_CNT++] = data[i+1];
            		STONE_RX_BUF[STONE_RX_CNT++] = data[i+2];
            		STONE_RX_BUF[STONE_RX_CNT++] = data[i+3];
            		STONE_RX_BUF[STONE_RX_CNT++] = data[i+4];
            		i=i+4;
            		receive_over_flage = 1;
					for (int j = 0; j < STONE_RX_CNT; j++) {
//						printf("%02x ", STONE_RX_BUF[j]);
					}
//					printf("\r\n");
            		receive_parse();
            		STONE_RX_CNT = 0;
            	}
            }
            len = 0;
        }
    }
}

unsigned short illegal_date (){

    printf("illegal\r\n");
    receive_over_flage = 0;
    STONE_RX_CNT=0;
    STONE_RX_BUF[0]='\0';
    return 0;
}

unsigned short do_crc(unsigned char *ptr, int len)   //CRC16_MODBUS calculation
{
    unsigned int i;
    unsigned short crc = 0xFFFF;
    unsigned char *crc_idx = ptr+len-2;
    unsigned char CRC_BUF[2];
    len = len-2;

    while(len--)
    {
        crc ^= *ptr++;
        for (i = 0; i < 8; ++i)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc = (crc >> 1);
        }
    }
    CRC_BUF[0]=crc>>8;
    CRC_BUF[1]=crc&0xFF;
    return memcmp(crc_idx,CRC_BUF,2);
}
