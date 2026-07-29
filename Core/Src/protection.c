#include <protection.h>

uint8_t chargeEnable = 1;
uint8_t dischargeEnable = 1;

void ChargeMosfet_ON(void)
{
    HAL_GPIO_WritePin(CHARGE_MOSFET_PORT, CHARGE_MOSFET_PIN, GPIO_PIN_SET);
}

void ChargeMosfet_OFF(void)
{
    HAL_GPIO_WritePin(CHARGE_MOSFET_PORT, CHARGE_MOSFET_PIN, GPIO_PIN_RESET);
}

void DischargeMosfet_ON(void)
{
    HAL_GPIO_WritePin(DISCHARGE_MOSFET_PORT, DISCHARGE_MOSFET_PIN, GPIO_PIN_SET);
}

void DischargeMosfet_OFF(void)
{
    HAL_GPIO_WritePin(DISCHARGE_MOSFET_PORT, DISCHARGE_MOSFET_PIN, GPIO_PIN_RESET);
}

void BMS_Protection(float maxTemp, float minTemp, float Current, float minCell, float maxCell, uint8_t chargeSignal)
{
    /* Default State */

    chargeEnable = 1;
    dischargeEnable = 1;

    /* Over Temperature */

    if(maxTemp > OVER_TEMP_THRESHOLD)
    {
        chargeEnable = 0;
        dischargeEnable = 0;

        printf("FAULT: OVER TEMPERATURE\r\n");
    }

    if(minTemp < UNDER_TEMP_THRESHOLD)
    {
        chargeEnable = 0;

        printf("FAULT: UNDER CHARGE TEMPERATURE\r\n");
    }

    /* Over Current */

    /* Over Discharge Current */

    if(Current < -OVER_DISCHARGE_CURRENT_THRESHOLD)
    {
        dischargeEnable = 0;

        printf("FAULT: OVER DISCHARGE CURRENT\r\n");
    }

    /* Over Charge Current */

    if(Current > OVER_CHARGE_CURRENT_THRESHOLD)
    {
        chargeEnable = 0;

        printf("FAULT: OVER CHARGE CURRENT\r\n");
    }

    /* Under battery Voltage */

    if(minCell < UNDER_BATT_VOLT_THRESHOLD)
    {
        dischargeEnable = 0;

        printf("FAULT: UNDER VOLTAGE\r\n");
    }

    /* Over battery Voltage */

    if(maxCell > OVER_BATT_VOLT_THRESHOLD)
    {
        chargeEnable = 0;

        printf("FAULT: OVER VOLTAGE\r\n");
    }

    /* Detect charger */
    if(chargeSignal == 0)
    {
        printf("Charger is connecting\r\n");
    }
    else
    {
        printf("Charger disconnected\r\n");
        chargeEnable = 0;
    }

    /* Apply MOSFET State */

    if(chargeEnable)
    {
        ChargeMosfet_ON();
    }
    else
    {
        ChargeMosfet_OFF();
    }

    if(dischargeEnable)
    {
        DischargeMosfet_ON();
    }
    else
    {
        DischargeMosfet_OFF();
    }

	printf("Charge MOSFET: %d\r\n", chargeEnable);
	printf("Discharge MOSFET: %d\r\n", dischargeEnable);
}
