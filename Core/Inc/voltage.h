#ifndef INC_VOLTAGE_H_
#define INC_VOLTAGE_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"

#define VOLTAGE_CHANNEL_COUNT 4U

HAL_StatusTypeDef Voltage_StartDma(ADC_HandleTypeDef *hadc);
uint8_t Voltage_IsFrameReady(void);
void Voltage_ClearFrameReady(void);
float Get_Voltage(uint8_t channel);


#endif /* INC_VOLTAGE_H_ */
