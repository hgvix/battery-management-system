/*
 * ADC_Current-Voltage.h
 *
 *  Created on: Jun 5, 2026
 *      Author: WV
 */

#ifndef INC_ADC_CURRENT_VOLTAGE_H_
#define INC_ADC_CURRENT_VOLTAGE_H_

#include <stdio.h>
#include "main.h"

#define ADC_CHANNELS 8
#define ADC_SAMPLES 1000
extern uint16_t adcBuf[ADC_CHANNELS * ADC_SAMPLES];
extern uint16_t adcBufProcess[ADC_CHANNELS * ADC_SAMPLES];
#define sensitivity  0.185
extern float currentOffset;
extern float g_vdda;

extern float Current;

uint16_t ADC_GetAverage(uint8_t channel);
void ADC_UpdateVdda(void);
void ACS712_Calibrate(void);
float ACS712_ReadCurrent(uint8_t ADC_channel, float currentOffset);
float Read_Voltage(uint8_t adcChannel);
void Cell_ReadVoltage(float *cellVoltage, float *batteryVoltage);
void Battery_GetMinMaxCellVoltage(float *cellVoltage, float *minCell, float *maxCell);

#endif /* INC_ADC_CURRENT_VOLTAGE_H_ */
