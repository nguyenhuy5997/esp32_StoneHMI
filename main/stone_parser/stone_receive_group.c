/* The protocol sent by serial screen currently adopts hexadecimal format,
 * which reduces the difficulty of parsing and processing burden of MCU of lower computer.
 * Multi-byte data adopts the transmission mode of high order before low order after
 * frame header +  CMD   +   LEN   +    DATA            +     terminator  +  CRC
 * ST<   0x1068    0x0004   0x01 0x02 0x03 0x04        >ET        CRC16
 */

#include "stone.h"

extern unsigned char STONE_RX_BUF[RX_LEN];
extern unsigned short STONE_RX_CNT;
extern const unsigned char frame_head[3];
extern QueueHandle_t  Stone_CMD_buf_handle;
int widget_last_len;
int widget_len;
recive_group STONER;

const char* m_static_str[] = {"button;","user_button;","switch;",
															 "check_button;",
															 "radio_button;",
															 "radio_button_mcu_get;",
															 "slider;",
															 "slider_over;",
															 "progress_bar;",
															 "label_text;",
															 "edit_text;",
																";percent:",
																"edit_int;",
																";int_value:",
																"selector_value;",
																";value:",
																"selector_num;",
																";num:",
																"spin_box_int;",
																"selector_text;",
																"spin_box_text;",
																"label_value;",
																"image_value;",
																"spin_box_float;",
																"combo_box_text;",
																"combo_box_int;",
																"combo_box_float;",
																"combo_box_num;",
																"mledit_text;",
																"chart_view;",
																"capacity;",
																"progress_circle_value;",
																"progress_circle_percent;",
																	"digit_clock;",
																	"hscroll_label;"};

/* 	Text type data memory allocation function, dynamically created, not automatically freed  */
/*	The stone_recive_free() interface can be called to free memory when the user is finished using it
 *	If the user does not release the memory, the memory space occupied last time will be released before the next use
 */
 /* Create memory space for "data" Pointers */
recive_group datainit(int len){
   if (STONER.data != NULL){
		 memset(STONER.widget, '\0', widget_last_len);		//If it is not empty, release it first
    free(STONER.data);
    STONER.data = NULL;
   }
  STONER.data = (unsigned char*)malloc(len*sizeof(char)+1);
	 widget_last_len = len;
  if (!STONER.data){
    #if print_recive_ALL
      printf("Memory request error\r\n");
    #endif
  }
  memset(STONER.data, '\0', len);
  return STONER;
}

 /* Create memory space for "widget" Pointers */
recive_group widgetinit(int len){
   if (STONER.widget != NULL){
		memset(STONER.widget, '\0', widget_last_len);
    free(STONER.widget);
    STONER.widget = NULL;
   }
  STONER.widget = (unsigned char*)malloc(len*sizeof(char)+1);
	widget_last_len = len;
  if (!STONER.widget){
    #if print_recive_ALL
	  printf("Memory request error\r\n");
    #endif
  }
  memset(STONER.widget, '\0', len);
  return STONER;
}

 /* Create memory space for "text" Pointers */
recive_group textinit(int len){
   if (STONER.text != NULL){
		 memset(STONER.text, '\0', widget_last_len);
    free(STONER.text);
    STONER.text = NULL;
   }
  STONER.text = (unsigned char*)malloc(len*sizeof(char)+1);
	 widget_last_len = len;
  if (!STONER.text){
    #if print_recive_ALL
	  printf("Memory request error\r\n");
    #endif
  }
  memset(STONER.text, '\0', len);
  return STONER;
}

/* Manually release memory space */
// sample1ï¼šstone_recive_free("data"); //Specifies to free memory for STONE.data
// sample2ï¼šstone_recive_free();       //Free up all created memory
void _stone_recive_free(char* name){
  if (name != NULL){
		if (strcmp(name,"data")==0)
		{
			memset(STONER.data, '\0', widget_last_len);
			free(STONER.data);
			STONER.data = NULL;
		}
		else if (strcmp(name,"widget")==0)
		{
			memset(STONER.widget, '\0', widget_last_len);
			free(STONER.widget);
			STONER.widget = NULL;
		}
		else if (strcmp(name,"text")==0)
		{
			memset(STONER.text, '\0', widget_last_len);
			free(STONER.text);
			STONER.text = NULL;
		}
  }
  else{
    if (STONER.data != NULL){
		memset(STONER.data, '\0', widget_last_len);
    free(STONER.data);
    STONER.data = NULL;
   }
  if (STONER.widget != NULL){
		memset(STONER.widget, '\0', widget_last_len);
    free(STONER.widget);
    STONER.widget = NULL;
   }
  if (STONER.text != NULL){
		memset(STONER.text, '\0', widget_last_len);
    free(STONER.text);
    STONER.text = NULL;
   }
  }
}

/* Parsing functions that receive data */
void receive_parse (){

	/* Caches directives represented by CMD */
  STONER.cmd = (unsigned int)STONE_RX_BUF[3];
  STONER.cmd = STONER.cmd << 8 | STONE_RX_BUF[4];

	/* Cache the length LEN represents */
  STONER.len = (unsigned int)STONE_RX_BUF[5];
  STONER.len = STONER.len << 8 | STONE_RX_BUF[6];

//  unsigned int data_i2 = 0;
//  int temp_value = 0;

  switch (STONER.cmd){
    case sys_state: {
      STONER = datainit(STONER.len+1);     //Memory is allocated one byte too much
      sscanf((const char*)STONE_RX_BUF+7,"%[^>ET]",STONER.data);

      #if print_recive_sys || print_recive_ALL
      printf("type:sys_start;");
      printf("len:");
      printf("%d",STONER.len);
      printf(";value:");
      printf("%2x",STONER.data[0]);
      printf(";\r\n");
      #endif
    }break;
    case sys_hello: {
      STONER = datainit(STONER.len+1);
      sscanf((const char*)STONE_RX_BUF+7,"%[^>ET]",STONER.data);

      #if print_recive_sys || print_recive_ALL
      printf("type:sys_hello;");
      printf("len:");
      printf("%d",STONER.len);
      printf(";value:");
      printf("%2x",STONER.data[0]);
      printf(";\r\n");
      #endif
    }break;
    case sys_version: {
      STONER = datainit(STONER.len+1);
			memcpy(STONER.data,STONE_RX_BUF+7,STONER.len);

      #if print_recive_sys || print_recive_ALL
			printf("type:sys_version;");
			printf("len:");
			printf("%d",STONER.len);
			printf(";value:");
			for (int wds=0;wds<STONER.len;wds++)
			printf("%c",STONER.data[wds]);
			printf(";\r\n");
      #endif
    }break;
    case control_button: {
			case_btn_switch_ckbtn_rdbtn(m_button);
    }break;
    case control_button_u: {
			case_btn_switch_ckbtn_rdbtn(m_user_button);
    }break;
    case control_switch: {
			case_btn_switch_ckbtn_rdbtn(m_switch);
    }break;
    case control_check_button: {
			case_btn_switch_ckbtn_rdbtn(m_check_button);
    }break;
    case control_radio_button: {
			case_btn_switch_ckbtn_rdbtn(m_radio_button);
    }break;
    case control_radio_button_s: {
			case_btn_switch_ckbtn_rdbtn(m_radio_button_mcu_get);

    }break;
    case control_slider: {
			case_slider_porgressbar(m_slider);
    }break;
    case control_slider_over: {
			case_slider_porgressbar(m_slider_over);
    }break;
    case control_progress_bar: {
			case_slider_porgressbar(m_progress_bar);
    }break;
    case control_progress_bar_p: {
      case_4_byte_data(m_progress_bar,m_percent);
    }break;
    case control_label_text: {
			case_data_text(m_label_text);
    }break;
    case control_label_value: {
			case_4_byte_ieee_value(m_label_value);
    }break;
    case control_edit_text: {
			case_data_text(m_edit_text);
    }break;
    case control_edit_int: {
      case_4_byte_data(m_edit_int,m_int_value);
    }break;
    case control_text_selector_text: {
      case_data_text(m_selector_text);
    }break;
    case control_text_selector_value: {
      case_4_byte_data(m_selector_value,m_value);
    }break;
    case control_text_selector_num: {
      case_4_byte_data(m_selector_num,m_num);
    }break;

		case control_image: {
      STONER = widgetinit(STONER.len);
			memcpy(STONER.widget,STONE_RX_BUF+7,STONER.len-2);
      STONER.value = STONE_RX_BUF[7+STONER.len-2];
			STONER.value = STONER.value<<8 | STONE_RX_BUF[7+STONER.len-1];
      #if print_recive_image_value || print_recive_ALL
      printf("type:image;");
      printf("len:");
      printf("%d",STONER.len);
      printf(";widget:");
			for (int wds=0;wds<STONER.len-2;wds++)
			printf("%c", STONER.widget[wds]);
			printf(";value:");
			printf("%d",STONER.value);
			printf(";\r\n");
      #endif
    }break;

		case control_image_u: {
      STONER = widgetinit(STONER.len);
			memcpy(STONER.widget,STONE_RX_BUF+7,STONER.len-2);
      STONER.value = STONE_RX_BUF[7+STONER.len-2];
			STONER.value = STONER.value<<8 | STONE_RX_BUF[7+STONER.len-1];
      #if print_recive_image_value || print_recive_ALL
			printf("type:image_user;");
			printf("len:");
			printf("%d",STONER.len);
			printf(";widget:");
			for (int wds=0;wds<STONER.len-2;wds++)
			printf("%c",STONER.widget[wds]);
			printf(";value:");
			printf("%d",STONER.value);
			printf(";\r\n");
      #endif
    }break;

    case control_image_value: {
			case_4_byte_ieee_value(m_image_value);
    }break;

		case control_spin_box_text: {
    case_data_text(m_spin_box_text);
    }break;
		case control_spin_box_int: {
      case_4_byte_data(m_spin_box_int,m_value);
    }break;
		case control_spin_box_float: {
			case_4_byte_ieee_value(m_spin_box_float);
    }break;
		case control_combo_box_text: {
			case_data_text(m_combo_box_text);
    }break;
		case control_combo_box_int: {
			case_4_byte_data(m_combo_box_int,m_value);
    }break;
		case control_combo_box_float: {
			case_4_byte_ieee_value(m_combo_box_float);
    }break;
		case control_combo_box_num: {
			case_4_byte_data(m_combo_box_num,m_num);
    }break;
		case control_mledit_text: {
			case_data_text(m_mledit_text);
    }break;
		case control_chart_view_capacity: {
			case_4_byte_data(m_chart_view,m_capacity);
    }break;
		case control_chart_view_value: {
			STONER = widgetinit(STONER.len-5);
			memcpy(STONER.widget,STONE_RX_BUF+7,STONER.len-6);
			STONER.value = STONE_RX_BUF[7+STONER.len-6];
			STONER.value = STONER.value<<8 | STONE_RX_BUF[7+STONER.len-5];
			STONER.float_value = write_hex_to_float(STONE_RX_BUF+7+STONER.len-4);
      #if print_recive_image_value || print_recive_ALL
			printf("type:chart_view;");
			printf("len:");
			printf("%d", STONER.len);
			printf(";widget:");
			for (int wds=0;wds<STONER.len-2;wds++)
				printf("%c", STONER.widget[wds]);
			printf(";num:");
			printf("%d,",STONER.value);
			printf("float_value:");
			printf("%f",STONER.float_value);
			printf(";\r\n");
      #endif
    }break;
		case control_progress_circle_value: {
			case_4_byte_ieee_value(m_progress_circle_value);
    }break;
		case control_progress_circle_p: {
			case_4_byte_data(m_progress_circle_p,m_percent);
    }break;
		case control_digit_clock: {
			case_data_text(m_digit_clock);
    }break;
		case control_hscroll_label: {
			case_4_byte_data(m_hscroll_label,m_value);
    }break;
		case displayed_window: {
			STONER = widgetinit(STONER.len);
			memcpy(STONER.widget,STONE_RX_BUF+7,STONER.len);
			STONER.widget[STONER.len] = 0;
			printf("type:displayed_window;");
			printf("len:");
			printf("%d;", STONER.len);
			printf("widget:%s;\r\n", STONER.widget);
	}break;
		default: {
//			printf("Unknow Command\r\n");
			return;
		}
  }
  recive_group * pSTONER;
  pSTONER = &STONER;
  xQueueSend(Stone_CMD_buf_handle, ( void * ) &pSTONER, (TickType_t)100);
}

void case_btn_switch_ckbtn_rdbtn (char _type){

			STONER = widgetinit(STONER.len);
			memcpy(STONER.widget,STONE_RX_BUF+7,STONER.len-1);
			STONER.value = STONE_RX_BUF[7+STONER.len-1];

      #if print_recive_button || print_recive_ALL
			printf("type:");
			printf("%s", m_static_str[(uint8_t)_type]);
			printf("len:");
			printf("%d", STONER.len);
			printf(";widget:");
			for (int wds=0;wds<STONER.len-1;wds++)
				printf("%c", STONER.widget[wds]);
			printf(";value:");
			printf("%d",STONER.value);
			printf(";\r\n");
      #endif
}

void case_slider_porgressbar (char _type){

			STONER = widgetinit(STONER.len-3);
			memcpy(STONER.widget,STONE_RX_BUF+7,STONER.len-4);
      STONER.float_value = write_hex_to_float(STONE_RX_BUF+7+STONER.len-4);

      #if print_recive_slider || print_recive_ALL
      printf("type:");
      printf("%s",m_static_str[(uint8_t)_type]);
      printf("len:");
      printf("%d", STONER.len);
      printf(";widget:");
			for (int wds=0;wds<STONER.len-4;wds++)
				printf("%c", STONER.widget[wds]);
			printf(";float_value:");
			printf("%f",STONER.float_value);
			printf(";\r\n");
      #endif
}

void case_data_text (char _type){

			STONER = widgetinit(40);
			sscanf((const char*)STONE_RX_BUF+8,"%[^\"]:",STONER.widget);
			widget_len = strlen((char*)STONER.widget);
			STONER = textinit(STONER.len-widget_len-3);
			memcpy(STONER.text,STONE_RX_BUF+10+widget_len,STONER.len-widget_len-3);
			STONER.text[STONER.len-widget_len-3] = 0;
      #if print_recive_label || print_recive_ALL
			printf("type:");
			printf("%s", m_static_str[(uint8_t)_type]);
			printf("len:");
			printf("%d",STONER.len);
			printf(";widget:");
			for (int wds=0;wds<widget_len;wds++)
				printf("%c", STONER.widget[wds]);
			printf(";text:");
			for (int wds=0;wds<STONER.len-widget_len-3;wds++)
				printf("%c",STONER.text[wds]);
			printf(";\r\n");
      #endif
}
void case_4_byte_data (char _type, char _data){

			STONER = widgetinit(STONER.len-3);
			memcpy(STONER.widget,STONE_RX_BUF+7,STONER.len-4);
			STONER.value = STONE_RX_BUF[7+STONER.len-4];
			STONER.value = (STONER.long_value<<8) | STONE_RX_BUF[7+STONER.len-3];
			STONER.value = (STONER.long_value<<8) | STONE_RX_BUF[7+STONER.len-2];
			STONER.value = (STONER.long_value<<8) | STONE_RX_BUF[7+STONER.len-1];
      #if print_recive_progress || print_recive_ALL
			printf("type:");
			printf("%s",m_static_str[(uint8_t)_type]);
			printf("len:");
			printf("%d",STONER.len);
			printf(";widget:");
			for (int wds=0;wds<STONER.len-4;wds++)
				printf("%c", STONER.widget[wds]);
			printf("%s", m_static_str[(uint8_t)_data]);
			printf("%d",STONER.value);
			printf(";\r\n");
      #endif
}

void case_4_byte_ieee_value (char _type){

			STONER = widgetinit(STONER.len-3);
			memcpy(STONER.widget,STONE_RX_BUF+7,STONER.len-4);
			widget_len = strlen((const char*)STONER.widget);
			STONER.float_value = write_hex_to_float(STONE_RX_BUF+7+widget_len);

      #if print_recive_label || print_recive_ALL
			printf("type:");
			printf("%s",(char*) m_static_str[(uint8_t)_type]);
			printf("len:");
			printf("%d",STONER.len);
			printf(";widget:");
			for (int wds=0;wds<STONER.len-4;wds++)
				printf("%c",STONER.widget[wds]);
			printf(";float_value:");
			printf("%f",STONER.float_value);
			printf(";\r\n");
      #endif
}

/* Calculator for transfer HEX data into float type */
float write_hex_to_float(unsigned char* buf)
{
unsigned char i = 0;
unsigned short int dat0;
unsigned short int dat1;
#if INT_MAX==32767
  long int temp_value = 0;
#elif INT_MAX==2147483647
  unsigned int temp_value = 0;
#endif
  temp_value = buf[i];
  temp_value <<= 8;

  temp_value |= buf[++i];
  temp_value <<= 8;

  temp_value |= buf[++i];
  temp_value <<= 8;

  temp_value |= buf[++i];

  dat0 = temp_value & 0x0000FFFF;
  dat1 = (temp_value >> 16) & 0x0000FFFF;
  HexFloat.buf[0] = dat0 & 0xFF;
  HexFloat.buf[1] = (dat0 >> 8) & 0xFF;
  HexFloat.buf[2] = dat1 & 0xFF;
  HexFloat.buf[3] = (dat1 >> 8) & 0xFF;

float fdata = HexFloat.flo;
return fdata;
}
