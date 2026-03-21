#include "UART.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

uint8_t uart1_Buff[RXBUFF_MAX_SIZEOF];
uint16_t uart1_i;
uint8_t rxdata1;

uint8_t uart3_Buff[RXBUFF_MAX_SIZEOF];
uint16_t uart3_i;
uint8_t rxdata3;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    uart1_Buff[uart1_i++] = rxdata1;
    HAL_UART_Receive_IT(&huart1, &rxdata1, 1);
    if (uart1_i >= RXBUFF_MAX_SIZEOF)
    {
      uart1_i = 0;
    }
  }
	else if (huart->Instance == USART3)
  {
    uart3_Buff[uart3_i++] = rxdata3;
    HAL_UART_Receive_IT(&huart3, &rxdata3, 1);
    if (uart3_i >= RXBUFF_MAX_SIZEOF)
    {
      uart3_i = 0;
    }
  }
}





