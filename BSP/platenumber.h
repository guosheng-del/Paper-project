#ifndef __PLATENUMBER_H
#define __PLATENUMBER_H
#include "stm32f1xx_hal.h"
#define RX_WAIT      0
#define RX_OK					1
struct plate_info
{
	char name[10];
	char unicode[10];
};
void Plate_Init(void);
uint8_t Plate_Check(uint8_t timeout);

uint8_t Plate_Font(char *name, char *unicode);
#endif /* __PLATENUMBER_H */



