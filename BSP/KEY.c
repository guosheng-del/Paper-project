#include "KEY.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "OLED.h"
#include "servo.h"
extern TIM_HandleTypeDef htim1;
struct keys key[KEY_MAX_NUMBER];
uint8_t key_flag;
extern uint8_t mode;
uint8_t servo;
void Key_Init(void)
{
  HAL_TIM_Base_Start_IT(&htim1);
}

void Key_Scan(void)
{
  if (key[0].key_flag == 1)
  {
		if (mode == 0)
			key_flag = 1;
		else if (mode == 1)
		{
			(servo == 1) ? (servo = 0) : (servo = 1);
			Servo_Start(servo * 90);
			
		}
    key[0].key_flag = 0;
  }
	if (key[1].key_flag == 1)
  {
		OLED_Clear();
		(mode == 1) ? (mode = 0) : (mode = 1);
    key[1].key_flag = 0;
  }
}



