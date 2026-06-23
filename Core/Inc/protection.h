/*
 * battery_functions.h
 *
 *  Created on: May 23, 2026
 *      Author: WV
 */

#ifndef INC_PROTECTION_H_
#define INC_PROTECTION_H_

#include "main.h"
#include <stdio.h>

#define CHARGE_MOSFET_PORT GPIOB
#define CHARGE_MOSFET_PIN  GPIO_PIN_8

#define DISCHARGE_MOSFET_PORT GPIOB
#define DISCHARGE_MOSFET_PIN  GPIO_PIN_9

#define OVER_DISCHARGE_CURRENT_THRESHOLD    3.0f
#define OVER_CHARGE_CURRENT_THRESHOLD       2.6f

#define OVER_TEMP_THRESHOLD      60.0f
#define UNDER_TEMP_THRESHOLD     0.0f

#define OVER_BATT_VOLT_THRESHOLD 4.2f
#define UNDER_BATT_VOLT_THRESHOLD 2.7f

void ChargeMosfet_ON(void);
void ChargeMosfet_OFF(void);
void DischargeMosfet_ON(void);
void DischargeMosfet_OFF(void);
void BMS_Protection(float Temp, float Current, float minCell, float maxCell, uint8_t chargeSignal);

#endif /* INC_PROTECTION_H_ */
