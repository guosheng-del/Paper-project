#include "platenumber.h"
#include "UART.h"
#include "OLED.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
const struct plate_info platebuff[] = {
"¼½","5180",
"½ú","664b",
"ÁÉ","8fbd",
"¼ª","5409",
"ºÚ","9ed1",
"ËÕ","82cf",
"Õã","6d59",
"Íî","7696",
"Ãö","95fd",
"¸Ó","8d63",
"Â³","9c81",
"Ô¥","8c6b",
"¶õ","9102",
"Ïæ","6e58",
"ÔÁ","7ca4",
"Çí","743c",
"´¨","5ddd",
"¹ó","8d35",
"ÔÆ","4e91",
"ÉÂ","9655",
"¸Ê","7518",
"Çà","9752",
"ÃÉ","8499",
"¹ð","6842",
"Äþ","5b81",
"¾©","4eac",
};




extern UART_HandleTypeDef huart3;
extern uint8_t uart3_Buff[RXBUFF_MAX_SIZEOF];
extern uint16_t uart3_i;
extern uint8_t rxdata3;

#define PLATE_BUFF		uart3_Buff
#define PLATE_COUNT		uart3_i

char font_unicode[5];
char font[16];
char number[7];
char plate_number[13];
uint16_t rx_len;
void Plate_Init(void)
{
	 HAL_UART_Receive_IT(&huart3, &rxdata3, 1);
}

uint8_t plate_time;
uint8_t Plate_WaitRx(void)
{
	if (plate_time < 5)
	{
		plate_time++;
		return RX_WAIT;
	}
	plate_time = 0;
	if (PLATE_COUNT == 0)
		return RX_WAIT;
	if (PLATE_COUNT == rx_len)
	{
		rx_len = 0;
		return RX_OK;
	}
	
	rx_len = PLATE_COUNT;
	return RX_WAIT;
}



void Plate_Clear_Buff(void)
{
	uart3_i = 0;
	memset(PLATE_BUFF, 0, sizeof(PLATE_BUFF));
}

uint8_t Plate_Check(uint8_t timeout)
{
	char *p = NULL;
	uint8_t i = 0;
	while(timeout--)
	{
		if (Plate_WaitRx() == RX_OK)
		{
//			OLED_ShowString(1, 32, (char *)PLATE_BUFF, 8);
			p = (char *)PLATE_BUFF;
			while ((PLATE_BUFF[i] < '0') && (PLATE_BUFF[i] > '9')) i++;
			 p += i;
			strncpy(font_unicode, p, 4);
			
			font_unicode[4] = '\0';
			Plate_Font(font, font_unicode);
			p += 4;
			strncpy(number, p, 6);
			number[6] = '\0';
			sprintf(plate_number, "%s%s", font, number);
			plate_number[12] = '\0';
			Plate_Clear_Buff();
			return RX_OK;
		}
	}
	return RX_WAIT;
}

uint8_t i = 0;
uint8_t Plate_Font(char *name, char *unicode)
{
	for (i = 0; i < 26; i++)
	{
		if (strcmp(unicode, platebuff[i].unicode) == 0)
		{
			strcpy(name, platebuff[i].name);
			return 1;
		}
	}
	return 0;
}





