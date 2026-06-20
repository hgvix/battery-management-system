#ifndef BATTERY_H
#define BATTERY_H

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "stm32f4xx_hal.h"

typedef struct
{
    float ratedCapacity_mAh;
    float measuredCapacity_mAh;
    float remainingCapacity_mAh;

    float soc;
    float soh;

    float cellSOC[4];

    uint8_t fullChargedFlag;

} Battery_t;

extern Battery_t battery;


void Battery_Init(float ratedCapacity, float minCellVoltage);
void Battery_Update(float current_A, float dt_s);
void Battery_Recalibrate(float minCellVoltage);
void Battery_UpdateSOH(void);
void Battery_Measurement(float current_A, float minCellVoltage, uint8_t chargeSignal);
void Battery_UpdateCellSOC(float *cellVoltage);

#endif
