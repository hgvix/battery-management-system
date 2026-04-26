#ifndef DS18B20_H
#define DS18B20_H

#include "stm32f4xx_hal.h"

uint8_t DS18B20_Start(void);
uint8_t DS18B20_Read(void);
void DS18B20_Write(uint8_t data);
float DS18B20_GetTemp(void);

#endif
