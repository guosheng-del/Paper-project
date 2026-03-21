#include "servo.h"
extern TIM_HandleTypeDef htim4;

void Servo_SetAnagl(float Angal)
{
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, Angal / 180 * 2000 + 500);
}


void Servo_Start(float Angal)
{
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
  Servo_SetAnagl(Angal);
}

void Servo_Stop(void)
{
  HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_2);
}


