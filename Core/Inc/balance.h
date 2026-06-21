#ifndef BALANCE_H_
#define BALANCE_H_

#include "main.h"
#include <stdio.h>

#define BALANCE_START_VOLTAGE    3.7f
#define BALANCE_DELTA_START      0.05f   // 50mV

#define BALANCE_STOP_DELTA       0.02f   // 20mV

#define BALANCE_MAX_TEMP         45.0f

void Balance_Init(void);
void Balance_Control(float *cellVoltage, float temperature, float minCell, float maxCell, uint8_t chargeSignal);

uint8_t Balance_GetState(uint8_t cell);

#endif
