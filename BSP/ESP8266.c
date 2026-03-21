#include "ESP8266.h"
#include "OLED.h"
#include "stdlib.h"
#include <string.h>
#include <stdio.h>
#include "UART.h"
extern uint8_t uart1_Buff[RXBUFF_MAX_SIZEOF];
extern uint16_t uart1_i;
extern uint8_t rxdata1;
extern UART_HandleTypeDef huart1;
char *ProductKey  = "k2806c8lYSI";
char *DeviceName	= "ESP8266DUAN";
char *Password		= "462a348c42fb8ecd5fb71ea8322fa8f203b0d4f19527452f03e7804690427b75";
char *ClientID		= "k2806c8lYSI.ESP8266DUAN|securemode=2\\,signmethod=hmacsha256\\,timestamp=1743064284900|";
char *MQTTHostURL	= "iot-as-mqtt.cn-shanghai.aliyuncs.com";
char *PropPort		= "/k2806c8lYSI/ESP8266DUAN/user/ESP8266DUAN";	
#define Wifi_Info		"AT+CWJAP=\"DPJ076\",\"12345678\"\r\n"

#define esp8266_cnt  uart1_i
#define esp8266_buf  uart1_Buff
unsigned short esp8266_cntPre = 0;

void ESP8266_Clear(void)
{
	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;
}

void Uart1_SendString(char *s)
{
	HAL_UART_Transmit(&huart1, (uint8_t *)s, strlen(s), 0xffff);
}
uint8_t esp8266_time;
_Bool ESP8266_WaitRecive(void)
{
	if (esp8266_time < 2)
	{
		esp8266_time++;
		return REV_WAIT;
	}
	esp8266_time = 0;
	if(esp8266_cnt == 0) 							//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;
		
	if(esp8266_cnt == esp8266_cntPre)				//如果上一次的值和这次相同，则说明接收完毕
	{
		esp8266_cnt = 0;							//清0接收计数
			
		return REV_OK;								//返回接收完成标志
	}
		
	esp8266_cntPre = esp8266_cnt;					//置为相同
	
	return REV_WAIT;								//返回接收未完成标志

}

_Bool ESP8266_SendCmd(char *cmd, char *res, uint16_t timeOut)
{
	Uart1_SendString(cmd);
	Delay_ms(100);
	while(timeOut --)
	{
		if(ESP8266_WaitRecive() == REV_OK)							//如果收到数据
		{
			if(strstr((const char *)esp8266_buf, res) != NULL)		//如果检索到关键词
			{
				ESP8266_Clear();									//清空缓存
				
				return 0;
			}
		}
		Delay_ms(1);
	}
	return 1;
}
void ESP8266_Init(void)
{
	HAL_UART_Receive_IT(&huart1, &rxdata1, 1);
	while(ESP8266_SendCmd("AT\r\n","OK", 100) == 1);
	
	ESP8266_SendCmd("AT+RST\r\n", "OK", 100);
  Delay_ms(1000);
				
	while(ESP8266_SendCmd("ATE0\r\n","OK", 100) == 1);
	
	while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK", 100) == 1);
				

	while(ESP8266_SendCmd("AT+CIPMUX=0\r\n", "OK", 100) == 1);
				

	while(ESP8266_SendCmd(Wifi_Info, "WIFI GOT IP", 100) == 1);		/* 连接wifi */
	ESP8266_Connect();
}

void ESP8266_Connect(void)
{
	char temp[200];	
	memset(temp, 0, sizeof(temp));
	sprintf(temp, "AT+MQTTUSERCFG=0,1,\"NULL\",\"%s&%s\",\"%s\",0,0,\"\"\r\n", DeviceName, ProductKey, Password);
	while(ESP8266_SendCmd(temp, "OK", 100) == 1);

	memset(temp, 0, sizeof(temp));
	sprintf(temp, "AT+MQTTCLIENTID=0,\"%s\"\r\n", ClientID);
	while(ESP8266_SendCmd(temp, "OK", 100) == 1);
	Delay_ms(200);
	memset(temp, 0, sizeof(temp));
	sprintf(temp, "AT+MQTTCONN=0,\"%s\",1883,1\r\n", MQTTHostURL);
	while(ESP8266_SendCmd(temp, "OK", 100) == 1);

	memset(temp, 0, sizeof(temp));
	sprintf(temp, "AT+MQTTSUB=0,\"/%s/%s/user/get\",1\r\n", ProductKey, DeviceName);
	ESP8266_SendCmd(temp, "OK", 100);
  
  memset(temp, 0, sizeof(temp));
	sprintf(temp, "AT+MQTTSUB=0,\"/sys/%s/%s/thing/service/property/set\",1\r\n", ProductKey, DeviceName);
	while(ESP8266_SendCmd(temp, "OK", 100) == 1);
}

/* 发送数据 */
void ESP8266_SendData(char *str)
{
	char Cmd_Buf[200];
	uint16_t str_len;
	str_len = strlen(str);
	sprintf(Cmd_Buf,"AT+MQTTPUBRAW=0,\"%s\",%u,0,0\r\n",PropPort,str_len);
	while(ESP8266_SendCmd(Cmd_Buf, ">", 100));
	Delay_ms(100);
	ESP8266_SendCmd(str, "MQTTPUB:OK", 1);
	Delay_ms(100);
}
int16_t Received_Data_Processing(const char *str, uint8_t len)						/* 处理接收的数据 */
{
	char temp[10];
	uint8_t temp_i = 0;
	char *p1 = NULL;
	if ((p1 = strstr((char *)esp8266_buf, str)) != NULL)													/* 通过关键字筛选有用的数据 */
	{
		p1 += strlen(str) + 3;
		while(*p1 != ',')
		{
			temp[temp_i++] = *p1;
			p1++;
		}
		return atoi(temp);
	}
	return RXERROR;
}

extern uint8_t price;
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{
  int16_t value;
	while(timeOut--)
	{
		if ((ESP8266_WaitRecive() == REV_OK) && (strstr((char *)esp8266_buf, "MQTTSUBRECV")))
		{
			value = Received_Data_Processing("price", strlen("price"));
			if (value != RXERROR)
				price = value;
			ESP8266_Clear();
		}
		Delay_ms(1);													//延时等待
	};
	
	return NULL;														//超时还未找到，返回空指针

}

