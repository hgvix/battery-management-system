#include "battery.h"

typedef struct
{
    float voltage;
    float soc;
} SOC_Table_t;

const SOC_Table_t socTable[] =
{
    {4.20f, 100},
    {4.15f, 95},
    {4.11f, 90},
    {4.08f, 85},
    {4.02f, 80},
    {3.98f, 75},
    {3.95f, 70},
    {3.91f, 65},
    {3.87f, 60},
    {3.85f, 55},
    {3.84f, 50},
    {3.82f, 45},
    {3.80f, 40},
    {3.79f, 35},
    {3.77f, 30},
    {3.75f, 25},
    {3.73f, 20},
    {3.69f, 15},
    {3.61f, 10},
    {3.50f, 5},
    {3.20f, 0}
};

Battery_t battery;
static float dischargedCapacity_mAh = 0.0f;
static uint32_t lastTick = 0;

static float VoltageToSOC(float voltage)
{
    uint8_t size = sizeof(socTable) / sizeof(socTable[0]);

    if(voltage >= socTable[0].voltage)
        return 100.0f;

    if(voltage <= socTable[size - 1].voltage)
        return 0.0f;

    for(uint8_t i = 0; i < size - 1; i++)
    {
        float v1 = socTable[i].voltage;
        float v2 = socTable[i + 1].voltage;

        if(voltage <= v1 && voltage >= v2)
        {
            float soc1 = socTable[i].soc;
            float soc2 = socTable[i + 1].soc;

            return soc1 + (voltage - v1) * (soc2 - soc1) / (v2 - v1);
        }
    }

    return 0.0f;
}

void Battery_Init(float ratedCapacity, float minCellVoltage)
{
    lastTick = HAL_GetTick();

    battery.fullChargedFlag = 0;

    battery.ratedCapacity_mAh = ratedCapacity;

    battery.measuredCapacity_mAh = ratedCapacity;

    battery.soc = VoltageToSOC(minCellVoltage);

    battery.remainingCapacity_mAh = battery.measuredCapacity_mAh * battery.soc / 100.0f;

    battery.soh = 100.0f;
}

void Battery_Update(float current_A, float dt_s)
{
    float delta_mAh;

    delta_mAh = current_A * dt_s * 1000.0f / 3600.0f;

    battery.remainingCapacity_mAh += delta_mAh;

    if(battery.remainingCapacity_mAh < 0)
        battery.remainingCapacity_mAh = 0;

    if(battery.remainingCapacity_mAh > battery.measuredCapacity_mAh)
    {
        battery.remainingCapacity_mAh = battery.measuredCapacity_mAh;
    }

    if(battery.measuredCapacity_mAh > 1.0f)
    {
        battery.soc = battery.remainingCapacity_mAh / battery.measuredCapacity_mAh * 100.0f;
    }
    else
    {
        battery.soc = 0;
    }

    if(battery.soc > 100.0f)
    {
        battery.soc = 100.0f;
    }

    if(battery.soc < 0.0f)
    {
        battery.soc = 0.0f;
    }

    if(current_A < 0)
    {
        dischargedCapacity_mAh += fabs(delta_mAh);
    }
    printf("dt = %.3f\r\n", dt_s);
    printf("delta = %.3f\r\n", delta_mAh);
}


void Battery_Recalibrate(float minCellVoltage)
{
    battery.soc = VoltageToSOC(minCellVoltage);

    battery.remainingCapacity_mAh = battery.measuredCapacity_mAh * battery.soc / 100.0f;
}

void Battery_UpdateSOH(void)
{
    battery.measuredCapacity_mAh = dischargedCapacity_mAh;

    battery.soh = battery.measuredCapacity_mAh / battery.ratedCapacity_mAh * 100.0f;

    if(battery.soh > 100.0f)
        battery.soh = 100.0f;

    dischargedCapacity_mAh = 0;
}

void Battery_Measurement(float current_A, float minCellVoltage, uint8_t chargeSignal)
{
	if(lastTick == 0)
	{
	    lastTick = HAL_GetTick();
	    return;
	}

	uint32_t now = HAL_GetTick();

	float dt_s = (now - lastTick) / 1000.0f;

	lastTick = now;

    Battery_Update(current_A, dt_s);

    /* Phát hiện sạc đầy */
    if(!battery.fullChargedFlag && !chargeSignal && minCellVoltage >= 4.18f && fabs(current_A) < 0.2f)
    {
        battery.remainingCapacity_mAh = battery.measuredCapacity_mAh;

        battery.soc = 100.0f;

        battery.fullChargedFlag = 1;

        dischargedCapacity_mAh = 0;
    }

    if(minCellVoltage <= 2.7f)
    {
        battery.soc = 0.0f;
        battery.remainingCapacity_mAh = 0.0f;
    }

//    /* Hiệu chỉnh SOC khi nghỉ */
//    if(chargeSignal && fabs(current_A) < 0.1f)
//    {
//        Battery_Recalibrate(minCellVoltage);
//    }

    /* Tính SOH sau chu kỳ xả đầy */
    if(battery.fullChargedFlag && minCellVoltage <= 2.7f)
    {
        Battery_UpdateSOH();

        battery.fullChargedFlag = 0;
    }
	printf("SOC: %.1f %%\r\n", battery.soc);

	printf("SOH: %.1f %%\r\n", battery.soh);
}

void Battery_UpdateCellSOC(float *cellVoltage)
{
    for(uint8_t i = 0; i < 4; i++)
    {
        battery.cellSOC[i] = VoltageToSOC(cellVoltage[i]);

        if(battery.cellSOC[i] > 100.0f)
            battery.cellSOC[i] = 100.0f;

        if(battery.cellSOC[i] < 0.0f)
            battery.cellSOC[i] = 0.0f;
    }
}
