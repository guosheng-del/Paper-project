/* USER CODE BEGIN Header one test */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ESP8266.h"
#include "delay.h"
#include "OLED.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "platenumber.h"
#include "KEY.h"
#include "servo.h"
#include "JQ8900.h"
#include "MFRC522.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern char font[16];
extern char number[7];
extern char plate_number[13];

extern uint8_t key_flag;
extern char font_unicode[5];
extern struct Record_Info rec;

uint8_t send_hour, send_minute, send_sec;
char s[100];
uint16_t money;
uint8_t price = 1;
uint8_t car;
uint8_t car_number1, car_number2;
uint8_t mode;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


struct card_info *card;

struct card_info * card_query(char *str)
{
  struct card_info *temp = card;
  while(temp)
  {
    if (strcmp(str, temp->plate_num) == 0)
    {
      return temp;
    }
    temp = temp->pNext;
  }
  return NULL;
}

void card_add(char *str)
{
  struct card_info *temp = card;
  struct card_info *next = NULL;
  next = malloc(sizeof(struct card_info));
  if (next == NULL)
    return ;
  strcpy(next->plate_num, str);
	next->plate_num[99] = '\0';
  next->pNext = NULL;
  if (temp == NULL)
  {
    card = next;
  }
  else
  {
    while(temp->pNext != NULL)
    {
      temp = temp->pNext;
    }
    temp->pNext = next;
  }
}

uint8_t card_delete(char *str)
{
  struct card_info *temp = card;
  struct card_info *prev = NULL;
  while (temp) 
  {
      if (strcmp(temp->plate_num, str) == 0)
      {
          if (prev == NULL) 
         {
              card = temp->pNext;
          } 
         else 
         {
              prev->pNext = temp->pNext;
          }
				 temp->day = 0;
					temp->hour = 0;
					temp->minute = 0;
					temp->sec = 0;
					temp->pNext = NULL;
          free(temp);
          return 1;
      }
      prev = temp; 
      temp = temp->pNext;
  }
  return 0;
}

void Page(void)
{
	if (mode == 0)
	{
		OLED_ShowChinese(1, 1, "���ƺţ�");
		OLED_ShowChinese(1, 16, "�շѣ�");
		OLED_ShowNum(40, 16, 1, 1, 8);
		OLED_ShowChinese(48, 16, "Сʱ");
		memset(s, 0, sizeof(s));
		sprintf(s, "%d  ", price);
		OLED_ShowString(90, 16, s, 8);
		OLED_ShowChinese(90 + strlen(s) + 16, 16, "Ԫ");
		OLED_ShowChinese(1, 32, "��λһ��");
		OLED_ShowChinese(60, 32, (car_number1) ? "ռ" : "��");
		OLED_ShowChinese(1, 48, "��λ����");
		OLED_ShowChinese(60, 48, (car_number2) ? "ռ" : "��");
	//	OLED_ShowNum(60, 32, car_number1, 1, 0);
	}
	else if (mode == 1)
	{
		OLED_ShowChinese(1, 1, "����ģʽ");
		
	}
	
}

uint8_t send_time;
void Data_Send(uint8_t flag)
{
	memset(s, 0, sizeof(s));
	if (flag == 1)
		sprintf(s, "{\"font\":\"%s\",\"number\":\"%s\",\"flag\":%d}\r\n",font_unicode, number, flag);
	else
		sprintf(s, "{\"font\":\"%s\",\"number\":\"%s\",\"flag\":%d,\"hour\":%d,\"minute\":%d,\"sec\":%d,\"money\":%d}\r\n",font_unicode, number, flag, send_hour, send_minute, send_sec,money);
	ESP8266_SendData(s);
}
char temp_plate[16];
uint8_t plate_flag;
uint8_t rfid_flag;
void Plate_affirm(void)
{	
	struct card_info *temp = NULL;
	char temp_buff[100];
	if (car > 0)
	{
		car_number1 = !HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5);
		car_number2 = !HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4);
	}
	if (MFRC533_GetCard(temp_buff) == 1)
	{
		OLED_ClearArea(60, 1, 68, 16);
		
		memset(plate_number, 0, sizeof(plate_number));
		rfid_flag = 1;
		strcpy(plate_number, temp_buff);
		strcpy(font_unicode, "4eac");
		strcpy(number, temp_buff);
		OLED_ShowString(60, 1, plate_number, 8);
	}
	
	if (((key_flag == 1) && (strcmp(plate_number, "") != 0)) || (rfid_flag == 1))
	{
		plate_number[strlen(plate_number)] = '\0';
		temp = card_query(plate_number);
		if (temp != NULL) 	/* ������ƺ��Ѿ����ڣ���ʾ������ */
		{
			memset(s, 0, sizeof(s));
			sprintf(s, "%02d:%02d:%02d", temp->hour, temp->minute, temp->sec);
			OLED_ClearArea(1, 16, 128, 16);
			OLED_ShowChinese(16, 16, "���ڳ���");
			OLED_ShowChinese(1, 32, "ʱ����");
			OLED_ShowString(40, 32, s, 8);
			rec.index = 2;
			strcpy(rec.path,  "00002");
			rec.volume = 20;
			JQ8900_Play_Recording(&rec);
			money += temp->hour;
			if ((temp->minute !=0) || (temp->sec != 0))
				money += 1;
			money *= price;
			OLED_ShowChinese(1, 48, "���ã�");
			memset(s, 0, sizeof(s));
			sprintf(s, "%d    ", money);
			OLED_ShowString(40, 48, s, 8);
			OLED_ShowChinese(40 + strlen(s) + 5, 48, "Ԫ");
			OLED_Update();
			Servo_Start(90);
			send_hour = temp->hour;
			send_minute = temp->minute;
			send_sec = temp->sec;
			
			Data_Send(0);
			while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == 0);
			Delay_ms(2000);
			Servo_Start(0);
			OLED_ClearArea(16, 16, 64, 16);
			OLED_Update();
			card_delete(plate_number);
			OLED_Clear();
			car--;
			money = 0;
		}
		else	if (car < 50)															/* ������Ʋ����ڣ���ʾ�복�� */
		{
			HAL_TIM_Base_Start_IT(&htim2);
			OLED_ClearArea(1, 16, 128, 16);
			OLED_ClearArea(1, 48, 125, 16);
			OLED_ClearArea(1, 32, 125, 16);
			OLED_ShowChinese(16, 16, "���ڽ���");
			OLED_Update();
			rec.index = 1;
			strcpy(rec.path,  "00001");
			rec.volume = 20;
			JQ8900_Play_Recording(&rec);
			Servo_Start(90);
			Data_Send(1);
			while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == 0);
			Delay_ms(2000);
			Servo_Start(0);
			OLED_ClearArea(16, 16, 64, 16);
			OLED_Update();
			car++;
			card_add(plate_number);
		}
		else if (car >= 50)
		{
			rec.index = 3;
			strcpy(rec.path,  "00003");
			rec.volume = 15;
			JQ8900_Play_Recording(&rec);
		}
		strcpy(temp_plate, plate_number);
		memset(font, 0, sizeof(font));
		memset(number, 0, sizeof(number));
		memset(plate_number, 0, sizeof(plate_number));
		OLED_ClearArea(64, 1, 64, 16);
		key_flag = 0;
		plate_flag = 0;
		rfid_flag = 0;
	}
	else 
	{
		if (Plate_Check(100) == RX_OK)
		{
			if (strcmp(temp_plate, plate_number) == 0)
			{
				memset(temp_plate, 0, sizeof(temp_plate));
				OLED_ClearArea(16, 16, 64, 16);
			}
			else
			{
				OLED_ShowChinese(64, 1, font);
				OLED_ShowString(81, 1, number, 8);
			}
		}
		else if (strcmp(plate_number, "") == 0)
			OLED_ClearArea(16, 16, 64, 16);
	}


}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  OLED_Init();
  ESP8266_Init();
	Plate_Init();
	Key_Init();
	Servo_Start(0);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		ESP8266_GetIPD(100);
		Key_Scan();
		if (mode == 0)
			Plate_affirm();
		Page();
		OLED_Update();
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
